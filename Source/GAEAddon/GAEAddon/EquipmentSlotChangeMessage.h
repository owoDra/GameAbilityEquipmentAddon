// Copyright (C) 2024 owoDra

#pragma once

#include "GameplayTagContainer.h"

#include "EquipmentSlotChangeMessage.generated.h"

class UEquipmentManagerComponent;
class UEquipmentData;
class UEquipmentInstance;


/**
 * FBEEquipmentSlotChangedMessage
 *
 * EquipmentManagerComponent に登録した Equipment が変更されたときのメッセージ
 */
USTRUCT(BlueprintType)
struct GAEADDON_API FEquipmentSlotChangedMessage
{
	GENERATED_BODY()
public:
	FEquipmentSlotChangedMessage() {}

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UEquipmentManagerComponent> OwnerComponent{ nullptr };

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTag SlotTag{ FGameplayTag::EmptyTag };

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<const UEquipmentData> Data{ nullptr };

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UEquipmentInstance> Instance{ nullptr };

};
