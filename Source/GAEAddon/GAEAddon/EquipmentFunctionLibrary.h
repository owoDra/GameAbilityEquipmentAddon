// Copyright (C) 2023 owoDra

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "EquipmentFunctionLibrary.generated.h"

class UEquipmentManagerComponent;
class APawn;


UCLASS(MinimalAPI)
class UEquipmentFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	/**
	 * Get EquipmentManagerComponent from actor
	 */
	UFUNCTION(BlueprintPure, Category = "Equipment", meta = (BlueprintInternalUseOnly = "false"))
	static GAEADDON_API UEquipmentManagerComponent* GetEquipmentManagerComponentFromPawn(const APawn* Pawn, bool LookForComponent = true);

};
