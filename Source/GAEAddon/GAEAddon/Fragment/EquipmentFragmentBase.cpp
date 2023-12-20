// Copyright (C) 2023 owoDra

#include "EquipmentFragmentBase.h"

#include "EquipmentInstance.h"

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
