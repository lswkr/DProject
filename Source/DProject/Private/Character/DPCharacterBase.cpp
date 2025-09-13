// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/DPCharacterBase.h"


#include "AbilitySystemComponent.h"
#include "DPGameplayTags.h"
#include "AbilitySystem/DPAbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "MotionWarpingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


ADPCharacterBase::ADPCharacterBase()
{

	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);

	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	//GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);
	//
	// Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	// Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	// Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	
	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComponent"));
}

void ADPCharacterBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ADPCharacterBase, bIsStunned);

}


void ADPCharacterBase::MulticastHandleDeath_Implementation(const FVector& DeathImpulse)
{
	UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation(), GetActorRotation());

	// Weapon->SetSimulatePhysics(true);
	// Weapon->SetEnableGravity(true);
	// Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	// Weapon->AddImpulse(DeathImpulse * 0.1f, NAME_None, true);
	//
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	GetMesh()->AddImpulse(DeathImpulse, NAME_None, true);
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	bDead = true;
	OnDeathDelegate.Broadcast(this);
}

void ADPCharacterBase::OnRep_Stunned()
{
}

void ADPCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void ADPCharacterBase::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bIsStunned = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bIsStunned ? 0.f : BaseWalkSpeed;
}



void ADPCharacterBase::InitAbilityActorInfo()
{
}

void ADPCharacterBase::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const
{
	check(IsValid(GetAbilitySystemComponent()));
	check(GameplayEffectClass);

	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffectClass, Level, ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
}

void ADPCharacterBase::InitializeDefaultAttributes() const
{
	ApplyEffectToSelf(DefaultPrimaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultSecondaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultVitalAttributes, 1.f);
}

void ADPCharacterBase::AddCharacterAbilities()
{
	UDPAbilitySystemComponent* DPASC = Cast<UDPAbilitySystemComponent>(AbilitySystemComponent);
	if (!HasAuthority()) return;

	DPASC->AddCharacterAbilities(StartupAbilities);
	DPASC->AddCharacterPassiveAbilities(StartupPassiveAbilities);
}

float ADPCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	const float DamageTaken = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	OnDamageDelegate.Broadcast(DamageTaken);
	return DamageTaken;
}

UAbilitySystemComponent* ADPCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAnimMontage* ADPCharacterBase::GetHitReactMontage_Implementation()
{
	return HitReactMontage;
}

// FVector ADPCharacterBase::GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag)
// {
// 	const FDPGameplayTags& GameplayTags = FDPGameplayTags::Get();
// 	
// 	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_LeftHand))
// 	{
// 		return GetMesh()->GetSocketLocation(LeftHandSocketName);
// 	}
// 	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_RightHand))
// 	{
// 		return GetMesh()->GetSocketLocation(RightHandSocketName);
// 	}
//
// 	return FVector();
// }

bool ADPCharacterBase::IsDead_Implementation() const
{
	return bDead;
}

AActor* ADPCharacterBase::GetAvatarActor_Implementation()
{
	return this;
}

TArray<FTaggedMontage> ADPCharacterBase::GetAttackMontages_Implementation()
{
	return AttackMontages;
}

UNiagaraSystem* ADPCharacterBase::GetHitEffect_Implementation()
{
	return HitEffect;
}

FTaggedMontage ADPCharacterBase::GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag)
{
	for (FTaggedMontage TaggedMontage : AttackMontages)
	{
		if (TaggedMontage.MontageTag == MontageTag)
		{
			return TaggedMontage;
		}
	}
	return FTaggedMontage();
}

ECharacterClass ADPCharacterBase::GetCharacterClass_Implementation()
{
	return CharacterClass;
}

FOnASCRegistered& ADPCharacterBase::GetOnASCRegisteredDelegate()
{
	return OnASCRegistered;
}

void ADPCharacterBase::Die(const FVector& DeathImpulse)
{
//	Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	MulticastHandleDeath(DeathImpulse);
}

FOnDeathSignature& ADPCharacterBase::GetOnDeathDelegate()
{
	return OnDeathDelegate;
}

FVector ADPCharacterBase::GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag)
{
	const FDPGameplayTags& GameplayTags = FDPGameplayTags::Get();
	// if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_Weapon) && IsValid(Weapon))
	// {
	// 	return Weapon->GetSocketLocation(WeaponSocketName);
	// }
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_LeftHand))
	{
		return GetMesh()->GetSocketLocation(LeftHandSocketName);
	}
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_RightHand))
	{
		return GetMesh()->GetSocketLocation(RightHandSocketName);
	}
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_Muzzle))
	{
		return GetMesh()->GetSocketLocation(MuzzleSocketName);
	}
	return FVector();
}

FOnDamageSignature& ADPCharacterBase::GetOnDamageDelegate()
{
	return OnDamageDelegate;
}


