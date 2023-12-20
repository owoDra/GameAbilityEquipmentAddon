// Copyright (C) 2023 owoDra

#include "EquipmentSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EquipmentSet)


#define LOCTEXT_NAMESPACE "EquipmentSet"

#if WITH_EDITOR
EDataValidationResult UEquipmentSet::IsDataValid(TArray<FText>& ValidationErrors)
{
	auto Result{ CombineDataValidationResults(Super::IsDataValid(ValidationErrors), EDataValidationResult::Valid) };

	//@TODO

	return Result;
}
#endif

#undef LOCTEXT_NAMESPACE
