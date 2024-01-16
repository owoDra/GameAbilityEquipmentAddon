// Copyright (C) 2024 owoDra

#include "EquipmentFragment_SetAnimLayersForMesh.h"

#include "GAEAddonLogs.h"

#include "EquipmentManagerComponent.h"
#include "EquipmentInstance.h"

#include "Character/CharacterMeshAccessorInterface.h"

#include "GameFramework/Pawn.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EquipmentFragment_SetAnimLayersForMesh)


UEquipmentFragment_SetAnimLayersForMesh::UEquipmentFragment_SetAnimLayersForMesh(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


void UEquipmentFragment_SetAnimLayersForMesh::OnActivated(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const
{
	Super::OnActivated(EMC, Instance);

	check(Instance);

	auto* Pawn{ Instance->GetPawn<APawn>() };

	for (const auto& KVP : AnimLayerToApply)
	{
		const auto& Tag{ KVP.Key };
		const auto& Class{ KVP.Value };

		if (!Class)
		{
			continue;
		}

		UE_LOG(LogGAEA, Log, TEXT("+ To [%s](Class:%s)")
			, *Tag.GetTagName().ToString()
			, *GetNameSafe(Class));

		if (auto* Mesh{ ICharacterMeshAccessorInterface::Execute_GetMeshByTag(Pawn, Tag) })
		{
			Instance->ApplyAnimLayer(Mesh, Class);
		}
	}
}

void UEquipmentFragment_SetAnimLayersForMesh::OnDeactivated(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const
{
	Super::OnDeactivated(EMC, Instance);

	check(Instance);

	Instance->RemoveAnimLayers();
}
