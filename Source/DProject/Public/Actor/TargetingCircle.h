// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TargetingCircle.generated.h"

UCLASS()
class DPROJECT_API ATargetingCircle : public AActor
{
	GENERATED_BODY()
	
public:	
	ATargetingCircle();
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UDecalComponent> TargetingCircleDecal;
	
protected:
	virtual void BeginPlay() override;



};
