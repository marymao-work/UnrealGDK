// Copyright (c) Improbable Worlds Ltd, All Rights Reserved

#include "Utils/EntityPool.h"

#include "Interop/SpatialReceiver.h"
#include "Interop/SpatialSender.h"
#include "SpatialGDKSettings.h"

#include "TimerManager.h"

DEFINE_LOG_CATEGORY(LogSpatialEntityPool);

using namespace SpatialGDK;

void UEntityPool::Init(USpatialNetDriver* InNetDriver, FTimerManager* InTimerManager)
{
	NetDriver = InNetDriver;
	Receiver = InNetDriver->Receiver;
	TimerManager = InTimerManager;

	SubView = &InNetDriver->Connection->GetCoordinator().CreateSubView(SpatialConstants::INVALID_QUERY_TAG, FSubView::NoFilter,
																	   FSubView::NoDispatcherCallbacks);

	ReserveEntityIDs(GetDefault<USpatialGDKSettings>()->EntityPoolInitialReservationCount);
}

void UEntityPool::ReserveEntityIDs(uint32 EntitiesToReserve)
{
	UE_LOG(LogSpatialEntityPool, Verbose, TEXT("Sending bulk entity ID Reservation Request for %d IDs"), EntitiesToReserve);

	checkf(!bIsAwaitingResponse, TEXT("Trying to reserve Entity IDs while another reserve request is in flight"));

	// TODO: UNR-4979 Allow full range of uint32 when SQD-1150 is fixed
	const uint32 TempMaxEntitiesToReserve = static_cast<uint32>(MAX_int32);
	if (EntitiesToReserve > TempMaxEntitiesToReserve)
	{
		UE_LOG(LogSpatialEntityPool, Log, TEXT("Clamping requested 'EntitiesToReserve' to MAX_int32 (from %u to %d)"), EntitiesToReserve,
			   MAX_int32);
		EntitiesToReserve = TempMaxEntitiesToReserve;
	}

	// Set up reserve IDs delegate
	ReserveEntityIDsDelegate CacheEntityIDsDelegate;
	CacheEntityIDsDelegate.BindLambda([EntitiesToReserve, this](const Worker_ReserveEntityIdsResponseOp& Op) {
		bIsAwaitingResponse = false;
		if (Op.status_code != WORKER_STATUS_CODE_SUCCESS)
		{
			// UNR-630 - Temporary hack to avoid failure to reserve entities due to timeout on large maps
			if (Op.status_code == WORKER_STATUS_CODE_TIMEOUT)
			{
				UE_LOG(LogSpatialEntityPool, Warning, TEXT("Failed to reserve entity IDs Reason: %s. Retrying..."),
					   UTF8_TO_TCHAR(Op.message));
				ReserveEntityIDs(EntitiesToReserve);
			}
			else
			{
				UE_LOG(LogSpatialEntityPool, Error, TEXT("Failed to reserve entity IDs Reason: %s."), UTF8_TO_TCHAR(Op.message));
			}

			return;
		}

		// Ensure we received the same number of reserved IDs as we requested
		check(EntitiesToReserve == Op.number_of_entity_ids);

		// Clean up any expired Entity ranges
		ReservedEntityIDRanges = ReservedEntityIDRanges.FilterByPredicate([](const EntityRange& Element) {
			return !Element.bExpired;
		});

		EntityRange NewEntityRange = {};
		NewEntityRange.CurrentEntityId = Op.first_entity_id;
		NewEntityRange.LastEntityId = Op.first_entity_id + (Op.number_of_entity_ids - 1);
		NewEntityRange.EntityRangeId = NextEntityRangeId++;

		UE_LOG(LogSpatialEntityPool, Verbose, TEXT("Reserved %d entities, caching in pool, Entity IDs: (%d, %d) Range ID: %d"),
			   Op.number_of_entity_ids, Op.first_entity_id, NewEntityRange.LastEntityId, NewEntityRange.EntityRangeId);

		ReservedEntityIDRanges.Add(NewEntityRange);

		FTimerHandle ExpirationTimer;
		TWeakObjectPtr<UEntityPool> WeakThis(this);
		TimerManager->SetTimer(
			ExpirationTimer,
			[WeakThis, ExpiringEntityRangeId = NewEntityRange.EntityRangeId]() {
				if (UEntityPool* Pool = WeakThis.Get())
				{
					Pool->OnEntityRangeExpired(ExpiringEntityRangeId);
				}
			},
			SpatialConstants::ENTITY_RANGE_EXPIRATION_INTERVAL_SECONDS, false);

		if (!bIsReady)
		{
			bIsReady = true;
			EntityPoolReadyDelegate.Broadcast();
		}
	});

	// Reserve the Entity IDs
	const Worker_RequestId ReserveRequestID = NetDriver->Connection->SendReserveEntityIdsRequest(EntitiesToReserve, RETRY_UNTIL_COMPLETE);
	bIsAwaitingResponse = true;
	// Add the spawn delegate
	ReserveEntityHandler.AddRequest(ReserveRequestID, CacheEntityIDsDelegate);
}

