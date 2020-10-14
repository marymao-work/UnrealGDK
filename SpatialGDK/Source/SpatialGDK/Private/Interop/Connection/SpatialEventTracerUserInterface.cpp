// Copyright (c) Improbable Worlds Ltd, All Rights Reserved

#include "Interop/Connection/SpatialEventTracerUserInterface.h"

#include "EngineClasses/SpatialNetDriver.h"
#include "Interop/Connection/SpatialEventTracer.h"
#include "Interop/Connection/SpatialWorkerConnection.h"

FString USpatialEventTracerUserInterface::CreateSpanId(UObject* WorldContextObject)
{
	SpatialGDK::SpatialEventTracer* EventTracer = GetEventTracer(WorldContextObject);
	if (EventTracer == nullptr)
	{
		return {};
	}

	return SpatialGDK::SpatialEventTracer::SpanIdToString(EventTracer->CreateSpan().GetValue());
}

FString USpatialEventTracerUserInterface::CreateSpanIdWithCauses(UObject* WorldContextObject, TArray<FString> Causes)
{
	SpatialGDK::SpatialEventTracer* EventTracer = GetEventTracer(WorldContextObject);
	if (EventTracer == nullptr)
	{
		return {};
	}

	TArray<Trace_SpanId> SpanIdS;
	for (const FString& String : Causes)
	{
		SpanIdS.Add(SpatialGDK::SpatialEventTracer::StringToSpanId(String));
	}

	return SpatialGDK::SpatialEventTracer::SpanIdToString(EventTracer->CreateSpan(SpanIdS.GetData(), SpanIdS.Num()).GetValue());
}

void USpatialEventTracerUserInterface::TraceEvent(UObject* WorldContextObject, FSpatialTraceEvent SpatialTraceEvent, const FString& SpanId)
{
	SpatialGDK::SpatialEventTracer* EventTracer = GetEventTracer(WorldContextObject);
	if (EventTracer == nullptr)
	{
		return;
	}

	EventTracer->TraceEvent(SpatialTraceEvent, SpatialGDK::SpatialEventTracer::StringToSpanId(SpanId));
}

void USpatialEventTracerUserInterface::SetActiveSpanId(UObject* WorldContextObject, FEventTracerDynamicDelegate Delegate, const FString& SpanId)
{
	SpatialGDK::SpatialEventTracer* EventTracer = GetEventTracer(WorldContextObject);
	if (EventTracer == nullptr)
	{
		return;
	}

	Trace_SpanId NewSpanId = SpatialGDK::SpatialEventTracer::StringToSpanId(SpanId);
	FString NewString = SpatialGDK::SpatialEventTracer::SpanIdToString(NewSpanId);

	EventTracer->SpanIdStack.AddNewLayer(SpatialGDK::SpatialEventTracer::StringToSpanId(SpanId));
	Delegate.Execute();
	EventTracer->SpanIdStack.PopLayer();
}

SpatialGDK::SpatialEventTracer* USpatialEventTracerUserInterface::GetEventTracer(UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
	if (World == nullptr)
	{
		World = GWorld;
	}

	USpatialNetDriver* NetDriver = Cast<USpatialNetDriver>(World->GetNetDriver());
	if (NetDriver != nullptr && NetDriver->Connection != nullptr)
	{
		return NetDriver->Connection->GetEventTracer();
	}
	return nullptr;
}
