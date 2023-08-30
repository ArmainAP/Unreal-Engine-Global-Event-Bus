﻿// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#include "GlobalEventBusSubsystem.h"

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

	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	UGlobalEventBusSubsystem* Manager = World->GetGameInstance()->GetSubsystem<UGlobalEventBusSubsystem>();
	GlobalEventArray& EventArray = Manager->GlobalEvents.FindOrAdd(ID);
	EventArray.Add(Callback);
	return true;
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

	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	UGlobalEventBusSubsystem* Manager = World->GetGameInstance()->GetSubsystem<UGlobalEventBusSubsystem>();
	PayloadGlobalEventArray& EventArray = Manager->GlobalPayloadEvents.FindOrAdd(ID);
	EventArray.Add(Callback);
	return true;
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

	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	UGlobalEventBusSubsystem* Manager = World->GetGameInstance()->GetSubsystem<UGlobalEventBusSubsystem>();
	Manager->GlobalEvents.Remove(ID);
	Manager->GlobalPayloadEvents.Remove(ID);
	return true;
}

void UGlobalEventBusSubsystem::UnregisterGlobalEvents(const UObject* WorldContextObject)
{
	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	UGlobalEventBusSubsystem* Manager = World->GetGameInstance()->GetSubsystem<UGlobalEventBusSubsystem>();
	Manager->GlobalEvents.Empty();
	Manager->GlobalPayloadEvents.Empty();
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

	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	UGlobalEventBusSubsystem* Manager = World->GetGameInstance()->GetSubsystem<UGlobalEventBusSubsystem>();

	bool bWasExecuted = false;
	GlobalEventArray& EventArray = Manager->GlobalEvents.FindOrAdd(ID);
	for (const FGlobalEvent& Event : EventArray)
	{
		bWasExecuted = Event.ExecuteIfBound();
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

	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	UGlobalEventBusSubsystem* Manager = World->GetGameInstance()->GetSubsystem<UGlobalEventBusSubsystem>();

	bool bWasExecuted = false;
	PayloadGlobalEventArray& EventArray = Manager->GlobalPayloadEvents.FindOrAdd(ID);
	for (const FGlobalEventPayload& Event : EventArray)
	{
		bWasExecuted = Event.ExecuteIfBound(Payload);
	}
	return bWasExecuted;
}
