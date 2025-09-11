// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ModMagCalc/MMC_HealthRegeneration.h"

#include "AbilitySystem/DPAttributeSet.h"
#include "Interface/CombatInterface.h"

UMMC_HealthRegeneration::UMMC_HealthRegeneration()
{
	MaxHealthDef.AttributeToCapture = UDPAttributeSet::GetMaxHealthAttribute();
	MaxHealthDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	MaxHealthDef.bSnapshot = false;
	
	WillDef.AttributeToCapture = UDPAttributeSet::GetWillAttribute();
	WillDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	WillDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(MaxHealthDef);
	RelevantAttributesToCapture.Add(WillDef);
}

float UMMC_HealthRegeneration::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float MaxHealth = 0.f;
	GetCapturedAttributeMagnitude(MaxHealthDef, Spec, EvaluateParameters, MaxHealth);
	MaxHealth = FMath::Max<float>(MaxHealth, 0.f);

	float Will = 0.f;
	GetCapturedAttributeMagnitude(WillDef, Spec, EvaluateParameters, Will);
	Will = FMath::Max<float>(Will, 0.f);
	
	int32 PlayerLevel = 1;
	if (Spec.GetContext().GetSourceObject()->Implements<UCombatInterface>())
	{
		PlayerLevel = ICombatInterface::Execute_GetPlayerLevel(Spec.GetContext().GetSourceObject());
	}
	
	return FMath::Min<float>(MaxHealth/10, PlayerLevel + (Will * MaxHealth)/75);
}
