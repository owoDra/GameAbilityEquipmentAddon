// Copyright (C) 2024 owoDra

#pragma once

#include "Abilities/Tasks/AbilityTask.h"

#include "AbilityTask_CreateTargetDataBase.generated.h"


/**
 * Delegate notifies that target data has been created
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTaskTargetDataCreatedDelegate, const FGameplayAbilityTargetDataHandle&, InTargetData);

/**
 * Delegate notified that target data could not be created
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTaskTargetDataNotCreatedDelegate);


/**
 * AbilityTask base class for creating TargetData by various methods
 */
UCLASS(Abstract)
class GAEADDON_API UAbilityTask_CreateTargetDataBase : public UAbilityTask
{
	GENERATED_BODY()
public:
	UAbilityTask_CreateTargetDataBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

private:
	UPROPERTY(BlueprintAssignable, meta = (AllowPrivateAccess = true))
	FTaskTargetDataCreatedDelegate OnCreated;

	UPROPERTY(BlueprintAssignable, meta = (AllowPrivateAccess = true))
	FTaskTargetDataNotCreatedDelegate OnNotCreated;

protected:
	/**
	 * Notifies the user that target data has been created and ends the task
	 */
	void HandleTargetDataCreated(const FGameplayAbilityTargetDataHandle& InTargetData, bool bShouldFinishTask = true);

	/**
	 * Notifies the user that target data has not created and ends the task
	 */
	void HandleTargetDataNotCreated();

};
