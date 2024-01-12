// Copyright (C) 2024 owoDra

#pragma once

#include "Fragment/EquipmentFragmentBase.h"

#include "EquipmentFragment_AddAbilities.generated.h"

class UAbilitySet;



/**
 * Class that defines the abilities granted to a character when equiping or activating equipment
 */
UCLASS(meta = (DisplayName = "EF Add Abilities"))
class UEquipmentFragment_AddAbilities : public UEquipmentFragmentBase
{
	GENERATED_BODY()
public:
	UEquipmentFragment_AddAbilities(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	//
	// List of abilities that will be granted to pawn when equipped.
	//
	UPROPERTY(EditDefaultsOnly, Category = "OnEquiped", meta = (TitleProperty = Ability))
	TArray<FAbilitySet_GameplayAbility> GrantedGameplayAbilities_OnEquiped;

	//
	// List of effects that will be granted to pawn when equipped.
	//
	UPROPERTY(EditDefaultsOnly, Category = "OnEquiped", meta = (TitleProperty = GameplayEffect))
	TArray<FAbilitySet_GameplayEffect> GrantedGameplayEffects_OnEquiped;

	//
	// List of attribute sets that will be granted to pawn when equipped.
	//
	UPROPERTY(EditDefaultsOnly, Category = "OnEquiped", meta = (TitleProperty = AttributeSet))
	TArray<FAbilitySet_AttributeSet> GrantedAttributes_OnEquiped;

public:
	//
	// List of abilities that will be granted to pawn when activated.
	//
	UPROPERTY(EditDefaultsOnly, Category = "Activated", meta = (TitleProperty = Ability))
	TArray<FAbilitySet_GameplayAbility> GrantedGameplayAbilities_OnActivated;

	//
	// List of effects that will be granted to pawn when activated.
	//
	UPROPERTY(EditDefaultsOnly, Category = "Activated", meta = (TitleProperty = GameplayEffect))
	TArray<FAbilitySet_GameplayEffect> GrantedGameplayEffects_OnActivated;

	//
	// List of attribute sets that will be granted to pawn when activated.
	//
	UPROPERTY(EditDefaultsOnly, Category = "Activated", meta = (TitleProperty = AttributeSet))
	TArray<FAbilitySet_AttributeSet> GrantedAttributes_OnActivated;


public:
	virtual void OnEquiped(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const override;
	virtual void OnUnequiped(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const override;
	virtual void OnActivated(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const override;
	virtual void OnDeactivated(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const override;

};
