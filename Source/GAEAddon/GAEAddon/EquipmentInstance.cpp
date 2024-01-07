// Copyright (C) 2023 owoDra

#include "EquipmentInstance.h"

#include "EquipmentData.h"
#include "GAEAddonLogs.h"

#include "Components/SkeletalMeshComponent.h"
#include "Net/UnrealNetwork.h"

#if UE_WITH_IRIS
#include "Iris/ReplicationSystem/ReplicationFragmentUtil.h"
#endif // UE_WITH_IRIS

#include UE_INLINE_GENERATED_CPP_BY_NAME(EquipmentInstance)


UEquipmentInstance::UEquipmentInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, StatTags(this)
{
}

void UEquipmentInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, StatTags);
}

#if UE_WITH_IRIS
void UEquipmentInstance::RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context, UE::Net::EFragmentRegistrationFlags RegistrationFlags)
{
	using namespace UE::Net;

	Super::RegisterReplicationFragments(Context, RegistrationFlags);

	// Build descriptors and allocate PropertyReplicationFragments for this object

	FReplicationFragmentUtil::CreateAndRegisterFragmentsForObject(this, Context, RegistrationFlags);
}
#endif // UE_WITH_IRIS

UWorld* UEquipmentInstance::GetWorld() const
{
	auto* Outer{ GetOuter() };
	
	return Outer ? Outer->GetWorld() : nullptr;
}


void UEquipmentInstance::OnEquiped(UEquipmentManagerComponent* EMC, const UEquipmentData* InEquipmentData)
{
	check(InEquipmentData);

	InEquipmentData->HandleEquiped(EMC, this);
}

void UEquipmentInstance::OnUnequiped(UEquipmentManagerComponent* EMC, const UEquipmentData* InEquipmentData)
{
	check(InEquipmentData);

	InEquipmentData->HandleUnequiped(EMC, this);
}

void UEquipmentInstance::OnActivated(UEquipmentManagerComponent* EMC, const UEquipmentData* InEquipmentData)
{
	check(InEquipmentData);

	InEquipmentData->HandleActivated(EMC, this);
}

void UEquipmentInstance::OnDeactivated(UEquipmentManagerComponent* EMC, const UEquipmentData* InEquipmentData)
{
	check(InEquipmentData);

	InEquipmentData->HandleDeactivated(EMC, this);
}


void UEquipmentInstance::AddStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	StatTags.AddStack(Tag, StackCount);
}

void UEquipmentInstance::RemoveStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	StatTags.RemoveStack(Tag, StackCount);
}

int32 UEquipmentInstance::GetStatTagStackCount(FGameplayTag Tag) const
{
	return StatTags.GetStackCount(Tag);
}

bool UEquipmentInstance::HasStatTag(FGameplayTag Tag) const
{
	return StatTags.ContainsTag(Tag);
}


void UEquipmentInstance::SpawnEquipmentMeshes(USkeletalMeshComponent* TargetMesh, const TArray<FEquipmentMeshToSpawn>& InMeshesToSpawn)
{
	if (InMeshesToSpawn.IsEmpty())
	{
		return;
	}

	check(TargetMesh);

	// Spawn meshes

	const auto bOwnerNoSee{ static_cast<bool>(TargetMesh->bOwnerNoSee) };
	const auto bOnlyOwnerSee{ static_cast<bool>(TargetMesh->bOnlyOwnerSee) };
	const auto bHiddenInGame{ static_cast<bool>(TargetMesh->bHiddenInGame) };
	const auto bCastShadow{ static_cast<bool>(TargetMesh->CastShadow) };

	UE_LOG(LogGAEA, Log, TEXT("[%s] Create Equipment Meshes for %s"),
		(GetWorld()->GetNetMode() == ENetMode::NM_Client) ? TEXT("CLIENT") : TEXT("SERVER"),
		*GetNameSafe(TargetMesh));

	for (const auto& SpawnInfo : InMeshesToSpawn)
	{
		if (SpawnInfo.MeshToSpawn)
		{
			USkeletalMeshComponent* NewMesh = NewObject<USkeletalMeshComponent>(TargetMesh->GetOwner());
			NewMesh->SetSkeletalMesh(SpawnInfo.MeshToSpawn);
			NewMesh->SetAnimInstanceClass(SpawnInfo.MeshAnimInstance);
			NewMesh->SetRelativeTransform(SpawnInfo.AttachTransform);
			NewMesh->AttachToComponent(TargetMesh, FAttachmentTransformRules::KeepRelativeTransform, SpawnInfo.AttachSocket);
			NewMesh->SetOwnerNoSee(bOwnerNoSee);
			NewMesh->SetOnlyOwnerSee(bOnlyOwnerSee);
			NewMesh->SetHiddenInGame(bHiddenInGame);
			NewMesh->SetCastShadow(bCastShadow);
			NewMesh->RegisterComponent();

			SpawnedMeshes.Add(NewMesh);
		}
	}
}

void UEquipmentInstance::DestroyEquipmentMeshes()
{
	for (const auto& Mesh : SpawnedMeshes)
	{
		if (Mesh)
		{
			Mesh->DestroyComponent();
		}
	}

	SpawnedMeshes.Empty();
}


void UEquipmentInstance::GrantAbilitySet_Equip(const UAbilitySet* AbillitySet, UAbilitySystemComponent* ASC)
{
	GrantAbilitySet(AbillitySet, ASC, GrantedHandles_Equip);
}

void UEquipmentInstance::GrantAbilitySet_Active(const UAbilitySet* AbillitySet, UAbilitySystemComponent* ASC)
{
	GrantAbilitySet(AbillitySet, ASC, GrantedHandles_Active);
}

void UEquipmentInstance::GrantAbilitySet(const UAbilitySet* AbillitySet, UAbilitySystemComponent* ASC, FAbilitySet_GrantedHandles& OutHandles)
{
	check(AbillitySet);
	check(ASC);

	AbillitySet->GiveToAbilitySystem(ASC, &OutHandles, this);
}

void UEquipmentInstance::RemoveAbilities_Equip(UAbilitySystemComponent* ASC)
{
	GrantedHandles_Equip.TakeFromAbilitySystem(ASC);
}

void UEquipmentInstance::RemoveAbilities_Active(UAbilitySystemComponent* ASC)
{
	GrantedHandles_Active.TakeFromAbilitySystem(ASC);
}


void UEquipmentInstance::ApplyAnimLayer(UAnimInstance* TargetAnimInstance, TSubclassOf<UAnimInstance> InLayer)
{
	check(TargetAnimInstance);

	if (InLayer)
	{
		TargetAnimInstance->LinkAnimClassLayers(InLayer);
	}
}

void UEquipmentInstance::RemoveAnimLayer(UAnimInstance* TargetAnimInstance, TSubclassOf<UAnimInstance> InLayer)
{
	check(TargetAnimInstance);

	if (InLayer)
	{
		TargetAnimInstance->UnlinkAnimClassLayers(InLayer);
	}
}
