// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "DPGameplayTags.h"
#include "AbilitySystem/DPAbilitySystemLibrary.h"
#include "AbilitySystem/DPAttributeSet.h"
#include "Interface/CombatInterface.h"


struct FDPGameplayTags;

struct DPDamageStatics
{
	/* Source */
	DECLARE_ATTRIBUTE_CAPTUREDEF(Attack);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	
	/* Target */
	DECLARE_ATTRIBUTE_CAPTUREDEF(Defense);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	
	DPDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDPAttributeSet, Attack, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDPAttributeSet, CriticalHitChance, Source, false);
	
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDPAttributeSet, Defense, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDPAttributeSet, BlockChance, Target, false);
	}
};

static const DPDamageStatics& DamageStatics()
{
	static DPDamageStatics DStatics;
	return DStatics;
}


UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().AttackDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
	
	RelevantAttributesToCapture.Add(DamageStatics().DefenseDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
}

void UExecCalc_Damage::DetermineDebuff(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	const FGameplayEffectSpec& Spec, FAggregatorEvaluateParameters EvaluationParameters) const
{
	const FDPGameplayTags& GameplayTags = FDPGameplayTags::Get();

	for (TTuple<FGameplayTag, FGameplayTag> Pair : GameplayTags.DamageTypesToDebuffs)
	{
		const FGameplayTag& DamageType = Pair.Key;
		const FGameplayTag& DebuffType = Pair.Value;
		const float TypeDamage = Spec.GetSetByCallerMagnitude(DamageType, false, -1.f);

		if (TypeDamage > -.5f) //약간의 오차허용
		{
			//디버프 확률
			const float SourceDebuffChance = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Chance, false, -1.f);

			const bool bDebuff = FMath::RandRange(1,99) < SourceDebuffChance;

			if (bDebuff)
			{
				FGameplayEffectContextHandle ContextHandle = Spec.GetContext();

				UDPAbilitySystemLibrary::SetIsSuccessfulDebuff(ContextHandle, true);
				UDPAbilitySystemLibrary::SetDamageType(ContextHandle, DamageType);

				const float DebuffDamage = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Damage, false, -1.f);
				const float DebuffDuration = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Duration, false, -1.f);
				const float DebuffFrequency = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Frequency, false, -1.f);

				UDPAbilitySystemLibrary::SetDebuffDamage(ContextHandle, DebuffDamage);
				UDPAbilitySystemLibrary::SetDebuffDuration(ContextHandle, DebuffDuration);
				UDPAbilitySystemLibrary::SetDebuffFrequency(ContextHandle, DebuffFrequency);
			}
			
		}
		
	}
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDefs;
	const FDPGameplayTags& Tags = FDPGameplayTags::Get();

	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_Attack, DamageStatics().AttackDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitChance, DamageStatics().CriticalHitChanceDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_Defense, DamageStatics().DefenseDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_BlockChance, DamageStatics().BlockChanceDef);
	
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	//레벨 가져오기
	int32 SourcePlayerLevel = 1;
	if (SourceAvatar->Implements<UCombatInterface>())
	{
		SourcePlayerLevel = ICombatInterface::Execute_GetPlayerLevel(SourceAvatar);
	}
	
	int32 TargetPlayerLevel = 1;
	if (TargetAvatar->Implements<UCombatInterface>())
	{
		TargetPlayerLevel = ICombatInterface::Execute_GetPlayerLevel(TargetAvatar);
	}

	
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	//디버프
	DetermineDebuff(ExecutionParams, Spec, EvaluationParameters);


	//Set by Caller Magnitude에서 Damage얻기
	float Damage = 0.f;
	
	for (const TTuple<FGameplayTag, FGameplayTag>& Pair  : Tags.DamageTypesToDebuffs)
	{
		const FGameplayTag DamageTypeTag = Pair.Key;

		float DamageTypeValue = Spec.GetSetByCallerMagnitude(DamageTypeTag, false);

		if (DamageTypeValue<=0.f)
		{
			continue;
		}
		Damage += DamageTypeValue;
	}

	//CriticalChance
	float SourceCriticalHitChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitChanceDef, EvaluationParameters, SourceCriticalHitChance);
	
	float SourceAttack = 0.f;
	float TargetDefense = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().AttackDef, EvaluationParameters, SourceAttack);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DefenseDef, EvaluationParameters, TargetDefense);
	
	SourceCriticalHitChance = FMath::Max<float>(SourceCriticalHitChance, 0.f);
	
	// Critical Hit Resistance reduces Critical Hit Chance by a certain percentage
	const bool bCriticalHit = FMath::RandRange(1, 100) < SourceCriticalHitChance;

	UDPAbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle, bCriticalHit);

	// Double damage plus a bonus if critical hit
	Damage = bCriticalHit ? 2.f * Damage + 5 * FMath::Max<float>(0.f, SourceAttack-TargetDefense): Damage;

	//Block일 경우 0.f
	float TargetBlockChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluationParameters, TargetBlockChance);
	TargetBlockChance = FMath::Max<float>(TargetBlockChance, 0.f);

	const bool bBlocked = FMath::RandRange(1,99) < TargetBlockChance;
	UDPAbilitySystemLibrary::SetIsBlockedHit(EffectContextHandle, bBlocked);
	Damage = bBlocked ? 0.f : Damage;

	//공격력 방어력 기반 최종 데미지 계산
	
	if (SourceAttack>TargetDefense)
	{
		Damage *= FMath::FRandRange(0.f, 0.5f);
	}

	const FGameplayModifierEvaluatedData EvaluatedData(UDPAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
