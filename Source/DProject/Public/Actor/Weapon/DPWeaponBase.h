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

};
