// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ModMagCalc/MMC_CriticalHitChance.h"

#include "AbilitySystem/DPAttributeSet.h"
#include "Interface/CombatInterface.h"

UMMC_CriticalHitChance::UMMC_CriticalHitChance()
{
	AttDef.AttributeToCapture = UDPAttributeSet::GetAttackAttribute();
	AttDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	AttDef.bSnapshot = false;
	
	LuckDef.AttributeToCapture = UDPAttributeSet::GetLuckAttribute();
	LuckDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	LuckDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(AttDef);
	RelevantAttributesToCapture.Add(LuckDef);
}

float UMMC_CriticalHitChance::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float Att = 0.f;
	GetCapturedAttributeMagnitude(AttDef, Spec, EvaluateParameters, Att);
	Att = FMath::Max<float>(Att, 0.f);

	float Luck = 0.f;
	GetCapturedAttributeMagnitude(LuckDef, Spec, EvaluateParameters, Luck);
	Luck = FMath::Max<float>(Luck, 0.f);
	
	int32 PlayerLevel = 1;
	if (Spec.GetContext().GetSourceObject()->Implements<UCombatInterface>())
	{
		PlayerLevel = ICombatInterface::Execute_GetPlayerLevel(Spec.GetContext().GetSourceObject());
	}
	
	return FMath::Max<float> (10, PlayerLevel * 0.25 + (Luck * Att)/50);
}
