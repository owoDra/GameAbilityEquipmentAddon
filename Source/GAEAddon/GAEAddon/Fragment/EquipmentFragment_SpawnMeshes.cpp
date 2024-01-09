// Copyright (C) 2024 owoDra

#include "EquipmentFragment_SpawnMeshes.h"

#include "EquipmentManagerComponent.h"
#include "EquipmentInstance.h"

#include "GameFramework/Character.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EquipmentFragment_SpawnMeshes)


UEquipmentFragment_SpawnMeshes::UEquipmentFragment_SpawnMeshes(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


void UEquipmentFragment_SpawnMeshes::OnActivated(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const
{
	check(Instance);

	auto* Character{ Instance->GetPawn<ACharacter>() };
	auto* Mesh{ Character ? Character->GetMesh() : nullptr };

	if (Mesh)
	{
		Instance->SpawnEquipmentMeshes(Mesh, MeshesToSpawn);
	}
}

void UEquipmentFragment_SpawnMeshes::OnDeactivated(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const
{
	check(Instance);

	Instance->DestroyEquipmentMeshes();
}
