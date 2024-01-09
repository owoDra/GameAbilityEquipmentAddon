// Copyright (C) 2024 owoDra

#pragma once

#include "Cost/AbilityCost.h"

#include "AbilityCost_EquipmentStatTag.generated.h"


/**
 * AbilityCost class of the type that consumes the OwningActor or AvatarActor's StatTag
 */
UCLASS(DefaultToInstanced, EditInlineNew, meta = (DisplayName = "Cost Equipment Stat Tag"))
class GAEADDON_API UAbilityCost_EquipmentStatTag : public UAbilityCost
{
	GENERATED_BODY()
public:
	UAbilityCost_EquipmentStatTag(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	//
	// How much of the tag to spend (keyed on ability level)
	//
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Costs")
	FScalableFloat Cost{ 1.0f };

	//
	// Which tag to spend some of
	//
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Costs", meta = (Categories = "Stat.Equipment"))
	FGameplayTag ConsumeTag;

public:
	virtual bool CheckCost(
		const UGAEGameplayAbility* Ability
		, const FGameplayAbilitySpecHandle Handle
		, const FGameplayAbilityActorInfo* ActorInfo
		, FGameplayTagContainer* OptionalRelevantTags) const override;

	virtual void ApplyCost(
		const UGAEGameplayAbility* Ability
		, const FGameplayAbilitySpecHandle Handle
		, const FGameplayAbilityActorInfo* ActorInfo
		, const FGameplayAbilityActivationInfo ActivationInfo) override;

};
