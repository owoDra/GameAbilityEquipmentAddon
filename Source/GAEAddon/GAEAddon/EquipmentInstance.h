﻿// Copyright (C) 2024 owoDra

#pragma once

#include "GameplayTag/GameplayTagStack.h"
#include "GameplayTag/GameplayTagStackInterface.h"

#include "AbilitySet.h"

#include "EquipmentInstance.generated.h"

class UEquipmentData;
class UEquipmentManagerComponent;
class USkeletalMesh;
class UAnimInstance;


/**
 * Mesh data with Animlayer applied
 */
USTRUCT(BlueprintType)
struct FApplyingAnimLayerHandle
{
	GENERATED_BODY()
public:
	FApplyingAnimLayerHandle() {}

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TWeakObjectPtr<USkeletalMeshComponent> MeshComponent{ nullptr };

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TWeakObjectPtr<UAnimInstance> AnimInstance{ nullptr };

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<UAnimInstance> AnimLayerClass{ nullptr };

};


/**
 * Actor spawn settings when equipping
 */
USTRUCT(BlueprintType)
struct GAEADDON_API FEquipmentMeshToSpawn
{
	GENERATED_BODY()
public:
	FEquipmentMeshToSpawn() {}

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<USkeletalMesh> MeshToSpawn;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<UAnimInstance> MeshAnimInstance;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName AttachSocket;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FTransform AttachTransform;

};


/**
 * A piece of equipment spawned and applied to a pawn
 */
UCLASS(BlueprintType)
class GAEADDON_API UEquipmentInstance 
	: public UObject
	, public IGameplayTagStackInterface
{
	GENERATED_BODY()
public:
	UEquipmentInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
#if UE_WITH_IRIS
	virtual void RegisterReplicationFragments(
		UE::Net::FFragmentRegistrationContext& Context,
		UE::Net::EFragmentRegistrationFlags RegistrationFlags) override;
#endif // UE_WITH_IRIS

public:
	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual UWorld* GetWorld() const override final;


public:
	/**
	 * Called when this Equipment is created by EquipmentManagerComponent.
	 * Associate and initialize this Equipment with EquipmentData.
	 */
	virtual void OnEquiped(UEquipmentManagerComponent* EMC, const UEquipmentData* InEquipmentData);

	/**
	 * Called when this Equipment is removed by EquipmentManagerComponent.
	 * Remove this SpawnedActor.
	 */
	virtual void OnUnequiped(UEquipmentManagerComponent* EMC, const UEquipmentData* InEquipmentData);

	/**
	 * Called when this Equipment is activated from EquipmentManagerComponent.
	 */
	virtual void OnActivated(UEquipmentManagerComponent* EMC, const UEquipmentData* InEquipmentData);

	/**
	 * Called when this Equipment is Deactive from EquipmentManagerComponent
	 */
	virtual void OnDeactivated(UEquipmentManagerComponent* EMC, const UEquipmentData* InEquipmentData);


protected:
	UPROPERTY(Replicated)
	FGameplayTagStackContainer StatTags;

protected:
	virtual FGameplayTagStackContainer* GetStatTags() override { return &StatTags; }
	virtual const FGameplayTagStackContainer* GetStatTagsConst() const override { return &StatTags; }


protected:
	TArray<TObjectPtr<USkeletalMeshComponent>> SpawnedMeshes;

public:
	/**
	 * Called when Equipment becomes Active.
	 * Spawns a SkeletalMeshComponent that will be the appearance of the Equipment.
	 * If ActorToSpawn is not set, nothing is spawned.
	 */
	virtual void SpawnEquipmentMeshes(USkeletalMeshComponent* TargetMesh, const TArray<FEquipmentMeshToSpawn>& InMeshesToSpawn);

	/**
	 * Called when Equipment becomes Deactive.
	 * Destroys the SkeletalMeshComponent that gives the Equipment its appearance.
	 */
	virtual void DestroyEquipmentMeshes();


protected:
	//
	// GrantedHandles of AbilitySet granted when registered to EquipmentManagerComponent
	// 
	// Note:
	//	Only server privileges retain data
	//
	FAbilitySet_GrantedHandles GrantedHandles_Equip;

	//
	// GrantedHandles of AbilitySet that is granted when it is made Active in EquipmentManagerComponent.
	// 
	// Note:
	//	Only server privileges retain data
	//
	FAbilitySet_GrantedHandles GrantedHandles_Active;

public:
	/**
	 * Apply AbilitySet granted at Equip time
	 */
	virtual void GrantAbilitySet_Equip(const UAbilitySet* AbillitySet, UAbilitySystemComponent* ASC);
	virtual void GrantAbilitySet_Equip(
		const TArray<FAbilitySet_GameplayAbility>& Abilities
		, const TArray<FAbilitySet_GameplayEffect>& Effects
		, const TArray<FAbilitySet_AttributeSet>& Sets
		, UAbilitySystemComponent* ASC);

	/**
	 * Apply AbilitySet granted at Activate time
	 */
	virtual void GrantAbilitySet_Active(const UAbilitySet* AbillitySet, UAbilitySystemComponent* ASC);
	virtual void GrantAbilitySet_Active(
		const TArray<FAbilitySet_GameplayAbility>& Abilities
		, const TArray<FAbilitySet_GameplayEffect>& Effects
		, const TArray<FAbilitySet_AttributeSet>& Sets
		, UAbilitySystemComponent* ASC);

	/**
	 * Apply AbilitySet
	 */
	virtual void GrantAbilitySet(const UAbilitySet* AbillitySet, UAbilitySystemComponent* ASC, FAbilitySet_GrantedHandles& OutHandles);

	/**
	 * Remove AbilitySet granted at Equip time
	 */
	virtual void RemoveAbilities_Equip(UAbilitySystemComponent* ASC);

	/**
	 * Remove AbilitySet granted at Activate time
	 */
	virtual void RemoveAbilities_Active(UAbilitySystemComponent* ASC);


protected:
	//
	// Mesh datas with AnimLayer applied
	//
	UPROPERTY(Transient)
	TArray<FApplyingAnimLayerHandle> ApplyingAnimLayers;

public:
	/**
	 * Called when Equipment becomes Active.
	 * Adapts AnimLayer to the Pawn.
	 */
	virtual void ApplyAnimLayer(USkeletalMeshComponent* TargetMesh, TSubclassOf<UAnimInstance> InLayer);

	/**
	 * Called when Equipment becomes Deactive.
	 * Deletes the adapted AnimLayer from the Pawn.
	 */
	virtual void RemoveAnimLayers();

protected:
	UFUNCTION()
	void HandleAnimLayerApplingMeshAnimInitialized();


public:
	template<typename T = APawn>
	T* GetPawn() const
	{
		return Cast<T>(GetOuter());
	}

	template<typename T = APawn>
	T* GetPawnChecked() const
	{
		return CastChecked<T>(GetOuter());
	}

};
