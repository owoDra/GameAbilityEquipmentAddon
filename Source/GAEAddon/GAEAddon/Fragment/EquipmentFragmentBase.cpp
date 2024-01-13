// Copyright (C) 2024 owoDra

#include "EquipmentFragmentBase.h"

#include "EquipmentInstance.h"
#include "GAEAddonLogs.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EquipmentFragmentBase)


UEquipmentFragmentBase::UEquipmentFragmentBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


#if WITH_EDITOR 
UClass* UEquipmentFragmentBase::GetRequiredInstanceClass() const
{
	return UEquipmentInstance::StaticClass();
}
#endif // WITH_EDITOR

void UEquipmentFragmentBase::OnEquiped(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const
{
	const auto* Pawn{ Instance ? Instance->GetPawn() : nullptr };

	UE_LOG(LogGAEA, Log, TEXT("[%s|%s] %s::OnEquiped")
		, Pawn ? (Pawn->HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT")) : TEXT("INVALID")
		, Pawn ? (Pawn->IsLocallyControlled() ? TEXT("Local") : TEXT("Other")) : TEXT("INVALID")
		, *GetNameSafe(this));
}

void UEquipmentFragmentBase::OnUnequiped(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const
{
	const auto* Pawn{ Instance ? Instance->GetPawn() : nullptr };

	UE_LOG(LogGAEA, Log, TEXT("[%s|%s] %s::OnUnequiped")
		, Pawn ? (Pawn->HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT")) : TEXT("INVALID")
		, Pawn ? (Pawn->IsLocallyControlled() ? TEXT("Local") : TEXT("Other")) : TEXT("INVALID")
		, *GetNameSafe(this));
}

void UEquipmentFragmentBase::OnActivated(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const
{
	const auto* Pawn{ Instance ? Instance->GetPawn() : nullptr };

	UE_LOG(LogGAEA, Log, TEXT("[%s|%s] %s::OnActivated")
		, Pawn ? (Pawn->HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT")) : TEXT("INVALID")
		, Pawn ? (Pawn->IsLocallyControlled() ? TEXT("Local") : TEXT("Other")) : TEXT("INVALID")
		, *GetNameSafe(this));
}

void UEquipmentFragmentBase::OnDeactivated(UEquipmentManagerComponent* EMC, UEquipmentInstance* Instance) const
{
	const auto* Pawn{ Instance ? Instance->GetPawn() : nullptr };

	UE_LOG(LogGAEA, Log, TEXT("[%s|%s] %s::OnDeactivated")
		, Pawn ? (Pawn->HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT")) : TEXT("INVALID")
		, Pawn ? (Pawn->IsLocallyControlled() ? TEXT("Local") : TEXT("Other")) : TEXT("INVALID")
		, *GetNameSafe(this));
}

