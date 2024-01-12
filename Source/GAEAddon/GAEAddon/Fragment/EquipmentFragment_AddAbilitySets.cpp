// Copyright (C) 2024 owoDra

#include "EquipmentFragment_AddAbilitySets.h"

#include "EquipmentManagerComponent.h"
#include "EquipmentInstance.h"

#include "AbilitySet.h"

#include "AbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EquipmentFragment_AddAbilitySets)


UEquipmentFragment_AddAbilitySets::UEquipmentFragment_AddAbilitySets(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


void UEquipmentFragment_AddAbilitySets::OnEquiped(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const
{
	check(EMC);
	check(Instance);

	auto* Pawn{ Instance->GetPawnChecked<APawn>() };
	if (Pawn->HasAuthority())
	{
		auto* ASC{ EMC->GetAbilitySystemComponent() };
		check(ASC);

		for (const auto& AbilitySet : AbilitySetsToGrantOnEquip)
		{
			Instance->GrantAbilitySet_Equip(AbilitySet, ASC);
		}
	}
}

void UEquipmentFragment_AddAbilitySets::OnUnequiped(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const
{
	check(EMC);
	check(Instance);

	auto* Pawn{ Instance->GetPawnChecked<APawn>() };
	if (Pawn->HasAuthority())
	{
		if (auto* ASC{ EMC->GetAbilitySystemComponent() })
		{
			Instance->RemoveAbilities_Equip(ASC);
		}
	}
}

void UEquipmentFragment_AddAbilitySets::OnActivated(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const
{
	check(EMC);
	check(Instance);

	auto* Pawn{ Instance->GetPawnChecked<APawn>() };
	if (Pawn->HasAuthority())
	{
		auto* ASC{ EMC->GetAbilitySystemComponent() };
		check(ASC);

		for (const auto& AbilitySet : AbilitySetsToGrantOnActive)
		{
			Instance->GrantAbilitySet_Active(AbilitySet, ASC);
		}
	}
}

void UEquipmentFragment_AddAbilitySets::OnDeactivated(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const
{
	check(EMC);
	check(Instance);

	auto* Pawn{ Instance->GetPawnChecked<APawn>() };
	if (Pawn->HasAuthority())
	{
		if (auto* ASC{ EMC->GetAbilitySystemComponent() })
		{
			Instance->RemoveAbilities_Active(ASC);
		}
	}
}
