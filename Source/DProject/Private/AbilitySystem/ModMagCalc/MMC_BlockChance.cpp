// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ModMagCalc/MMC_BlockChance.h"

#include "AbilitySystem/DPAttributeSet.h"
#include "Interface/CombatInterface.h"

UMMC_BlockChance::UMMC_BlockChance()
{
	DefDef.AttributeToCapture = UDPAttributeSet::GetDefenseAttribute();
	DefDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	DefDef.bSnapshot = false;
	
	LuckDef.AttributeToCapture = UDPAttributeSet::GetLuckAttribute();
	LuckDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	LuckDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(DefDef);
	RelevantAttributesToCapture.Add(LuckDef);
}

float UMMC_BlockChance::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float Def = 0.f;
	GetCapturedAttributeMagnitude(DefDef, Spec, EvaluateParameters, Def);
	Def = FMath::Max<float>(Def, 0.f);

	float Luck = 0.f;
	GetCapturedAttributeMagnitude(LuckDef, Spec, EvaluateParameters, Luck);
	Luck = FMath::Max<float>(Luck, 0.f);
	
	int32 PlayerLevel = 1;
	if (Spec.GetContext().GetSourceObject()->Implements<UCombatInterface>())
	{
		PlayerLevel = ICombatInterface::Execute_GetPlayerLevel(Spec.GetContext().GetSourceObject());
	}
	
	return FMath::Min<float> (20, PlayerLevel * 0.25 + (Luck * Def)/20);
}
