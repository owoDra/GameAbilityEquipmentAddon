// Copyright (C) 2023 owoDra

#pragma once

#include "Net/Serialization/FastArraySerializer.h"

#include "GameplayTagContainer.h"

#include "EquipmentContainer.generated.h"

class UEquipmentData;
class UEquipmentInstance;
class UEquipmentManagerComponent;


/**
 * Information on Equipment registered with EquipmentManagerComponent
 */
USTRUCT(BlueprintType)
struct FEquipmentEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	friend struct FEquipmentContainer;

public:
	FEquipmentEntry()
		: Activated(false)
	{
	}

public:
	UPROPERTY()
	TObjectPtr<const UEquipmentData> Data{ nullptr };

	UPROPERTY()
	TObjectPtr<UEquipmentInstance> Instance{ nullptr };

	UPROPERTY()
	FGameplayTag SlotTag{ FGameplayTag::EmptyTag };

	UPROPERTY()
	uint8 Activated : 1;

public:
	FString GetDebugString() const;

	bool IsValid() const;

};


/**
 * List of Equipment registered with EquipmentManagerComponent
 */
USTRUCT(BlueprintType)
struct FEquipmentContainer : public FFastArraySerializer
{
	GENERATED_BODY()

	friend class UEquipmentManagerComponent;

public:
	FEquipmentContainer()
		: OwnerComponent(nullptr)
	{
	}

	FEquipmentContainer(UEquipmentManagerComponent* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{
	}

protected:
	UPROPERTY()
	TArray<FEquipmentEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UEquipmentManagerComponent> OwnerComponent;

public:
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FEquipmentEntry, FEquipmentContainer>(Entries, DeltaParms, *this);
	}

protected:
	UEquipmentInstance* AddEntry(const UEquipmentData* EquipmentData, FGameplayTag SlotTag);
	UEquipmentInstance* RemoveEntry(FGameplayTag SlotTag);
	TArray<UEquipmentInstance*> RemoveAllEntries();

	void ActivateEntry(int32 SlotIndex);
	void DeactivateEntry(int32 SlotIndex);


protected:
	void BroadcastSlotChangeMessage(
		FGameplayTag SlotTag = FGameplayTag::EmptyTag,
		const UEquipmentData* EquipmentData = nullptr,
		UEquipmentInstance* Instance = nullptr);

	void BroadcastActiveSlotChangeMessage(
		FGameplayTag SlotTag = FGameplayTag::EmptyTag,
		const UEquipmentData* EquipmentData = nullptr,
		UEquipmentInstance* Instance = nullptr);

};

template<>
struct TStructOpsTypeTraits<FEquipmentContainer> : public TStructOpsTypeTraitsBase2<FEquipmentContainer>
{
	enum { WithNetDeltaSerializer = true };
};
