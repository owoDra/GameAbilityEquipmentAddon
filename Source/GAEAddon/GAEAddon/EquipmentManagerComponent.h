// Copyright (C) 2023 owoDra

#pragma once

#include "Components/PawnComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"

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
class GAEADDON_API UEquipmentManagerComponent 
	: public UPawnComponent
	, public IGameFrameworkInitStateInterface
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
	virtual void OnRegister() override;
	virtual void OnUnregister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	void InitializeWithAbilitySystem();
	void UninitializeFromAbilitySystem();

public:
	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual void CheckDefaultInitialization() override;


private:
	UPROPERTY(Replicated)
	FEquipmentContainer EquipmentContainer;

public:
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void ReadyForReplication() override;


public:
	/**
	 * Adds Equipment to the specified Slot.
	 * 
	 * Tips:
	 *	If you want to add more than one Equipments, use ResetEquipments.
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Equipment", meta = (GameplayTagFilter = "Equipment.Slot"))
	bool AddEquipment(FGameplayTag SlotTag, const UEquipmentData* EquipmentData, bool ActivateImmediately = true);

	/**
	 * Add multiple Equipments to the specified Slot at once.
	 * 
	 * Tips:
	 *	It can be made Active by specifying a SlotTag in the ActivateSlotTag.
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Equipment", meta = (GameplayTagFilter = "Equipment.Slot"))
	void ResetEquipments(const TArray<FEquipmentSetEntry>& Entries, FGameplayTag ActivateSlotTag = FGameplayTag());

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Equipment", meta = (GameplayTagFilter = "Equipment.Slot"))
	void ResetEquipmentsByEquipmentSet(const UEquipmentSet* InSet);

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


public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Component")
	static UEquipmentManagerComponent* FindEquipmentManagerComponent(const APawn* Pawn);

	UAbilitySystemComponent* GetAbilitySystemComponent() const { return AbilitySystemComponent; }

};
