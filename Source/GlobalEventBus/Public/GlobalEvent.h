// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "GlobalEvent.generated.h"

UCLASS(Blueprintable, BlueprintType)
class GLOBALEVENTBUS_API UGlobalEvent : public UDataAsset
{
	GENERATED_BODY()

public:
	UGlobalEvent() : ID(FGuid::NewGuid().ToString()) {}

	UPROPERTY(EditDefaultsOnly)
	FString ID = FGuid(0, 0, 0, 0).ToString();
};