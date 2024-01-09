// Copyright (C) 2024 owoDra

#pragma once

#include "Fragment/EquipmentFragmentBase.h"

#include "EquipmentFragment_SetTagStats.generated.h"

class UAbilitySet;


/**
 * Initialize TagStats of equipment when equipped.
 */
UCLASS(meta = (DisplayName = "EF Set Tag Stats"))
class UEquipmentFragment_SetTagStats : public UEquipmentFragmentBase
{
	GENERATED_BODY()
public:
	UEquipmentFragment_SetTagStats(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	//
	// Initial StatTag for this Equipment
	//
	UPROPERTY(EditDefaultsOnly, Category = "SetTagStats")
	TMap<FGameplayTag, int32> InitialEquipmentStats;

public:
	virtual void OnEquiped(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const override;

};
