// Copyright (c) Improbable Worlds Ltd, All Rights Reserved
#pragma once

#include "EngineClasses/AbstractPackageMap.h"
#include "WorkerSDK/improbable/c_worker.h"

#include "SpatialPackageMapClientMock.generated.h"

UCLASS()
class USpatialPackageMapClientMock : public UObject, public AbstractPackageMap
{
	GENERATED_BODY()
public:

	void Init(Worker_EntityId inReturnEntityId);

	virtual Worker_EntityId GetEntityIdFromObject(const UObject* Object) override;

private:
	Worker_EntityId ReturnEntityId;
}; 