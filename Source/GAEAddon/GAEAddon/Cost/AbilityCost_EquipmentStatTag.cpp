// Copyright (C) 2023 owoDra

#include "AbilityCost_EquipmentStatTag.h"

#include "EquipmentInstance.h"

#include "GAEGameplayAbility.h"

#include "GameplayTag/GameplayTagStackInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AbilityCost_EquipmentStatTag)


UAbilityCost_EquipmentStatTag::UAbilityCost_EquipmentStatTag(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


bool UAbilityCost_EquipmentStatTag::CheckCost(const UGAEGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	// Returns false if EquipmentInstance is not inherited

	auto* SourceObject{ Ability->GetSourceObject(Handle, ActorInfo) };

	if (!SourceObject->IsA<UEquipmentInstance>())
	{
		return false;
	}

	// Check the number of tags

	if (auto* Interface{ Cast<IGameplayTagStackInterface>(SourceObject) })
	{
		const auto AbilityLevel{ Ability->GetAbilityLevel(Handle, ActorInfo) };
		const auto NumStacksReal{ Cost.GetValueAtLevel(AbilityLevel) };
		const auto NumStacks{ FMath::TruncToInt(NumStacksReal) };

		return (Interface->GetStatTagStackCount(ConsumeTag) >= NumStacks);
	}
	
	return false;
}

void UAbilityCost_EquipmentStatTag::ApplyCost(const UGAEGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	// if EquipmentInstance is not inherited, skip

	auto* SourceObject{ Ability->GetSourceObject(Handle, ActorInfo) };

	if (!SourceObject->IsA<UEquipmentInstance>())
	{
		return;
	}

	// Modify the number of tags

	if (auto* Interface{ Cast<IGameplayTagStackInterface>(SourceObject) })
	{
		const auto AbilityLevel{ Ability->GetAbilityLevel(Handle, ActorInfo) };
		const auto NumStacksReal{ Cost.GetValueAtLevel(AbilityLevel) };
		const auto NumStacks{ FMath::TruncToInt(NumStacksReal) };

		Interface->RemoveStatTagStack(ConsumeTag, NumStacks);
	}
}
