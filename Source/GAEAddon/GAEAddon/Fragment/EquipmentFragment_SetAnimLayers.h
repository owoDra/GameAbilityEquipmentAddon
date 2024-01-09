// Copyright (C) 2024 owoDra

#pragma once

#include "Fragment/EquipmentFragmentBase.h"

#include "EquipmentFragment_SetAnimLayers.generated.h"


/**
 * Class that defines the abilities granted to a character when equiping or activating equipment
 */
UCLASS(meta = (DisplayName = "EF Set Anim Layers"))
class UEquipmentFragment_SetAnimLayers : public UEquipmentFragmentBase
{
	GENERATED_BODY()
public:
	UEquipmentFragment_SetAnimLayers(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	//
	// AnimLayer class that adapts to the Pawn's Mesh when Equipment is Active.
	//
	UPROPERTY(EditDefaultsOnly, Category = "SetAnimLayers")
	TSubclassOf<UAnimInstance> AnimLayerToApply{ nullptr };

public:
	virtual void OnActivated(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const override;
	virtual void OnDeactivated(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const override;

};
