// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ModMagCalc/MMC_ManaRegeneration.h"

#include "AbilitySystem/DPAttributeSet.h"
#include "Interface/CombatInterface.h"

UMMC_ManaRegeneration::UMMC_ManaRegeneration()
{
	MaxManaDef.AttributeToCapture = UDPAttributeSet::GetMaxManaAttribute();
	MaxManaDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	MaxManaDef.bSnapshot = false;
	
	WillDef.AttributeToCapture = UDPAttributeSet::GetWillAttribute();
	WillDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	WillDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(MaxManaDef);
	RelevantAttributesToCapture.Add(WillDef);
}

float UMMC_ManaRegeneration::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float MaxMana = 0.f;
	GetCapturedAttributeMagnitude(MaxManaDef, Spec, EvaluateParameters, MaxMana);
	MaxMana = FMath::Max<float>(MaxMana, 0.f);

	float Will = 0.f;
	GetCapturedAttributeMagnitude(WillDef, Spec, EvaluateParameters, Will);
	Will = FMath::Max<float>(Will, 0.f);
	
	int32 PlayerLevel = 1;
	if (Spec.GetContext().GetSourceObject()->Implements<UCombatInterface>())
	{
		PlayerLevel = ICombatInterface::Execute_GetPlayerLevel(Spec.GetContext().GetSourceObject());
	}
	
	return FMath::Min<float>(MaxMana/10, PlayerLevel + (Will * MaxMana)/100);
}
