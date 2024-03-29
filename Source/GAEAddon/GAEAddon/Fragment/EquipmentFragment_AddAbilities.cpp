﻿// Copyright (C) 2024 owoDra

#include "EquipmentFragment_AddAbilities.h"

#include "EquipmentManagerComponent.h"
#include "EquipmentInstance.h"

#include "AbilitySet.h"

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

	auto* Pawn{ Instance->GetPawnChecked<APawn>() };
	if (Pawn->HasAuthority())
	{
		auto* ASC{ EMC->GetAbilitySystemComponent() };
		check(ASC);

		Instance->GrantAbilitySet_Equip(GrantedGameplayAbilities_OnEquiped, GrantedGameplayEffects_OnEquiped, GrantedAttributes_OnEquiped, ASC);
	}
}

void UEquipmentFragment_AddAbilities::OnUnequiped(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const
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

void UEquipmentFragment_AddAbilities::OnActivated(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const
{
	check(EMC);
	check(Instance);

	auto* Pawn{ Instance->GetPawnChecked<APawn>() };
	if (Pawn->HasAuthority())
	{
		auto* ASC{ EMC->GetAbilitySystemComponent() };
		check(ASC);

		Instance->GrantAbilitySet_Equip(GrantedGameplayAbilities_OnActivated, GrantedGameplayEffects_OnActivated, GrantedAttributes_OnActivated, ASC);
	}
}

void UEquipmentFragment_AddAbilities::OnDeactivated(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const
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
