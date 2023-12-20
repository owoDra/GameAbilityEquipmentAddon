// Copyright (C) 2023 owoDra

#pragma once

#include "Engine/DataAsset.h"

#include "GameplayTagContainer.h"

#include "EquipmentSet.generated.h"

class UEquipmentData;


/**
 * Used to define the EquipmentData and Slot to be added in the EquipmentSet.
 */
USTRUCT(BlueprintType)
struct FEquipmentSetEntry
{
	GENERATED_BODY()
public:
	FEquipmentSetEntry() {}

public:
	UPROPERTY(EditAnywhere, meta = (Categories = "Equipment.Slot"))
	FGameplayTag SlotTag;

	UPROPERTY(EditAnywhere)
	TObjectPtr<const UEquipmentData> EquipmentData{ nullptr };

};


/**
 * Data asset used to collectively add Equipment to EquipmentComponent
 */
UCLASS(BlueprintType, Const)
class UEquipmentSet : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UEquipmentSet() {}
	
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(TArray<FText>& ValidationErrors) override;
#endif // WITH_EDITOR

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	TArray<FEquipmentSetEntry> Entries;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment", meta = (Categories = "Equipment.Slot"))
	FGameplayTag DefaultActiveSlotTag;

};
