// Copyright (C) 2023 owoDra

#include "EquipmentContainer.h"

#include "EquipmentData.h"
#include "EquipmentInstance.h"
#include "EquipmentManagerComponent.h"
#include "GAEAddonLogs.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EquipmentContainer)


//////////////////////////////////////////////////////////////////////
// FBEEquipmentEntry

#pragma region FBEEquipmentEntry

FString FEquipmentEntry::GetDebugString() const
{
	return FString::Printf(TEXT("(Slot: %s) Data: %s, Instance: %s"), *SlotTag.GetTagName().ToString(), *GetNameSafe(Data), *GetNameSafe(Instance));
}

bool FEquipmentEntry::IsValid() const
{
	return (Data != nullptr) && (Instance != nullptr) && (SlotTag.IsValid());
}

#pragma endregion


//////////////////////////////////////////////////////////////////////
// FEquipmentContainer

#pragma region FEquipmentContainer

void FEquipmentContainer::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
 	for (const auto& Index : RemovedIndices)
 	{
		const auto& Entry{ Entries[Index] };
		if (Entry.IsValid())
		{
			const auto& Instance{ Entry.Instance };
			const auto& Data{ Entry.Data };

			if (Entry.Activated == true)
			{
				Instance->OnDeactivated(OwnerComponent, Data);
			}

			Instance->OnUnequiped(OwnerComponent, Data);

			BroadcastSlotChangeMessage(Entry.SlotTag);
		}
 	}
}

void FEquipmentContainer::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (const auto& Index : AddedIndices)
	{
		const auto& Entry{ Entries[Index] };
		if (Entry.IsValid())
		{
			const auto& Instance{ Entry.Instance };
			const auto& Data{ Entry.Data };

			Instance->OnEquiped(OwnerComponent, Data);

			BroadcastSlotChangeMessage(Entry.SlotTag, Data, Instance);

			if (Entry.Activated == true)
			{
				Instance->OnActivated(OwnerComponent, Data);

				BroadcastActiveSlotChangeMessage(Entry.SlotTag, Data, Instance);
			}
		}
	}
}

void FEquipmentContainer::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (const auto& Index : ChangedIndices)
	{
		const auto& Entry{ Entries[Index] };
		if (Entry.IsValid())
		{
			const auto& Instance{ Entry.Instance };
			const auto& Data{ Entry.Data };

			if (Entry.Activated == true)
			{
				Instance->OnActivated(OwnerComponent, Data);

				BroadcastActiveSlotChangeMessage(Entry.SlotTag, Data, Instance);
			}
			else
			{
				Instance->OnDeactivated(OwnerComponent, Data);
			}
		}
	}
}


UEquipmentInstance* FEquipmentContainer::AddEntry(const UEquipmentData* EquipmentData, FGameplayTag SlotTag)
{
	if (!EquipmentData)
	{
		UE_LOG(LogGAEA, Warning, TEXT("Invalid EquipmentData was attempted to be added."));

		return nullptr;
	}

	// Is it trying to add to the allowed slots?

	if (!EquipmentData->AllowedSlotTags.IsEmpty())
	{
		if (!EquipmentData->AllowedSlotTags.HasTag(SlotTag))
		{
			return nullptr;
		}
	}
	
	// Create instance

	auto InstanceType{ UEquipmentInstance::StaticClass() };
	if (EquipmentData->InstanceType)
	{
		InstanceType = EquipmentData->InstanceType;
	}
	
	auto& NewEntry{ Entries.AddDefaulted_GetRef() };
	NewEntry.SlotTag = SlotTag;
	NewEntry.Data = EquipmentData;
	NewEntry.Instance = NewObject<UEquipmentInstance>(OwnerComponent->GetOwner(), InstanceType);
	NewEntry.Instance->OnEquiped(OwnerComponent, EquipmentData);
	
	MarkItemDirty(NewEntry);

	return NewEntry.Instance;
}

UEquipmentInstance* FEquipmentContainer::RemoveEntry(FGameplayTag SlotTag)
{
	for (auto It{ Entries.CreateIterator() }; It; ++It)
	{
		auto& Entry{ *It };

		if (Entry.SlotTag == SlotTag)
		{
			auto Instance{ Entry.Instance };

			if (Instance)
			{
				const auto& Data{ Entry.Data };

				if (Entry.Activated)
				{
					Instance->OnDeactivated(OwnerComponent, Data);
				}

				Instance->OnUnequiped(OwnerComponent, Data);
			}

			It.RemoveCurrent();

			MarkArrayDirty();

			return Instance;
		}
	}

	return nullptr;
}

TArray<UEquipmentInstance*> FEquipmentContainer::RemoveAllEntries()
{
	TArray<UEquipmentInstance*> RemovingInstances;

	for (auto It{ Entries.CreateIterator() }; It; ++It)
	{
		auto& Entry{ *It };
		auto Instance{ Entry.Instance };

		if (Instance)
		{
			const auto& Data{ Entry.Data };

			if (Entry.Activated)
			{
				Instance->OnDeactivated(OwnerComponent, Data);
			}

			Instance->OnUnequiped(OwnerComponent, Data);
		}

		RemovingInstances.Add(Instance);

		It.RemoveCurrent();

		MarkArrayDirty();

	}

	return RemovingInstances;
}


void FEquipmentContainer::ActivateEntry(int32 SlotIndex)
{
	if (Entries.IsValidIndex(SlotIndex))
	{
		auto& Entry{ Entries[SlotIndex] };

		if (auto Instance{ Entry.Instance })
		{
			Instance->OnActivated(OwnerComponent, Entry.Data);
		}

		Entry.Activated = true;

		MarkItemDirty(Entry);
	}
}

void FEquipmentContainer::DeactivateEntry(int32 SlotIndex)
{
	if (Entries.IsValidIndex(SlotIndex))
	{
		auto& Entry{ Entries[SlotIndex] };

		if (auto Instance{ Entry.Instance })
		{
			Instance->OnDeactivated(OwnerComponent, Entry.Data);
		}

		Entry.Activated = false;

		MarkItemDirty(Entry);
	}
}


void FEquipmentContainer::BroadcastSlotChangeMessage(FGameplayTag SlotTag, const UEquipmentData* EquipmentData, UEquipmentInstance* Instance)
{
}

void FEquipmentContainer::BroadcastActiveSlotChangeMessage(FGameplayTag SlotTag, const UEquipmentData* EquipmentData, UEquipmentInstance* Instance)
{
}

#pragma endregion
