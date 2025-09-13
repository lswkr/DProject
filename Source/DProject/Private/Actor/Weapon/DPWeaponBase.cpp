// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Weapon/DPWeaponBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "DPGameplayTags.h"
#include "SourceControlAssetDataCache.h"
#include "AbilitySystem/DPAbilitySystemLibrary.h"
#include "Character/DPCharacterBase.h"
#include "Components/BoxComponent.h"

class ADPCharacterBase;
class UAbilitySystemInterface;

ADPWeaponBase::ADPWeaponBase()
{
 	PrimaryActorTick.bCanEverTick = false;
	
	BoxComponent = CreateDefaultSubobject<UBoxComponent>("BoxComponent");
	SetRootComponent(BoxComponent);
	//BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);

	BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	BoxComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	BoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMeshComponent");
	SkeletalMeshComponent->SetupAttachment(GetRootComponent());
	SkeletalMeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
}

UBoxComponent* ADPWeaponBase::GetWeaponBoxComponent()
{
	return BoxComponent;
}

// Called when the game starts or when spawned
void ADPWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	SetReplicateMovement(true);
	
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnBoxComponentOverlap);
}

void ADPWeaponBase::OnBoxComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	UE_LOG(LogTemp, Warning, TEXT("%s detected"), *OtherActor->GetName());
	// if (!IsValidOverlap(OtherActor)) return;
	
	
	
	if (HasAuthority())
	{
		
		// if (OtherActor->Implements<UAbilitySystemInterface>())
		// {
		// 	FGameplayEventData Data;
		// 	Data.Instigator = this;
		// 	Data.Target = OtherActor;
		//
		// 	if (UDPAbilitySystemLibrary::IsHostile(this, OtherActor))
		// 	{
		// 		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		// 			this,
		// 			FDPGameplayTags::Get().Event_HitReact,
		// 			Data
		// 		);
		// 	}
		// }
		if (UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
		{
			if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
			{
				if (UDPAbilitySystemLibrary::IsHostile(GetOwner(), OtherActor))
				{
					const FVector DeathImpulse = GetOwner()->GetActorForwardVector() * DamageEffectParams.DeathImpulseMagnitude;
					DamageEffectParams.WorldContextObject = this;
					DamageEffectParams.DamageGameplayEffectClass = DamageGameplayEffectClass;
					DamageEffectParams.SourceAbilitySystemComponent = SourceASC;
					DamageEffectParams.TargetAbilitySystemComponent = TargetASC;
					DamageEffectParams.BaseDamage = BaseDamage;
					DamageEffectParams.AbilityLevel = WeaponLevel;
					DamageEffectParams.DamageType = DamageType;
					DamageEffectParams.DebuffChance = DebuffChance;
					DamageEffectParams.DebuffDuration = DebuffDuration;
					DamageEffectParams.DebuffFrequency = DebuffFrequency;
					DamageEffectParams.DebuffDamage = DebuffDamage;
					DamageEffectParams.DeathImpulseMagnitude = DeathImpulseMagnitude;
					DamageEffectParams.DeathImpulse = (OtherActor->GetActorLocation()-this->GetActorLocation());
					UE_LOG(LogTemp, Warning, TEXT("OtherActor Success: %s (ADPWeaponBase::line 47)"), *OtherActor->GetName());	
					UDPAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
				}
			}
		}
		
	}
}

bool ADPWeaponBase::IsValidOverlap(AActor* OverlappedActor)
{
	UE_LOG(LogTemp, Warning, TEXT("IsValidOverlap:  (ADPWeaponBase::line 60)"))

	ADPCharacterBase* SourceAvatarActor =Cast<ADPCharacterBase>(GetOwner());
	
	if (!SourceAvatarActor->Implements<UAbilitySystemInterface>())
	{
		return false;
	}

	DamageEffectParams.SourceAbilitySystemComponent = SourceAvatarActor->GetAbilitySystemComponent();
	
	if (SourceAvatarActor == OverlappedActor) {return false;};
	if (!UDPAbilitySystemLibrary::IsHostile(SourceAvatarActor, OverlappedActor)){ UE_LOG(LogTemp, Warning, TEXT("not hostile: %s (ADPWeaponBase::line 60)"), *OverlappedActor->GetName());
		return false;}

	return true;
}

