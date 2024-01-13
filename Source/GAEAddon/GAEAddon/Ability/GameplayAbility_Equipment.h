// Copyright (C) 2024 owoDra

#pragma once

#include "GAEGameplayAbility.h"

#include "GameplayAbility_Equipment.generated.h"


/**
 * Base GameplayAbility class for Equipment
 */
UCLASS(Abstract)
class GAEADDON_API UGameplayAbility_Equipment : public UGAEGameplayAbility
{
	GENERATED_BODY()
public:
	UGameplayAbility_Equipment(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle
		, const FGameplayAbilityActorInfo* ActorInfo
		, const FGameplayTagContainer* SourceTags = nullptr
		, const FGameplayTagContainer* TargetTags = nullptr
		, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle
		, const FGameplayAbilityActorInfo* ActorInfo
		, const FGameplayAbilityActivationInfo ActivationInfo
		, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle
		, const FGameplayAbilityActorInfo* ActorInfo
		, const FGameplayAbilityActivationInfo ActivationInfo
		, bool bReplicateEndAbility
		, bool bWasCancelled) override;


protected:
	//
	// Whether to receive callbacks for TargetData
	//
	UPROPERTY(AdvancedDisplay, EditDefaultsOnly, Category = "Targeting")
	bool bShouldCallbackTargetDataReady{ false };

	//
	// Delegate handle to indicate that TargetData has been synchronized between Client and Server.
	//
	FDelegateHandle TargetDataReadyCallbackDelegateHandle;

protected:
	void BindTargetDataReadyCallback(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActivationInfo ActivationInfo);
	void UnbindTargetDataReadyCallback(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActivationInfo ActivationInfo);

	/**
	 * Executed when TargetData is synchronized with Client and Server and when TargetData is ready
	 */
	void HandleTargetDataReady(const FGameplayAbilityTargetDataHandle& InData, FGameplayTag ApplicationTag);

	/**
	 * Executed when TargetData is synchronized with Client and Server and when TargetData is ready
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Targeting")
	void OnTargetDataReady(const FGameplayAbilityTargetDataHandle& TargetData);

	/**
	 * This function sends TargetData created by the client to the server and synchronizes it.
	 */
	UFUNCTION(BlueprintCallable, Category = "Targeting")
	void SendTargetData(const FGameplayAbilityTargetDataHandle& InTargetData);

};
