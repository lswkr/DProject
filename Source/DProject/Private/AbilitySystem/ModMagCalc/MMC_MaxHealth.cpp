// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ModMagCalc/MMC_MaxHealth.h"

#include "AbilitySystem/DPAttributeSet.h"
#include "Interface/CombatInterface.h"

UMMC_MaxHealth::UMMC_MaxHealth()
{
	StrDef.AttributeToCapture = UDPAttributeSet::GetStrengthAttribute();
	StrDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	StrDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(StrDef);
}

float UMMC_MaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float Str = 0.f;
	GetCapturedAttributeMagnitude(StrDef, Spec, EvaluateParameters, Str);
	Str = FMath::Max<float>(Str, 0.f);

	int32 PlayerLevel = 1;
	if (Spec.GetContext().GetSourceObject()->Implements<UCombatInterface>())
	{
		PlayerLevel = ICombatInterface::Execute_GetPlayerLevel(Spec.GetContext().GetSourceObject());
	}
	
	return 80.f + 2.5f * Str + 10.f * PlayerLevel;
}
