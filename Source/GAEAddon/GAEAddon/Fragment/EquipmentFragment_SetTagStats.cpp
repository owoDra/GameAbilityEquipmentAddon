// Copyright (C) 2023 owoDra

#include "EquipmentFragment_SetTagStats.h"

#include "EquipmentManagerComponent.h"
#include "EquipmentInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EquipmentFragment_SetTagStats)


UEquipmentFragment_SetTagStats::UEquipmentFragment_SetTagStats(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


void UEquipmentFragment_SetTagStats::OnEquiped(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const
{
	check(Instance);

	auto* Pawn{ Instance->GetPawnChecked<APawn>() };
	if (Pawn->HasAuthority())
	{
		for (const auto& KVP : InitialEquipmentStats)
		{
			const auto& Tag{ KVP.Key };
			const auto& Count{ KVP.Value };

			Instance->AddStatTagStack(Tag, Count);
		}
	}
}
