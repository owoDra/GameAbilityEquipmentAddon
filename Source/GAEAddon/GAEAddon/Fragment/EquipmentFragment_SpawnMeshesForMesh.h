// Copyright (C) 2024 owoDra

#pragma once

#include "Fragment/EquipmentFragmentBase.h"

#include "GameplayTagContainer.h"

#include "EquipmentFragment_SpawnMeshesForMesh.generated.h"

struct FEquipmentMeshToSpawn;


/**
 * Definition data for the component to which the mesh is added
 */
USTRUCT(BlueprintType)
struct FMeshComponentToAddEquipment
{
	GENERATED_BODY()
public:
	FMeshComponentToAddEquipment() {}

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Categories = "MeshType"))
	FGameplayTag MeshTypeTag;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bAddToOwner{ true };

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bAddToOther{ true };

};


/**
 * Class that defines the abilities granted to a character when equiping or activating equipment
 */
UCLASS(meta = (DisplayName = "EF Spawn Meshes For Mesh"))
class UEquipmentFragment_SpawnMeshesForMesh : public UEquipmentFragmentBase
{
	GENERATED_BODY()
public:
	UEquipmentFragment_SpawnMeshesForMesh(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	//
	// Definition of SkeletalMesh, which is the appearance of the equipment
	//
	UPROPERTY(EditDefaultsOnly, Category = "SpawnMeshes")
	TArray<FEquipmentMeshToSpawn> MeshesToSpawn;

	//
	// List of tags for the component to which the mesh is to be added
	//
	UPROPERTY(EditDefaultsOnly, Category = "SpawnMeshes")
	TArray<FMeshComponentToAddEquipment> ComponentToAdd;

public:
	virtual void OnActivated(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const override;
	virtual void OnDeactivated(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const override;

};