void UEntityPool::Advance()
{
	ReserveEntityHandler.ProcessOps(*SubView->GetViewDelta().WorkerMessages);
}

void UEntityPool::OnEntityRangeExpired(uint32 ExpiringEntityRangeId)
{
	UE_LOG(LogSpatialEntityPool, Verbose, TEXT("Entity range expired! Range ID: %d"), ExpiringEntityRangeId);

	int32 FoundEntityRangeIndex = ReservedEntityIDRanges.IndexOfByPredicate([ExpiringEntityRangeId](const EntityRange& Element) {
		return Element.EntityRangeId == ExpiringEntityRangeId;
	});

	if (FoundEntityRangeIndex == INDEX_NONE)
	{
		// This entity range has already been cleaned up as a result of running out of Entity IDs.
		UE_LOG(LogSpatialEntityPool, Verbose, TEXT("Entity range ID: %d has already been depleted"), ExpiringEntityRangeId);
		return;
	}

	if (FoundEntityRangeIndex < ReservedEntityIDRanges.Num() - 1)
	{
		// This is not the most recent entity range, just clean up without requesting additional IDs.
		UE_LOG(LogSpatialEntityPool, Verbose, TEXT("Newer range detected, cleaning up Entity range ID: %d without new request"),
			   ExpiringEntityRangeId);
		ReservedEntityIDRanges.RemoveAt(FoundEntityRangeIndex);
	}
	else
	{
		// Reserve then cleanup
		if (!bIsAwaitingResponse)
		{
			UE_LOG(LogSpatialEntityPool, Verbose, TEXT("Reserving new Entity range to replace Entity range ID: %d"), ExpiringEntityRangeId);
			ReserveEntityIDs(GetDefault<USpatialGDKSettings>()->EntityPoolRefreshCount);
		}
		// Mark this entity range as expired, so it gets cleaned up when we receive a new entity range from Spatial.
		ReservedEntityIDRanges[FoundEntityRangeIndex].bExpired = true;
	}
}

Worker_EntityId UEntityPool::GetNextEntityId()
{
	if (ReservedEntityIDRanges.Num() == 0)
	{
		// TODO: Improve error message
		UE_LOG(LogSpatialEntityPool, Warning,
			   TEXT("Tried to pop an entity ID from the pool when there were no entity IDs. Try altering your Entity Pool configuration"));
		return SpatialConstants::INVALID_ENTITY_ID;
	}

	EntityRange& CurrentEntityRange = ReservedEntityIDRanges[0];
	Worker_EntityId NextId = CurrentEntityRange.CurrentEntityId++;

	uint32_t TotalRemainingEntityIds = 0;
	for (EntityRange Range : ReservedEntityIDRanges)
	{
		TotalRemainingEntityIds += Range.LastEntityId - Range.CurrentEntityId + 1;
	}

	UE_LOG(LogSpatialEntityPool, Verbose, TEXT("Popped ID, %i IDs remaining"), TotalRemainingEntityIds);

	if (TotalRemainingEntityIds < GetDefault<USpatialGDKSettings>()->EntityPoolRefreshThreshold && !bIsAwaitingResponse)
	{
		UE_LOG(LogSpatialEntityPool, Verbose, TEXT("Pool under threshold, reserving more entity IDs"));
		ReserveEntityIDs(GetDefault<USpatialGDKSettings>()->EntityPoolRefreshCount);
	}

	if (CurrentEntityRange.CurrentEntityId > CurrentEntityRange.LastEntityId)
	{
		ReservedEntityIDRanges.RemoveAt(0);
	}

	return NextId;
}

FEntityPoolReadyEvent& UEntityPool::GetEntityPoolReadyDelegate()
{
	return EntityPoolReadyDelegate;
}
