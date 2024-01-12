// Copyright (C) 2024 owoDra

#pragma once

#include "Fragment/EquipmentFragmentBase.h"

#include "EquipmentFragment_AddAbilitySets.generated.h"

class UAbilitySet;



/**
 * Class that defines the abilities granted to a character when equiping or activating equipment
 */
UCLASS(meta = (DisplayName = "EF Add Ability Sets"))
class UEquipmentFragment_AddAbilitySets : public UEquipmentFragmentBase
{
	GENERATED_BODY()
public:
	UEquipmentFragment_AddAbilitySets(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	//
	// List of ability sets that will be granted to pawn when equipped.
	//
	UPROPERTY(EditDefaultsOnly, Category = "AddAbilitySets")
	TArray<TObjectPtr<const UAbilitySet>> AbilitySetsToGrantOnEquip;

	//
	// List of ability sets that will be granted to pawn when active
	//
	UPROPERTY(EditDefaultsOnly, Category = "AddAbilitySets")
	TArray<TObjectPtr<const UAbilitySet>> AbilitySetsToGrantOnActive;

public:
	virtual void OnEquiped(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const override;
	virtual void OnUnequiped(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const override;
	virtual void OnActivated(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const override;
	virtual void OnDeactivated(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const override;

};
