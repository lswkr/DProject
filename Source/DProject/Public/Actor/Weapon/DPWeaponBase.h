// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DPAbilityTypes.h"
#include "GameFramework/Actor.h"
#include "DPWeaponBase.generated.h"

class UBoxComponent;
class UNiagaraSystem;

UCLASS()
class DPROJECT_API ADPWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ADPWeaponBase();

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	FDamageEffectParams DamageEffectParams;

	UFUNCTION()
	UBoxComponent* GetWeaponBoxComponent();
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> BoxComponent;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;
	
	UFUNCTION()
	virtual void OnBoxComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	// UPROPERTY(EditDefaultsOnly)
	// FScalableFloat AttackPowerAtLevel;

	UPROPERTY(EditDefaultsOnly)	
	float Level = 1.f;

	bool IsValidOverlap(AActor* OverlappedActor);

	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> ImpactEffect;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> ImpactSound;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> DamageGameplayEffectClass = nullptr;

	UPROPERTY(EditDefaultsOnly)
	float BaseDamage = 0.f;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag DamageType = FGameplayTag();

	UPROPERTY(EditDefaultsOnly)
	float WeaponLevel = 1.f;
	
	UPROPERTY(EditDefaultsOnly)
	float DebuffChance = 0.f;

	UPROPERTY(EditDefaultsOnly)
	float DebuffDamage = 0.f;
	
	UPROPERTY(EditDefaultsOnly)
	float DebuffDuration = 0.f;
	
	UPROPERTY(EditDefaultsOnly)
	float DebuffFrequency = 0.f;

	UPROPERTY(EditDefaultsOnly)
	float DeathImpulseMagnitude = 1000.f;
	
	/* USTRUCT(BlueprintType)
struct FDamageEffectParams
{
	GENERATED_BODY()

	FDamageEffectParams() {}

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UObject> WorldContextObject = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> SourceAbilitySystemComponent;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> TargetAbilitySystemComponent;

	UPROPERTY(BlueprintReadWrite)
	float DeathImpulseMagnitude = 0.f;

	UPROPERTY(BlueprintReadWrite)
	FVector DeathImpulse = FVector::ZeroVector;

};*/

};
