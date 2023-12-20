// Copyright (C) 2023 owoDra

#include "EquipmentData.h"

#include "Fragment/EquipmentFragmentBase.h"
#include "EquipmentInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EquipmentData)


#define LOCTEXT_NAMESPACE "EquipmentData"

#if WITH_EDITOR
EDataValidationResult UEquipmentData::IsDataValid(TArray<FText>& ValidationErrors)
{
	auto Result{ CombineDataValidationResults(Super::IsDataValid(ValidationErrors), EDataValidationResult::Valid) };

	// Check all fragments

	auto* CurrentInstanceType{ UEquipmentInstance::StaticClass() };
	if (InstanceType)
	{
		CurrentInstanceType = InstanceType;
	}

	auto EntryIndex{ 0 };
	for (const auto& Fragment : Fragments)
	{
		if (Fragment)
		{
			const auto* RequiedClass{ Fragment->GetRequiredInstanceClass() };

			if (!CurrentInstanceType->IsChildOf(RequiedClass))
			{
				Result = EDataValidationResult::Invalid;
				ValidationErrors.Add(
					FText::Format(
						LOCTEXT(
							"FragmentRequiedInstanceWrong",
							"The instance class requested({0}) by the fragment entered in the {1} index does not match the current InstanceType({2})"),
						FText::FromString(*GetNameSafe(RequiedClass)),
						FText::AsNumber(EntryIndex),
						FText::FromString(*GetNameSafe(CurrentInstanceType))
					));
			}
			else
			{
				auto ChildResult{ Fragment->IsDataValid(ValidationErrors) };
				Result = CombineDataValidationResults(Result, ChildResult);
			}
		}
		else
		{
			Result = EDataValidationResult::Invalid;
			ValidationErrors.Add(FText::Format(LOCTEXT("FragmentEntryIsNull", "Null entry at index {0} in Fragments"), FText::AsNumber(EntryIndex)));
		}

		++EntryIndex;
	}

	// Check DisplayName

	if (DisplayName.IsEmpty())
	{
		Result = EDataValidationResult::Invalid;
		ValidationErrors.Add(FText::Format(LOCTEXT("FragmentInvalidDisplayName", "Invalid Display Name for [{0}]."), FText::AsCultureInvariant(GetNameSafe(this))));
	}

	// Verify that no child classes are created by BP
	// (C++ -> BP is allowed, but BP -> BP is prohibited)

	if (!GetClass()->IsNative())
	{
		auto* ParentClass{ GetClass()->GetSuperClass() };

		// Find the native parent

		auto* FirstNativeParent{ ParentClass };
		while ((FirstNativeParent != nullptr) && !FirstNativeParent->IsNative())
		{
			FirstNativeParent = FirstNativeParent->GetSuperClass();
		}

		if (FirstNativeParent != ParentClass)
		{
			ValidationErrors.Add(FText::Format(LOCTEXT("ItemInheritenceIsUnsupported", "Blueprint subclasses of Blueprint experiences is not currently supported. Parent class was {0} but should be {1}."),
				FText::AsCultureInvariant(GetPathNameSafe(ParentClass)),
				FText::AsCultureInvariant(GetPathNameSafe(FirstNativeParent))
			));
			Result = EDataValidationResult::Invalid;
		}
	}

	return Result;
}
#endif

#if WITH_EDITORONLY_DATA
void UEquipmentData::UpdateAssetBundleData()
{
	Super::UpdateAssetBundleData();

	for (const auto& Fragment : Fragments)
	{
		if (Fragment)
		{
			Fragment->AddAdditionalAssetBundleData(AssetBundleData);
		}
	}
}
#endif


void UEquipmentData::HandleEquiped(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const
{
	for (const auto& Fragment : Fragments)
	{
		Fragment->OnEquiped(EMC, Instance);
	}
}

void UEquipmentData::HandleUnequiped(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const
{
	for (const auto& Fragment : Fragments)
	{
		Fragment->OnUnequiped(EMC, Instance);
	}
}

void UEquipmentData::HandleActivated(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const
{
	for (const auto& Fragment : Fragments)
	{
		Fragment->OnActivated(EMC, Instance);
	}
}

void UEquipmentData::HandleDeactivated(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const
{
	for (const auto& Fragment : Fragments)
	{
		Fragment->OnDeactivated(EMC, Instance);
	}
}


const UEquipmentFragmentBase* UEquipmentData::FindFragmentByClass(TSubclassOf<UEquipmentFragmentBase> FragmentClass) const
{
	if (FragmentClass != nullptr)
	{
		for (const auto& Fragment : Fragments)
		{
			if (Fragment && Fragment->IsA(FragmentClass))
			{
				return Fragment;
			}
		}
	}

	return nullptr;
}

#undef LOCTEXT_NAMESPACE
