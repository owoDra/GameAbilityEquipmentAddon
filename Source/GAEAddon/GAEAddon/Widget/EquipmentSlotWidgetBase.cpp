// Copyright (C) 2024 owoDra

#include "EquipmentSlotWidgetBase.h"

#include "EquipmentFunctionLibrary.h"
#include "EquipmentManagerComponent.h"

#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/Pawn.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EquipmentSlotWidgetBase)


UEquipmentSlotWidgetBase::UEquipmentSlotWidgetBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


void UEquipmentSlotWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ListenPawnChange();

	RefreshEquipmentManagerComponent(GetOwningPlayerPawn());
}

void UEquipmentSlotWidgetBase::NativeDestruct()
{
	Super::NativeDestruct();

	UnlistenEquipmentSlotEvents();
	UnlistenPawnChange();
}


void UEquipmentSlotWidgetBase::ListenPawnChange()
{
	auto* PlayerController{ GetOwningPlayer() };

	if (ensure(PlayerController))
	{
		FScriptDelegate NewDelegate;
		NewDelegate.BindUFunction(this, GET_FUNCTION_NAME_CHECKED(UEquipmentSlotWidgetBase, HandlePawnChange));
		PlayerController->OnPossessedPawnChanged.Add(NewDelegate);
	}
}

void UEquipmentSlotWidgetBase::UnlistenPawnChange()
{
	auto* PlayerController{ GetOwningPlayer() };

	if (ensure(PlayerController))
	{
		PlayerController->OnPossessedPawnChanged.RemoveAll(this);
	}
}

void UEquipmentSlotWidgetBase::HandlePawnChange(APawn* OldPawn, APawn* NewPawn)
{
	RefreshEquipmentManagerComponent(NewPawn);
}


void UEquipmentSlotWidgetBase::RefreshEquipmentManagerComponent(APawn* InPawn)
{
	if (auto* NewEquipmentManagerComponent{ UEquipmentFunctionLibrary::GetEquipmentManagerComponentFromPawn(InPawn) })
	{
		UnlistenEquipmentSlotEvents();

		EquipmentManagerComponent = NewEquipmentManagerComponent;

		ListenEquipmentSlotEvents();

		RefreshEquipmentSlot();
	}
}

void UEquipmentSlotWidgetBase::RefreshEquipmentSlot()
{
	if (EquipmentManagerComponent.IsValid())
	{
		FEquipmentSlotChangedMessage Info;

		if (EquipmentManagerComponent->GetSlotInfo(AssociateSlotTag, Info))
		{
			SetEquipment(Info.Data, Info.Instance);
		}
		else
		{
			SetEquipment(nullptr, nullptr);
		}

		if (EquipmentManagerComponent->GetActiveSlotInfo(Info))
		{
			SetIsActiveSlot(Info.SlotTag == AssociateSlotTag);
		}
		else
		{
			SetIsActiveSlot(false);
		}
	}
}

void UEquipmentSlotWidgetBase::ListenEquipmentSlotEvents()
{
	if (EquipmentManagerComponent.IsValid())
	{
		{
			FScriptDelegate NewDelegate;
			NewDelegate.BindUFunction(this, GET_FUNCTION_NAME_CHECKED(UEquipmentSlotWidgetBase, HandleSlotChanged));
			EquipmentManagerComponent->OnEquipmentSlotChange.Add(NewDelegate);
		}

		{
			FScriptDelegate NewDelegate;
			NewDelegate.BindUFunction(this, GET_FUNCTION_NAME_CHECKED(UEquipmentSlotWidgetBase, HandleActiveSlotChanged));
			EquipmentManagerComponent->OnActiveEquipmentSlotChange.Add(NewDelegate);
		}
	}
}

void UEquipmentSlotWidgetBase::UnlistenEquipmentSlotEvents()
{
	if (EquipmentManagerComponent.IsValid())
	{
		EquipmentManagerComponent->OnEquipmentSlotChange.RemoveAll(this);
		EquipmentManagerComponent->OnActiveEquipmentSlotChange.RemoveAll(this);
	}
}


void UEquipmentSlotWidgetBase::HandleSlotChanged(FEquipmentSlotChangedMessage Info)
{
	if (Info.SlotTag == AssociateSlotTag)
	{
		SetEquipment(Info.Data, Info.Instance);
	}
}

void UEquipmentSlotWidgetBase::HandleActiveSlotChanged(FEquipmentSlotChangedMessage Info)
{
	SetIsActiveSlot(Info.SlotTag == AssociateSlotTag);
}

void UEquipmentSlotWidgetBase::SetIsActiveSlot(bool bActive)
{
	if (bActivated != bActive)
	{
		bActivated = bActive;

		OnActiveSlotChanged(bActivated);
	}
}

void UEquipmentSlotWidgetBase::SetEquipment(const UEquipmentData* Data, UEquipmentInstance* Instance)
{
	EquipmentInstance = Instance;

	OnSlotChanged(Data, Instance);
}


UObject* UEquipmentSlotWidgetBase::GetStatTagCostTarget() const
{
	switch (CostTarget)
	{
	case EStatTagCostTarget::SourceObject:
		return EquipmentInstance.Get();
		break;

	case EStatTagCostTarget::Avatar:
		return GetOwningPlayerPawn();
		break;

	case EStatTagCostTarget::Owner:
		return GetOwningPlayerPawn();
		break;

	case EStatTagCostTarget::Controller:
		return GetOwningPlayer();
		break;

	case EStatTagCostTarget::PlayerState:
		return GetOwningPlayerState();
		break;

	default:
		return nullptr;
		break;
	}
}
