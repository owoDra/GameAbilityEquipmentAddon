// Copyright (C) 2023 owoDra

#include "EquipmentManagerComponent.h"

#include "EquipmentSet.h"
#include "EquipmentData.h"
#include "EquipmentInstance.h"
#include "GAEAddonLogs.h"

#include "InitState/InitStateTags.h"

#include "GAEAbilitySystemComponent.h"

#include "AbilitySystemGlobals.h"
#include "Engine/ActorChannel.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EquipmentManagerComponent)


const FName UEquipmentManagerComponent::NAME_ActorFeatureName("EquipmentManager");

UEquipmentManagerComponent::UEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, EquipmentContainer(this)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

void UEquipmentManagerComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, EquipmentContainer);
	DOREPLIFETIME(ThisClass, InitialEquipmentSet);
}


void UEquipmentManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UninitializeFromAbilitySystem();

	Super::EndPlay(EndPlayReason);
}


void UEquipmentManagerComponent::InitializeWithAbilitySystem()
{
	auto* Pawn{ GetPawnChecked<APawn>() };
	auto* NewASC{ UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Pawn) };

	if (AbilitySystemComponent)
	{
		if (AbilitySystemComponent == NewASC)
		{
			return;
		}

		UninitializeFromAbilitySystem();
	}

	AbilitySystemComponent = NewASC;

	if (!AbilitySystemComponent)
	{
		UE_LOG(LogGAEA, Error, TEXT("EquipmentManagerComponent: Cannot initialize component for owner [%s] with NULL ability system."), *GetNameSafe(Pawn));
		return;
	}
}

void UEquipmentManagerComponent::UninitializeFromAbilitySystem()
{
	if (AbilitySystemComponent)
	{
		RemoveAllEquipments();
	}

	AbilitySystemComponent = nullptr;
}


void UEquipmentManagerComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	Super::OnActorInitStateChanged(Params);

	// Wait for initialization of AbilitySystemCompoenet

	if (Params.FeatureName == UGAEAbilitySystemComponent::NAME_ActorFeatureName)
	{
		if ((Params.FeatureState == TAG_InitState_DataInitialized))
		{
			CheckDefaultInitialization();
		}
	}
}

bool UEquipmentManagerComponent::CanChangeInitStateToDataInitialized(UGameFrameworkComponentManager* Manager) const
{
	if (!Manager->HasFeatureReachedInitState(GetOwner(), UGAEAbilitySystemComponent::NAME_ActorFeatureName, TAG_InitState_DataInitialized))
	{
		return false;
	}

	if (!InitialEquipmentSet)
	{
		return false;
	}

	return true;
}

void UEquipmentManagerComponent::HandleChangeInitStateToDataInitialized(UGameFrameworkComponentManager* Manager)
{
	InitializeWithAbilitySystem();

	ApplyInitialEquipmentSet();
}


#pragma region Equipment Container

bool UEquipmentManagerComponent::ReplicateSubobjects(UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	auto bWroteSomething{ Super::ReplicateSubobjects(Channel, Bunch, RepFlags) };

	for (auto& Entry : EquipmentContainer.Entries)
	{
		if (auto Instance{ Entry.Instance })
		{
			bWroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}

	return bWroteSomething;
}

void UEquipmentManagerComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	if (IsUsingRegisteredSubObjectList())
	{
		for (const auto& Entry : EquipmentContainer.Entries)
		{
			auto Instance{ Entry.Instance };

			if (IsValid(Instance))
			{
				AddReplicatedSubObject(Instance);
			}
		}
	}
}

#pragma endregion


#pragma region Initial Equipments

void UEquipmentManagerComponent::OnRep_InitialEquipmentSet()
{
	check(InitialEquipmentSet);

	CheckDefaultInitialization();
}

