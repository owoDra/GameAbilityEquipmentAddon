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
}


void UEquipmentManagerComponent::OnRegister()
{
	Super::OnRegister();

	// This component can only be added to classes derived from APawn

	const auto* Pawn{ GetPawn<APawn>() };
	ensureAlwaysMsgf((Pawn != nullptr), TEXT("EquipmentManagerComponent on [%s] can only be added to Pawn actors."), *GetNameSafe(GetOwner()));

	// No more than two of these components should be added to a Actor.

	TArray<UActorComponent*> Components;
	GetOwner()->GetComponents(UEquipmentManagerComponent::StaticClass(), Components);
	ensureAlwaysMsgf((Components.Num() == 1), TEXT("Only one EquipmentManagerComponent should exist on [%s]."), *GetNameSafe(GetOwner()));

	// Register this component in the GameFrameworkComponentManager.

	RegisterInitStateFeature();
}

void UEquipmentManagerComponent::OnUnregister()
{
	UninitializeFromAbilitySystem();

	Super::OnUnregister();
}

void UEquipmentManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// Start listening for changes in the initialization state of all features 
	// related to the Pawn that owns this component.

	BindOnActorInitStateChanged(NAME_None, FGameplayTag(), false);

	// Change the initialization state of this component to [Spawned]

	ensure(TryToChangeInitState(TAG_InitState_Spawned));

	// Check if initialization process can continue

	CheckDefaultInitialization();
}

void UEquipmentManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();
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
		UE_LOG(LogGAEA, Error, TEXT("EquipmentManagerComponent: Cannot initialize health component for owner [%s] with NULL ability system."), *GetNameSafe(Pawn));
		return;
	}

	CheckDefaultInitialization();
}

void UEquipmentManagerComponent::UninitializeFromAbilitySystem()
{
	if (AbilitySystemComponent)
	{
		RemoveAllEquipments();
	}

	AbilitySystemComponent = nullptr;
}


bool UEquipmentManagerComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	auto* Pawn{ GetPawn<APawn>() };

	/**
	 * [InitState None] -> [InitState Spawned]
	 */
	if (!CurrentState.IsValid() && DesiredState == TAG_InitState_Spawned)
	{
		if (Pawn != nullptr)
		{
			return true;
		}
	}

	/**
	 * [InitState Spawned] -> [InitState DataAvailable]
	 */
	else if (CurrentState == TAG_InitState_Spawned && DesiredState == TAG_InitState_DataAvailable)
	{
		return Manager->HasFeatureReachedInitState(Pawn, UGAEAbilitySystemComponent::NAME_ActorFeatureName, TAG_InitState_DataInitialized);
	}

	/**
	 * [InitState DataAvailable] -> [InitState DataInitialized]
	 */
	else if (CurrentState == TAG_InitState_DataAvailable && DesiredState == TAG_InitState_DataInitialized)
	{
		if (AbilitySystemComponent)
		{
			return true;
		}
	}

	/**
	 * [InitState DataInitialized] -> [InitState GameplayReady]
	 */
	else if (CurrentState == TAG_InitState_DataInitialized && DesiredState == TAG_InitState_GameplayReady)
	{
		return true;
	}

	return false;
}

void UEquipmentManagerComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	UE_LOG(LogGAEA, Log, TEXT("[%s] Equipment Manager Component InitState Reached: %s"),
		GetOwner()->HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT"), *DesiredState.GetTagName().ToString());

	/**
	 * [InitState Spawned] -> [InitState DataAvailable]
	 */
	if (CurrentState == TAG_InitState_Spawned && DesiredState == TAG_InitState_DataAvailable)
	{
		InitializeWithAbilitySystem();
	}
}

void UEquipmentManagerComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	// Wait for initialization of AbilitySystemCompoenet

	if (Params.FeatureName == UGAEAbilitySystemComponent::NAME_ActorFeatureName)
	{
		if ((Params.FeatureState == TAG_InitState_DataInitialized) || (Params.FeatureState == TAG_InitState_GameplayReady))
		{
			CheckDefaultInitialization();
		}
	}
}

void UEquipmentManagerComponent::CheckDefaultInitialization()
{
	static const TArray<FGameplayTag> StateChain
	{
		TAG_InitState_Spawned,
		TAG_InitState_DataAvailable,
		TAG_InitState_DataInitialized,
		TAG_InitState_GameplayReady
	};

	ContinueInitStateChain(StateChain);
}


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


bool UEquipmentManagerComponent::AddEquipment(FGameplayTag SlotTag, const UEquipmentData* EquipmentData, bool ActivateImmediately)
{
	// Must have Authority

	if (!GetOwner()->HasAuthority())
	{
		return false;
	}

	// Check AbilitySystemComponent

	if (!AbilitySystemComponent)
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

void UEquipmentManagerComponent::ResetEquipments(const TArray<FEquipmentSetEntry>& Entries, FGameplayTag ActivateSlotTag)
{
	// Must have Authority

	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	// Check AbilitySystemComponent

	if (!AbilitySystemComponent)
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

bool UEquipmentManagerComponent::RemoveEquipment(FGameplayTag SlotTag)
{
	// Must have Authority

	if (!GetOwner()->HasAuthority())
	{
		return false;
	}

	// Check AbilitySystemComponent

	if (!AbilitySystemComponent)
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

	// Check AbilitySystemComponent

	if (!AbilitySystemComponent)
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


void UEquipmentManagerComponent::SetActiveSlot(FGameplayTag SlotTag)
{
	// Must have Authority

	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	// Check AbilitySystemComponent

	if (!AbilitySystemComponent)
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


UEquipmentManagerComponent* UEquipmentManagerComponent::FindEquipmentManagerComponent(const APawn* Pawn)
{
	return (Pawn ? Pawn->FindComponentByClass<UEquipmentManagerComponent>() : nullptr);
}
