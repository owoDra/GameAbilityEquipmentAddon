// Copyright (C) 2024 owoDra

#include "GameplayAbility_Equipment.h"

#include "EquipmentInstance.h"
#include "GameplayTag/GAEATags_Flag.h"
#include "GAEAddonLogs.h"

#include "AbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayAbility_Equipment)


UGameplayAbility_Equipment::UGameplayAbility_Equipment(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ActivationBlockedTags.AddTag(TAG_Flag_EquipmentUseBlocked);
}


bool UGameplayAbility_Equipment::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	if (!GetTypedSourceObject<UEquipmentInstance>())
	{
		UE_LOG(LogGAEA, Error, TEXT("Ability(%s) cannot be activated because there is no associated equipment"), *GetPathName());

		return false;
	}

	return true;
}

void UGameplayAbility_Equipment::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	BindTargetDataReadyCallback(Handle, ActivationInfo);

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UGameplayAbility_Equipment::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (IsEndAbilityValid(Handle, ActorInfo))
	{
		if (ScopeLockCount > 0)
		{
			WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &ThisClass::EndAbility, Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled));
			return;
		}

		UnbindTargetDataReadyCallback(Handle, ActivationInfo);

		Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	}
}


void UGameplayAbility_Equipment::BindTargetDataReadyCallback(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (!bShouldCallbackTargetDataReady)
	{
		return;
	}

	auto* ASC{ CurrentActorInfo->AbilitySystemComponent.Get() };
	check(ASC);

	TargetDataReadyCallbackDelegateHandle = ASC->AbilityTargetDataSetDelegate(Handle, ActivationInfo.GetActivationPredictionKey()).AddUObject(this, &ThisClass::HandleTargetDataReady);
}

void UGameplayAbility_Equipment::UnbindTargetDataReadyCallback(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (!bShouldCallbackTargetDataReady)
	{
		return;
	}

	auto* ASC{ CurrentActorInfo->AbilitySystemComponent.Get() };
	check(ASC);

	ASC->AbilityTargetDataSetDelegate(Handle, ActivationInfo.GetActivationPredictionKey()).Remove(TargetDataReadyCallbackDelegateHandle);
	ASC->ConsumeClientReplicatedTargetData(Handle, ActivationInfo.GetActivationPredictionKey());
}


void UGameplayAbility_Equipment::HandleTargetDataReady(const FGameplayAbilityTargetDataHandle& InData, FGameplayTag ApplicationTag)
{
	auto* ASC{ CurrentActorInfo->AbilitySystemComponent.Get() };
	check(ASC);

	auto ScopedPrediction{ FScopedPredictionWindow(ASC) };

	// Take ownership of the target data to make sure no callbacks into game code invalidate it out from under us

	auto LocalTargetDataHandle{ FGameplayAbilityTargetDataHandle(MoveTemp(const_cast<FGameplayAbilityTargetDataHandle&>(InData))) };

	// Notify to server if need

	const auto bShouldNotifyServer{ CurrentActorInfo->IsLocallyControlled() && !CurrentActorInfo->IsNetAuthority() };
	if (bShouldNotifyServer)
	{
		ASC->CallServerSetReplicatedTargetData(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey(), LocalTargetDataHandle, ApplicationTag, ASC->ScopedPredictionKey);
	}

	// Do some process in blueprint

	OnTargetDataReady(LocalTargetDataHandle);

	ASC->ConsumeClientReplicatedTargetData(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey());
}

void UGameplayAbility_Equipment::SendTargetData(const FGameplayAbilityTargetDataHandle& InTargetData)
{
	if (!bShouldCallbackTargetDataReady)
	{
		UE_LOG(LogGAEA, Warning, TEXT("UGameplayAbility_Equipment::SendTargetData: bShouldCallbackTargetDataReady must be true to use this function"));
		return;
	}

	if (IsLocallyControlled())
	{
		HandleTargetDataReady(InTargetData, FGameplayTag());
	}
}