void UEquipmentManagerComponent::ApplyInitialEquipmentSet()
{
	// Must have Authority

	if (!HasAuthority())
	{
		return;
	}

	check(AbilitySystemComponent);
	check(InitialEquipmentSet);

	// Add Equipments to the specified slots

	for (const auto& Entry : InitialEquipmentSet->Entries)
	{
		// Check if the argument is valid

		if (!Entry.EquipmentData || !Entry.SlotTag.IsValid())
		{
			continue;
		}

		// Add Equipment to the specified slot

		if (auto* Result{ EquipmentContainer.AddEntry(Entry.EquipmentData, Entry.SlotTag) })
		{
			if (IsUsingRegisteredSubObjectList() && IsReadyForReplication())
			{
				AddReplicatedSubObject(Result);
			}
		}
	}

	// Set new active slot

	auto ActivateSlotTag{ InitialEquipmentSet->DefaultActiveSlotTag };

	if (ActivateSlotTag.IsValid())
	{
		SetActiveSlot(ActivateSlotTag);
	}
}

void UEquipmentManagerComponent::SetInitialEquipmentSet(const UEquipmentSet* NewEquipmentSet)
{
	if (HasAuthority())
	{
		// If alread set, skip below

		if (InitialEquipmentSet)
		{
			return;
		}

		// Set Initial Equipment Set

		if (InitialEquipmentSet != NewEquipmentSet)
		{
			InitialEquipmentSet = NewEquipmentSet;

			CheckDefaultInitialization();
		}
	}
}

#pragma endregion


#pragma region Equipment Change

void UEquipmentManagerComponent::ResetEquipments(const TArray<FEquipmentSetEntry>& Entries, FGameplayTag ActivateSlotTag)
{
	// Must have Authority

	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	// If has not game ready, skip

	if (!HasReachedInitState(TAG_InitState_GameplayReady))
	{
		return;
	}

	// If the specified slot already has Equipment, remove it.

	auto Instances{ EquipmentContainer.RemoveAllEntries() };
	for (const auto& Instance : Instances)
	{
		if (IsUsingRegisteredSubObjectList())
		{
			RemoveReplicatedSubObject(Instance);
		}
	}

	Instances.Empty();


	// Add Equipments to the specified slots

	for (const auto& Entry : Entries)
	{
		// Check if the argument is valid

		if (!Entry.EquipmentData || !Entry.SlotTag.IsValid())
		{
			continue;
		}

		// Add Equipment to the specified slot

		if (auto* Result{ EquipmentContainer.AddEntry(Entry.EquipmentData, Entry.SlotTag) })
		{
			if (IsUsingRegisteredSubObjectList() && IsReadyForReplication())
			{
				AddReplicatedSubObject(Result);
			}
		}
	}

	// Set new active slot

	if (ActivateSlotTag.IsValid())
	{
		SetActiveSlot(ActivateSlotTag);
	}
}

void UEquipmentManagerComponent::ResetEquipmentsByEquipmentSet(const UEquipmentSet* InSet)
{
	if (InSet)
	{
		ResetEquipments(InSet->Entries, InSet->DefaultActiveSlotTag);
	}
}

bool UEquipmentManagerComponent::AddEquipment(FGameplayTag SlotTag, const UEquipmentData* EquipmentData, bool ActivateImmediately)
{
	// Must have Authority

	if (!GetOwner()->HasAuthority())
	{
		return false;
	}

	// If has not game ready, skip

	if (!HasReachedInitState(TAG_InitState_GameplayReady))
	{
		return false;
	}

	// Check if the argument is valid

	if (!EquipmentData || !SlotTag.IsValid())
	{
		return false;
	}

	// If the specified slot already has Equipment, remove it.

	if (auto* Instance{ EquipmentContainer.RemoveEntry(SlotTag) })
	{
		if (IsUsingRegisteredSubObjectList())
		{
			RemoveReplicatedSubObject(Instance);
		}
	}

	// Add Equipment to the specified slot

	if (auto* Result{ EquipmentContainer.AddEntry(EquipmentData, SlotTag) })
	{
		if (IsUsingRegisteredSubObjectList() && IsReadyForReplication())
		{
			AddReplicatedSubObject(Result);
		}

		if (ActivateImmediately == true)
		{
			SetActiveSlot(SlotTag);
		}

		return true;
	}

	return false;
}

