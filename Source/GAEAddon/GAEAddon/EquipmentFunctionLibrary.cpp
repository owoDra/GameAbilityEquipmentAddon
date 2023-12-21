// Copyright (C) 2023 owoDra

#include "EquipmentFunctionLibrary.h"

#include "EquipmentManagerComponent.h"
#include "EquipmentManagerComponentInterface.h"

#include "GameFramework/Pawn.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EquipmentFunctionLibrary)


UEquipmentManagerComponent* UEquipmentFunctionLibrary::GetEquipmentManagerComponentFromPawn(const APawn* Pawn, bool LookForComponent)
{
	if (!Pawn)
	{
		return nullptr;
	}

	const auto* EMCI{ Cast<IEquipmentManagerComponentInterface>(Pawn) };
	if (EMCI)
	{
		return EMCI->GetEquipmentManagerComponent();
	}

	if (LookForComponent)
	{
		return UEquipmentManagerComponent::FindEquipmentManagerComponent(Pawn);
	}

	return nullptr;
}
