// Copyright (C) 2023 owoDra

#include "EquipmentFragment_SetAnimLayers.h"

#include "EquipmentManagerComponent.h"
#include "EquipmentInstance.h"

#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EquipmentFragment_SetAnimLayers)


UEquipmentFragment_SetAnimLayers::UEquipmentFragment_SetAnimLayers(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UEquipmentFragment_SetAnimLayers::OnActivated(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const
{
	check(Instance);

	auto* Character{ Instance->GetPawn<ACharacter>() };
	auto* Mesh{ Character ? Character->GetMesh() : nullptr };

	if (Mesh && AnimLayerToApply)
	{
		Mesh->LinkAnimClassLayers(AnimLayerToApply);
	}
}

void UEquipmentFragment_SetAnimLayers::OnDeactivated(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const
{
	check(Instance);

	auto* Character{ Instance->GetPawn<ACharacter>() };
	auto* Mesh{ Character ? Character->GetMesh() : nullptr };

	if (Mesh && AnimLayerToApply)
	{
		Mesh->UnlinkAnimClassLayers(AnimLayerToApply);
	}
}
