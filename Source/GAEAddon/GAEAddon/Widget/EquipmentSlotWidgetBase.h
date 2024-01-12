// Copyright (C) 2024 owoDra

#pragma once

#include "Blueprint/UserWidget.h"

#include "EquipmentSlotChangeMessage.h"

#include "Type/AbilityCostStatTagTargetTypes.h"

#include "GameplayTagContainer.h"

#include "EquipmentSlotWidgetBase.generated.h"

class UEquipmentManagerComponent;
class UEquipmentInstance;
class APawn;
class AActor;


/**
 * Base widget class that provides the basic functionality for implementing widgets in EquipmentSlot
 */
UCLASS(Abstract, Blueprintable)
class GAEADDON_API UEquipmentSlotWidgetBase : public UUserWidget
{
	GENERATED_BODY()
public:
	UEquipmentSlotWidgetBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;

	void ListenPawnChange();
	void UnlistenPawnChange();

private:
	UFUNCTION()
	void HandlePawnChange(APawn* OldPawn, APawn* NewPawn);


protected:
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Components")
	TWeakObjectPtr<UEquipmentManagerComponent> EquipmentManagerComponent{ nullptr };

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Components")
	TWeakObjectPtr<UEquipmentInstance> EquipmentInstance{ nullptr };

	UPROPERTY(BlueprintReadOnly, Transient, Category = "State")
	bool bActivated{ false };

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Equipment", meta = (Categories = "Equipment.Slot"))
	FGameplayTag AssociateSlotTag;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Equipment")
	EStatTagCostTarget CostTarget;

protected:
	UFUNCTION(BlueprintCallable, Category = "Health")
	virtual void RefreshEquipmentManagerComponent(APawn* InPawn);

	UFUNCTION(BlueprintCallable, Category = "Health")
	virtual void RefreshEquipmentSlot();

	void ListenEquipmentSlotEvents();
	void UnlistenEquipmentSlotEvents();


private:
	UFUNCTION()
	void HandleSlotChanged(FEquipmentSlotChangedMessage Info);

	UFUNCTION()
	void HandleActiveSlotChanged(FEquipmentSlotChangedMessage Info);

protected:
	virtual void SetIsActiveSlot(bool bActive);
	virtual void SetEquipment(const UEquipmentData* Data, UEquipmentInstance* Instance);

	UFUNCTION(BlueprintImplementableEvent, Category = "Equipment")
	void OnSlotChanged(const UEquipmentData* Data, UEquipmentInstance* Instance);

	UFUNCTION(BlueprintImplementableEvent, Category = "Equipment")
	void OnActiveSlotChanged(bool bActive);


	//////////////////////////////////////////////////////////////
	// Utilities
protected:
	UObject* GetStatTagCostTarget() const;

	template<typename T>
	T* GetCostTarget() const
	{
		return Cast<T>(GetStatTagCostTarget());
	}

};
