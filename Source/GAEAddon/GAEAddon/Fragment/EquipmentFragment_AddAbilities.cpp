// Copyright (C) 2023 owoDra

#include "EquipmentFragment_AddAbilities.h"

#include "EquipmentManagerComponent.h"
#include "EquipmentInstance.h"

#include "AbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EquipmentFragment_AddAbilities)


UEquipmentFragment_AddAbilities::UEquipmentFragment_AddAbilities(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


void UEquipmentFragment_AddAbilities::OnEquiped(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const
{
	check(EMC);
	check(Instance);

	auto* ASC{ EMC->GetAbilitySystemComponent() };
	check(ASC);

	for (const auto& AbilitySet : AbilitySetsToGrantOnEquip)
	{
		Instance->GrantAbilitySet_Equip(AbilitySet, ASC);
	}
}

void UEquipmentFragment_AddAbilities::OnUnequiped(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const
{
	check(EMC);
	check(Instance);

	if (auto* ASC{ EMC->GetAbilitySystemComponent() })
	{
		Instance->RemoveAbilities_Equip(ASC);
	}
}

void UEquipmentFragment_AddAbilities::OnActivated(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const
{
	check(EMC);
	check(Instance);

	auto* ASC{ EMC->GetAbilitySystemComponent() };
	check(ASC);

	for (const auto& AbilitySet : AbilitySetsToGrantOnActive)
	{
		Instance->GrantAbilitySet_Active(AbilitySet, ASC);
	}
}

void UEquipmentFragment_AddAbilities::OnDeactivated(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const
{
	check(EMC);
	check(Instance);

	if (auto* ASC{ EMC->GetAbilitySystemComponent() })
	{
		Instance->RemoveAbilities_Active(ASC);
	}
}
