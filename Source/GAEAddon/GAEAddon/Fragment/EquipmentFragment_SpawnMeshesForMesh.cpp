// Copyright (C) 2024 owoDra

#include "EquipmentFragment_SpawnMeshesForMesh.h"

#include "GAEAddonLogs.h"

#include "EquipmentManagerComponent.h"
#include "EquipmentInstance.h"

#include "Character/CharacterMeshAccessorInterface.h"

#include "GameFramework/Pawn.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EquipmentFragment_SpawnMeshesForMesh)


UEquipmentFragment_SpawnMeshesForMesh::UEquipmentFragment_SpawnMeshesForMesh(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


void UEquipmentFragment_SpawnMeshesForMesh::OnActivated(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const
{
	Super::OnActivated(EMC, Instance);

	check(Instance);

	auto* Pawn{ Instance->GetPawn<APawn>() };
	const auto bLocallyControlled{ Pawn->IsLocallyControlled() };

	UE_LOG(LogGAEA, Log, TEXT("+ Add (%d) Meshes to (%d) components"), MeshesToSpawn.Num(), ComponentToAdd.Num());

	for (const auto& Entry : ComponentToAdd)
	{
		const auto bCanAdd
		{
			(bLocallyControlled && Entry.bAddToOwner) || (!bLocallyControlled && Entry.bAddToOther)
		};

		UE_LOG(LogGAEA, Log, TEXT("++ To [%s](bAddToOwner:%s, bAddToOther:%s, bCanAdd:%s)")
			, *Entry.MeshTypeTag.GetTagName().ToString()
			, Entry.bAddToOwner ? TEXT("TRUE") : TEXT("FALSE")
			, Entry.bAddToOther ? TEXT("TRUE") : TEXT("FALSE")
			, bCanAdd ? TEXT("TRUE") : TEXT("FALSE"));

		if (bCanAdd)
		{
			if (auto* Mesh{ ICharacterMeshAccessorInterface::Execute_GetMeshByTag(Pawn, Entry.MeshTypeTag) })
			{
				Instance->SpawnEquipmentMeshes(Mesh, MeshesToSpawn);
			}
		}
	}
}

void UEquipmentFragment_SpawnMeshesForMesh::OnDeactivated(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const
{
	Super::OnDeactivated(EMC, Instance);

	check(Instance);

	Instance->DestroyEquipmentMeshes();
}
