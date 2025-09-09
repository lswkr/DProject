// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DPProjectileBase.generated.h"

class UBoxComponent;
class UProjectileMovementComponent;
UCLASS()
class DPROJECT_API ADPProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ADPProjectileBase();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;
	
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> BoxComponent;
	
private:
	UPROPERTY(EditDefaultsOnly)
	float LifeSpan = 5.f;

};
