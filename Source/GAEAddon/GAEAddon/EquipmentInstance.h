// Copyright (C) 2023 owoDra

#pragma once

#include "GameplayTag/GameplayTagStack.h"

#include "AbilitySet.h"

#include "EquipmentInstance.generated.h"

class UEquipmentData;
class UEquipmentManagerComponent;
class USkeletalMesh;
class UAnimInstance;


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
class GAEADDON_API UEquipmentInstance : public UObject
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

public:
	/**
	 * Add a Tag to Equipment that can be handled as statistics 
	 * 
	 * Tips:
	 *	if StackCount is less than or equal to 0, do nothing.
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Equipment")
	void AddStatTagStack(FGameplayTag Tag, int32 StackCount);

	/**
	 * Delete Tags that can be handled as statistics in Equipment
	 * 
	 * Tips:
	 *	If StackCount is less than or equal to 0, do nothing.
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Equipment")
	void RemoveStatTagStack(FGameplayTag Tag, int32 StackCount);

	/**
	 * Returns the number of Tags that can be handled as statistics in Equipment.
	 *
	 * Tips:
	 *	Returns 0 if not present.
	 */
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	int32 GetStatTagStackCount(FGameplayTag Tag) const;

	/**
	 * Returns whether or not a Tag that can be handled as statistics exists in Equipment.
	 */
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	bool HasStatTag(FGameplayTag Tag) const;


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

	/**
	 * Apply AbilitySet granted at Activate time
	 */
	virtual void GrantAbilitySet_Active(const UAbilitySet* AbillitySet, UAbilitySystemComponent* ASC);

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


public:
	/**
	 * Called when Equipment becomes Active.
	 * Adapts AnimLayer to the Pawn.
	 */
	virtual void ApplyAnimLayer(UAnimInstance* TargetAnimInstance, TSubclassOf<UAnimInstance> InLayer);

	/**
	 * Called when Equipment becomes Deactive.
	 * Deletes the adapted AnimLayer from the Pawn.
	 */
	virtual void RemoveAnimLayer(UAnimInstance* TargetAnimInstance, TSubclassOf<UAnimInstance> InLayer);


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
