// Copyright (C) 2024 owoDra

#pragma once

#include "EquipmentFragmentBase.generated.h"

class UEquipmentInstance;
class UEquipmentManagerComponent;


/**
 * Base class for additional information that can be assigned to Equipment
 */
UCLASS(Abstract, Const, DefaultToInstanced, EditInlineNew)
class GAEADDON_API UEquipmentFragmentBase : public UObject
{
	GENERATED_BODY()
public:
	UEquipmentFragmentBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

#if WITH_EDITOR 
	virtual UClass* GetRequiredInstanceClass() const;
#endif // WITH_EDITOR

#if WITH_EDITORONLY_DATA
	virtual void AddAdditionalAssetBundleData(FAssetBundleData& AssetBundleData) {}
#endif

public:
	/**
	 * Executed when Equipment is Equiped
	 */
	virtual void OnEquiped(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const {}

	/**
	 * Executed when Equipment is Unequiped
	 */
	virtual void OnUnequiped(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const {}

	/**
	 * Executed when Equipment is Activated
	 */
	virtual void OnActivated(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const {}

	/**
	 * Executed when Equipment is Deactivated
	 */
	virtual void OnDeactivated(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const {}

};
