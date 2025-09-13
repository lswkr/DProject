// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DPAbilityTypes.h"
#include "GameFramework/Actor.h"
#include "DPProjectileBase.generated.h"

class UBoxComponent;
class UProjectileMovementComponent;
class UNiagaraSystem;

UENUM(BlueprintType)
enum class EProjectileType : uint8
{
	EOnHit,
	EOnOverlap
};


UCLASS()
class DPROJECT_API ADPProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ADPProjectileBase();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	FDamageEffectParams DamageEffectParams;

	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	EProjectileType ProjectileType = EProjectileType::EOnHit;
	
	UFUNCTION(BlueprintCallable)
	virtual void OnHit();
	virtual void Destroyed() override;
	
	UFUNCTION()
	virtual void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> BoxComponent;

	bool IsValidOverlap(AActor* OtherActor);
	bool bHit = false;

	UPROPERTY()
	TObjectPtr<UAudioComponent> LoopingSoundComponent;

	
private:
	UPROPERTY(EditDefaultsOnly)
	float LifeSpan = 5.f;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> ImpactEffect;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> ImpactSound;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> LoopingSound;
};
