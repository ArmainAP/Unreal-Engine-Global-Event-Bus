// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "GlobalEvent.h"
#include "GlobalEventBusSubsystem.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FGlobalEvent, const UGlobalEvent*, Event);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FGlobalEventPayload, const UGlobalEvent*, Event, const UObject*, Payload);

UCLASS()
class GLOBALEVENTBUS_API UGlobalEventBusSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "Global Event Bus", meta = (WorldContext = "WorldContextObject"))
	static bool RegisterGlobalEventAsset(const UObject* WorldContextObject, const UGlobalEvent* Asset, const FGlobalEvent& Callback);

	UFUNCTION(BlueprintCallable, Category = "Global Event Bus", meta = (WorldContext = "WorldContextObject"))
	static bool RegisterGlobalEventID(const UObject* WorldContextObject, const FString& ID, const FGlobalEvent& Callback);

	UFUNCTION(BlueprintCallable, Category = "Global Event Bus", meta = (WorldContext = "WorldContextObject"))
	static bool RegisterGlobalPayloadEventAsset(const UObject* WorldContextObject, const UGlobalEvent* Asset, const FGlobalEventPayload& Callback);

	UFUNCTION(BlueprintCallable, Category = "Global Event Bus", meta = (WorldContext = "WorldContextObject"))
	static bool RegisterGlobalPayloadEventID(const UObject* WorldContextObject, const FString& ID, const FGlobalEventPayload& Callback);

	UFUNCTION(BlueprintCallable, Category = "Global Event Bus", meta = (WorldContext = "WorldContextObject"))
	static bool UnregisterGlobalEvent(const UObject* WorldContextObject, const UGlobalEvent* Asset);

	UFUNCTION(BlueprintCallable, Category = "Global Event Bus", meta = (WorldContext = "WorldContextObject"))
	static bool UnregisterGlobalEventID(const UObject* WorldContextObject, const FString& ID);

	UFUNCTION(BlueprintCallable, Category = "Global Event Bus", meta = (WorldContext = "WorldContextObject"))
	static void UnregisterGlobalEvents(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Global Event Bus", meta = (WorldContext = "WorldContextObject"))
	static bool InvokeGlobalEventAsset(const UObject* WorldContextObject, const UGlobalEvent* Asset);

	UFUNCTION(BlueprintCallable, Category = "Global Event Bus", meta = (WorldContext = "WorldContextObject"))
	static bool InvokeGlobalEventID(const UObject* WorldContextObject, const FString& ID);

	UFUNCTION(BlueprintCallable, Category = "Global Event Bus", meta = (WorldContext = "WorldContextObject"))
	static bool InvokeGlobalPayloadEventAsset(const UObject* WorldContextObject, const UGlobalEvent* Asset, const UObject* Payload);

	UFUNCTION(BlueprintCallable, Category = "Global Event Bus", meta = (WorldContext = "WorldContextObject"))
	static bool InvokeGlobalPayloadEventID(const UObject* WorldContextObject, const FString& ID, const UObject* Payload);

private:
	void RegisterConsoleCommands();
	void HandleInvokeEvent(const TArray<FString>& Args);
	static UGlobalEventBusSubsystem* Get(const UObject* WorldContextObject);

protected:
	UPROPERTY()
	TMap<FString, const UGlobalEvent*> GlobalEventAssets;
	
	typedef TArray<FGlobalEvent> GlobalEventArray;
	TMap<FString, GlobalEventArray> GlobalEvents;

	typedef TArray<FGlobalEventPayload> PayloadGlobalEventArray;
	TMap<FString, PayloadGlobalEventArray> GlobalPayloadEvents;
};