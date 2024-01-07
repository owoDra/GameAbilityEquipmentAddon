// Copyright (C) 2023 owoDra

#pragma once

#include "Component/GFCPawnComponent.h"

#include "EquipmentSet.h"
#include "EquipmentContainer.h"
#include "EquipmentSlotChangeMessage.h"

#include "EquipmentManagerComponent.generated.h"

class EquipmentSet;
class UEquipmentData;
class UEquipmentInstance;
class APawn;
class UAbilitySystemComponent;


/**
 * Components for managing Equipment
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class GAEADDON_API UEquipmentManagerComponent : public UGFCPawnComponent
{
	GENERATED_BODY()
public:
	UEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//
	// Function name used to add this component
	//
	static const FName NAME_ActorFeatureName;

protected:
	UPROPERTY(Transient)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent{ nullptr };

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void InitializeWithAbilitySystem();
	void UninitializeFromAbilitySystem();

public:
	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;

protected:
	virtual bool CanChangeInitStateToDataInitialized(UGameFrameworkComponentManager* Manager) const override;
	virtual void HandleChangeInitStateToDataInitialized(UGameFrameworkComponentManager* Manager) override;

	////////////////////////////////////////////////////////////////////////////////////
	// Equipment Container
#pragma region Equipment Container
private:
	UPROPERTY(Replicated)
	FEquipmentContainer EquipmentContainer;

public:
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void ReadyForReplication() override;

#pragma endregion


	////////////////////////////////////////////////////////////////////////////////////
	// Initial Equipments
#pragma region Initial Equipments
protected:
	//
	// Initial equipment set
	// 
	// Tips:
	//	If you do not have any initial equipment, prepare an empty EquipmentSet and set it during initialization.
	//
	UPROPERTY(EditAnywhere, ReplicatedUsing = "OnRep_InitialEquipmentSet")
	TObjectPtr<const UEquipmentSet> InitialEquipmentSet{ nullptr };

protected:
	UFUNCTION()
	virtual void OnRep_InitialEquipmentSet();

	/**
	 * Apply the initial equipment set
	 */
	virtual void ApplyInitialEquipmentSet();

public:
	/**
	 * Set the initial equipment set
	 */
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable)
	void SetInitialEquipmentSet(const UEquipmentSet* NewEquipmentSet);

#pragma endregion


	////////////////////////////////////////////////////////////////////////////////////
	// Equipment Change
#pragma region Equipment Change
public:
	/**
	 * Remove all equipment and then add new equipment again.
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Equipment", meta = (GameplayTagFilter = "Equipment.Slot"))
	void ResetEquipments(const TArray<FEquipmentSetEntry>& Entries, FGameplayTag ActivateSlotTag);

	/**
	 * Use EquipmentSet to remove all equipment and then add new equipment again
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Equipment")
	void ResetEquipmentsByEquipmentSet(const UEquipmentSet* InSet);

	/**
	 * Adds Equipment to the specified Slot.
	 * 
	 * Tips:
	 *	If you want to add more than one Equipments, use ResetEquipments.
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Equipment", meta = (GameplayTagFilter = "Equipment.Slot"))
	bool AddEquipment(FGameplayTag SlotTag, const UEquipmentData* EquipmentData, bool ActivateImmediately = true);

	/**
	 * Remove the Equipment in the specified Slot.
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Equipment", meta = (GameplayTagFilter = "Equipment.Slot"))
	bool RemoveEquipment(FGameplayTag SlotTag);

	/**
	 * Remove all the Equipment that has been added.
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Equipment")
	void RemoveAllEquipments();

#pragma endregion


	////////////////////////////////////////////////////////////////////////////////////
	// Active Slot
#pragma region Active Slot
public:
	/**
	 * Changing an Active Slot
	 * If the Slot to be set is already Active, nothing happens.
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Equipment", meta = (GameplayTagFilter = "Equipment.Slot"))
	void SetActiveSlot(FGameplayTag SlotTag);

	/**
	 * Gets Equipment Data, Instance, and SlotTag of the active slot.
	 */
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	bool GetActiveSlotInfo(FEquipmentSlotChangedMessage& SlotInfo);

	/**
	 * Gets Equipment Data, Instance, and SlotTag for the specified Slot.
	 */
	UFUNCTION(BlueprintCallable, Category = "Equipment", meta = (GameplayTagFilter = "Equipment.Slot"))
	bool GetSlotInfo(FGameplayTag SlotTag, FEquipmentSlotChangedMessage& SlotInfo);

#pragma endregion


	////////////////////////////////////////////////////////////////////////////////////
	// Utilities
#pragma region Utilities
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Component")
	static UEquipmentManagerComponent* FindEquipmentManagerComponent(const APawn* Pawn);

	UAbilitySystemComponent* GetAbilitySystemComponent() const { return AbilitySystemComponent; }

#pragma endregion

};
