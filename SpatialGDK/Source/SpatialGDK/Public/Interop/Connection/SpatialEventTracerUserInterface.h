// Copyright (c) Improbable Worlds Ltd, All Rights Reserved

#pragma once

#include "Interop/Connection/SpatialTraceEvent.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include <WorkerSDK/improbable/c_trace.h>

#include "SpatialEventTracerUserInterface.generated.h"

DECLARE_DYNAMIC_DELEGATE(FEventTracerDynamicDelegate);

namespace SpatialGDK
{
class SpatialEventTracer;
}

UCLASS()
class SPATIALGDK_API USpatialEventTracerUserInterface : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "SpatialOS|EventTracing", meta = (WorldContext = "WorldContextObject"))
	static FString CreateSpanId(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "SpatialOS|EventTracing", meta = (WorldContext = "WorldContextObject"))
	static FString CreateSpanIdWithCauses(UObject* WorldContextObject, TArray<FString> Causes);

	UFUNCTION(BlueprintCallable, Category = "SpatialOS|EventTracing", meta = (WorldContext = "WorldContextObject"))
	static void TraceEvent(UObject* WorldContextObject, FSpatialTraceEvent SpatialTraceEvent, const FString& SpanId);

	UFUNCTION(BlueprintCallable, Category = "SpatialOS|EventTracing", meta = (WorldContext = "WorldContextObject"))
	static void SetActiveSpanId(UObject* WorldContextObject, FEventTracerDynamicDelegate Delegate, const FString& SpanId);

private:
	static SpatialGDK::SpatialEventTracer* GetEventTracer(UObject* WorldContextObject);
};
