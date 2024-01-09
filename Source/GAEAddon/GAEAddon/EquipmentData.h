// Copyright (C) 2024 owoDra

#pragma once

#include "Engine/DataAsset.h"

#include "GameplayTagContainer.h"

#include "EquipmentData.generated.h"

class UEquipmentFragmentBase;
class UEquipmentInstance;


/**
 * Class that defines predefined basic information such as the name and details of Equipment
 */
UCLASS(BlueprintType, Const)
class GAEADDON_API UEquipmentData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UEquipmentData() {}
	
#if WITH_EDITOR 
	virtual EDataValidationResult IsDataValid(TArray<FText>& ValidationErrors) override;
#endif // WITH_EDITOR
	
#if WITH_EDITORONLY_DATA
	virtual void UpdateAssetBundleData() override;
#endif // WITH_EDITORONLY_DATA

public:
	//
	// Name displayed in game
	//
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Info")
	FText DisplayName;

	//
	// Description of the weapon to be displayed in the game
	//
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Info")
	FText Description;

	//
	// Image of the weapon to be displayed in the game
	//
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Info")
	TObjectPtr<UTexture2D> Image{ nullptr };

	//
	// Specify the Slot to which equipment can be added
	// 
	// Tips:
	//	Can be added to all slots if nothing is set 
	//
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Assignment", meta = (Categories = "Equipment.Slot"))
	FGameplayTagContainer AllowedSlotTags;

	//
	// Class of instances to manage the performance and status of equipment
	// 
	// Tips:
	//	If not set, it is created based on the default UEquipmentInstance.
	//
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Assignment")
	TSubclassOf<UEquipmentInstance> InstanceType{ nullptr };

	//
	// List of additional information about Equipment
	//
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, Category = "Fragments")
	TArray<TObjectPtr<UEquipmentFragmentBase>> Fragments;

public:
	/**
	 * Executed when Equipment is Equiped
	 */
	void HandleEquiped(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const;

	/**
	 * Executed when Equipment is Unequiped
	 */
	void HandleUnequiped(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const;

	/**
	 * Executed when Equipment is Activated
	 */
	void HandleActivated(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const;

	/**
	 * Executed when Equipment is Deactivated
	 */
	void HandleDeactivated(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const;


public:
	/**
	 * Get the EquipmentFragment in EquipmentData by class. If not, nullptr is returned.
	 */
	const UEquipmentFragmentBase* FindFragmentByClass(TSubclassOf<UEquipmentFragmentBase> FragmentClass) const;

	template <typename T>
	const T* FindFragment() const
	{
		return Cast<T>(FindFragmentByClass(T::StaticClass()));
	}

};
