// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#include "GlobalEventBusSubsystem.h"


void UGlobalEventBusSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	RegisterConsoleCommands();
}

void UGlobalEventBusSubsystem::Deinitialize()
{
	GlobalEvents.Empty();
	GlobalPayloadEvents.Empty();
	Super::Deinitialize();
}

bool UGlobalEventBusSubsystem::RegisterGlobalEventAsset(const UObject* WorldContextObject, const UGlobalEvent* Asset,
	const FGlobalEvent& Callback)
{
	if(Asset->IsValidLowLevel())
	{
		if (UGlobalEventBusSubsystem* Manager = UGlobalEventBusSubsystem::Get(WorldContextObject))
		{
			Manager->GlobalEventAssets.Add(Asset->ID, Asset);
		}
		return RegisterGlobalEventID(WorldContextObject, Asset->ID, Callback);
	}
	return false;
}

bool UGlobalEventBusSubsystem::RegisterGlobalEventID(const UObject* WorldContextObject, const FString& ID,
	const FGlobalEvent& Callback)
{
	if (ID.IsEmpty())
	{
		return false;
	}

	if (UGlobalEventBusSubsystem* Manager = UGlobalEventBusSubsystem::Get(WorldContextObject))
	{
		GlobalEventArray& EventArray = Manager->GlobalEvents.FindOrAdd(ID);
		EventArray.Add(Callback);
		return true;
	}

	return false;
}

bool UGlobalEventBusSubsystem::RegisterGlobalPayloadEventAsset(const UObject* WorldContextObject,
	const UGlobalEvent* Asset, const FGlobalEventPayload& Callback)
{
	if(Asset->IsValidLowLevel())
	{
		return RegisterGlobalPayloadEventID(WorldContextObject, Asset->ID, Callback);
	}
	return false;
}

bool UGlobalEventBusSubsystem::RegisterGlobalPayloadEventID(const UObject* WorldContextObject, const FString& ID,
	const FGlobalEventPayload& Callback)
{
	if (ID.IsEmpty())
	{
		return false;
	}

	if (UGlobalEventBusSubsystem* Manager = UGlobalEventBusSubsystem::Get(WorldContextObject))
	{
		PayloadGlobalEventArray& EventArray = Manager->GlobalPayloadEvents.FindOrAdd(ID);
		EventArray.Add(Callback);
		return true;
	}

	return false;
}

bool UGlobalEventBusSubsystem::UnregisterGlobalEvent(const UObject* WorldContextObject, const UGlobalEvent* Asset)
{
	if(Asset->IsValidLowLevel())
	{
		return UnregisterGlobalEventID(WorldContextObject, Asset->ID);
	}
	return false;
}

bool UGlobalEventBusSubsystem::UnregisterGlobalEventID(const UObject* WorldContextObject, const FString& ID)
{
	if (ID.IsEmpty())
	{
		return false;
	}

	if (UGlobalEventBusSubsystem* Manager = UGlobalEventBusSubsystem::Get(WorldContextObject))
	{
		Manager->GlobalEvents.Remove(ID);
		Manager->GlobalPayloadEvents.Remove(ID);
		return true;
	}

	return false;
}

void UGlobalEventBusSubsystem::UnregisterGlobalEvents(const UObject* WorldContextObject)
{
	if (UGlobalEventBusSubsystem* Manager = UGlobalEventBusSubsystem::Get(WorldContextObject))
	{
		Manager->GlobalEvents.Empty();
		Manager->GlobalPayloadEvents.Empty();
	}
}

bool UGlobalEventBusSubsystem::InvokeGlobalEventAsset(const UObject* WorldContextObject, const UGlobalEvent* Asset)
{
	if(Asset->IsValidLowLevel())
	{
		return InvokeGlobalEventID(WorldContextObject, Asset->ID);
	}
	return false;
}

bool UGlobalEventBusSubsystem::InvokeGlobalEventID(const UObject* WorldContextObject, const FString& ID)
{
	if (ID.IsEmpty())
	{
		return false;
	}

	bool bWasExecuted = false;
	if (UGlobalEventBusSubsystem* Manager = UGlobalEventBusSubsystem::Get(WorldContextObject))
	{
		const UGlobalEvent* EventAsset = Manager->GlobalEventAssets.FindOrAdd(ID);
		GlobalEventArray& EventArray = Manager->GlobalEvents.FindOrAdd(ID);
		for (const FGlobalEvent& Event : EventArray)
		{
			bWasExecuted = Event.ExecuteIfBound(EventAsset);
		}
	}

	return bWasExecuted;
}

bool UGlobalEventBusSubsystem::InvokeGlobalPayloadEventAsset(const UObject* WorldContextObject,
	const UGlobalEvent* Asset, const UObject* Payload)
{
	if(Asset->IsValidLowLevel())
	{
		return InvokeGlobalPayloadEventID(WorldContextObject, Asset->ID, Payload);
	}
	return false;
}

bool UGlobalEventBusSubsystem::InvokeGlobalPayloadEventID(const UObject* WorldContextObject, const FString& ID, const UObject* Payload)
{
	if (ID.IsEmpty())
	{
		return false;
	}

	bool bWasExecuted = false;
	if (UGlobalEventBusSubsystem* Manager = UGlobalEventBusSubsystem::Get(WorldContextObject))
	{
		const UGlobalEvent* EventAsset = Manager->GlobalEventAssets.FindOrAdd(ID);
		PayloadGlobalEventArray& EventArray = Manager->GlobalPayloadEvents.FindOrAdd(ID);
		for (const FGlobalEventPayload& Event : EventArray)
		{
			bWasExecuted = Event.ExecuteIfBound(EventAsset, Payload);
		}
	}
	
	return bWasExecuted;
}

void UGlobalEventBusSubsystem::RegisterConsoleCommands()
{
	IConsoleManager& ConsoleManager = IConsoleManager::Get();
	ConsoleManager.RegisterConsoleCommand(
		TEXT("InvokeGlobalEvent"),
		TEXT("Invoke a global event by its ID."),
		FConsoleCommandWithArgsDelegate::CreateUObject(this, &UGlobalEventBusSubsystem::HandleInvokeEvent),
		ECVF_Cheat
	);
}

void UGlobalEventBusSubsystem::HandleInvokeEvent(const TArray<FString>& Args)
{
	if (Args.Num() < 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("InvokeGlobalEvent: Missing event ID argument."));
		return;
	}

	const FString EventID = Args[0];
	const UObject* WorldContextObject = GEngine->GetWorldFromContextObjectChecked(this);

	if (!InvokeGlobalEventID(WorldContextObject, EventID))
	{
		UE_LOG(LogTemp, Warning, TEXT("InvokeGlobalEvent: Failed to invoke event with ID '%s'"), *EventID);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("InvokeGlobalEvent: Successfully invoked event with ID '%s'"), *EventID);
	}
}

UGlobalEventBusSubsystem* UGlobalEventBusSubsystem::Get(const UObject* WorldContextObject)
{
	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return nullptr;
	}

	UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance)
	{
		return nullptr;
	}
	
	return World->GetGameInstance()->GetSubsystem<UGlobalEventBusSubsystem>();
}
