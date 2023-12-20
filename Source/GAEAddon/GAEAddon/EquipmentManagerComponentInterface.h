// Copyright (C) 2023 owoDra

#pragma once

#include "UObject/Interface.h"

#include "EquipmentManagerComponentInterface.generated.h"

class UEquipmentManagerComponent;


/** 
 * Interface for actors that expose access to an equipment manager component 
 */
UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UEquipmentManagerComponentInterface : public UInterface
{
	GENERATED_BODY()
public:
	UEquipmentManagerComponentInterface(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

};

class GAEADDON_API IEquipmentManagerComponentInterface
{
	GENERATED_BODY()
public:
	IEquipmentManagerComponentInterface() {}

public:
	/** 
	 * Returns the equipment manager component to use for this actor. It may live on another actor, such as a Pawn using the PlayerState's component 
	 */
	virtual UEquipmentManagerComponent* GetEquipmentManagerComponent() const = 0;

};
