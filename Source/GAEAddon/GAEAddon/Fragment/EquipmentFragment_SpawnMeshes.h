// Copyright (C) 2024 owoDra

#pragma once

#include "Fragment/EquipmentFragmentBase.h"

#include "EquipmentFragment_SpawnMeshes.generated.h"

struct FEquipmentMeshToSpawn;


/**
 * Class that defines the abilities granted to a character when equiping or activating equipment
 */
UCLASS(meta = (DisplayName = "EF Spawn Meshes"))
class UEquipmentFragment_SpawnMeshes : public UEquipmentFragmentBase
{
	GENERATED_BODY()
public:
	UEquipmentFragment_SpawnMeshes(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	//
	// Definition of SkeletalMesh, which is the appearance of the equipment
	//
	UPROPERTY(EditDefaultsOnly, Category = "SpawnMeshes")
	TArray<FEquipmentMeshToSpawn> MeshesToSpawn;

public:
	virtual void OnActivated(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const override;
	virtual void OnDeactivated(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const override;

};
