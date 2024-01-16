// Copyright (C) 2024 owoDra

#pragma once

#include "Fragment/EquipmentFragmentBase.h"

#include "GameplayTagContainer.h"

#include "EquipmentFragment_SetAnimLayersForMesh.generated.h"

class UAnimInstance;


/**
 * Class that defines the abilities granted to a character when equiping or activating equipment
 */
UCLASS(meta = (DisplayName = "EF Set Anim Layers For Mesh"))
class UEquipmentFragment_SetAnimLayersForMesh : public UEquipmentFragmentBase
{
	GENERATED_BODY()
public:
	UEquipmentFragment_SetAnimLayersForMesh(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	//
	// AnimLayer class that adapts to the Pawn's Mesh when Equipment is Active.
	//
	UPROPERTY(EditDefaultsOnly, Category = "SetAnimLayers", meta = (ForceInlineRow))
	TMap<FGameplayTag, TSubclassOf<UAnimInstance>> AnimLayerToApply;

public:
	virtual void OnActivated(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const override;
	virtual void OnDeactivated(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const override;

};
