// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_BlockChance.generated.h"

/**
 * 
 */
UCLASS()
class DPROJECT_API UMMC_BlockChance : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

	
public:
	UMMC_BlockChance();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
	FGameplayEffectAttributeCaptureDefinition DefDef;
	FGameplayEffectAttributeCaptureDefinition LuckDef;
};