bool UEquipmentManagerComponent::RemoveEquipment(FGameplayTag SlotTag)
{
	// Must have Authority

	if (!GetOwner()->HasAuthority())
	{
		return false;
	}

	// If has not game ready, skip

	if (!HasReachedInitState(TAG_InitState_GameplayReady))
	{
		return false;
	}

	// Check if the argument is valid

	if (!SlotTag.IsValid())
	{
		return false;
	}

	// Remove equipment from slot

	if (auto* Instance{ EquipmentContainer.RemoveEntry(SlotTag) })
	{
		if (IsUsingRegisteredSubObjectList())
		{
			RemoveReplicatedSubObject(Instance);
		}

		return true;
	}
	
	return false;
}

void UEquipmentManagerComponent::RemoveAllEquipments()
{
	// Must have Authority

	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	// If has not game ready, skip

	if (!HasReachedInitState(TAG_InitState_GameplayReady))
	{
		return;
	}

	// Remove equipments from slots

	auto Instances{ EquipmentContainer.RemoveAllEntries() };
	for (const auto& Instance : Instances)
	{
		if (IsUsingRegisteredSubObjectList())
		{
			RemoveReplicatedSubObject(Instance);
		}
	}

	Instances.Empty();
}

#pragma endregion


#pragma region Active Slot

void UEquipmentManagerComponent::SetActiveSlot(FGameplayTag SlotTag)
{
	// Must have Authority

	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	// If has not game ready, skip

	if (!HasReachedInitState(TAG_InitState_GameplayReady))
	{
		return;
	}

	// Check if the argument is valid

	if (!SlotTag.IsValid())
	{
		return;
	}

	// Cache new active slot indexes and last active slot indices

	auto LastActiveIndex{ static_cast<int32>(INDEX_NONE) };
	auto NewActiveIndex{ static_cast<int32>(INDEX_NONE) };

	for (auto It{ EquipmentContainer.Entries.CreateIterator() }; It; ++It)
	{
		auto& Entry{ *It };

		if (Entry.Activated)
		{
			LastActiveIndex = It.GetIndex();
		}

		if (Entry.SlotTag == SlotTag)
		{
			NewActiveIndex = It.GetIndex();
		}
	}

	// Check if the new active slot index is valid

	if ((NewActiveIndex == INDEX_NONE) || (NewActiveIndex == LastActiveIndex))
	{
		return;
	}

	// Set active slot
	
	if (LastActiveIndex != INDEX_NONE)
	{
		EquipmentContainer.DeactivateEntry(LastActiveIndex);
	}

	EquipmentContainer.ActivateEntry(NewActiveIndex);
}

bool UEquipmentManagerComponent::GetActiveSlotInfo(FEquipmentSlotChangedMessage& SlotInfo)
{
	SlotInfo = FEquipmentSlotChangedMessage();

	for (const auto& Entry : EquipmentContainer.Entries)
	{
		if (Entry.Activated)
		{
			SlotInfo.OwnerComponent = this;
			SlotInfo.SlotTag = Entry.SlotTag;
			SlotInfo.Instance = Entry.Instance;
			SlotInfo.Data = Entry.Data;

			return true;
		}
	}

	return false;
}

bool UEquipmentManagerComponent::GetSlotInfo(FGameplayTag SlotTag, FEquipmentSlotChangedMessage& SlotInfo)
{
	SlotInfo = FEquipmentSlotChangedMessage();

	for (const auto& Entry : EquipmentContainer.Entries)
	{
		if (Entry.SlotTag == SlotTag)
		{
			SlotInfo.OwnerComponent = this;
			SlotInfo.SlotTag = Entry.SlotTag;
			SlotInfo.Instance = Entry.Instance;
			SlotInfo.Data = Entry.Data;

			return true;
		}
	}

	return false;
}

#pragma endregion


#pragma region Utilities

UEquipmentManagerComponent* UEquipmentManagerComponent::FindEquipmentManagerComponent(const APawn* Pawn)
{
	return (Pawn ? Pawn->FindComponentByClass<UEquipmentManagerComponent>() : nullptr);
}

#pragma endregion
