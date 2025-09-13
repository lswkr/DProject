// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/DPDamageGameplayAbility.h"
#include "PointsExplosion.generated.h"

/**
 * 
 */
UCLASS()
class DPROJECT_API UPointsExplosion : public UDPDamageGameplayAbility
{
	GENERATED_BODY()
public:
	virtual FString GetDescription(int32 Level) override;

};
