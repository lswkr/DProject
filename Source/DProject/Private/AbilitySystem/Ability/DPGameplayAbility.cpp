// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Ability/DPGameplayAbility.h"

#include "AbilitySystem/DPAttributeSet.h"
#include "Character/DPCharacterBase.h"

FString UDPGameplayAbility::GetDescription(int32 Level)
{
	return FString::Printf(TEXT("GetDescription"));
}

FString UDPGameplayAbility::GetLockedDescription(int32 Level)
{
	return FString::Printf(TEXT("GetLockedDescription"));
}

ADPCharacterBase* UDPGameplayAbility::GetAvatarCharacterFromActorInfo()
{
	if (!CachedAvatarCharacter.IsValid())
	{
		CachedAvatarCharacter = Cast<ADPCharacterBase> (CurrentActorInfo->AvatarActor);
	}
	return CachedAvatarCharacter.IsValid() ? CachedAvatarCharacter.Get():nullptr;
}

float UDPGameplayAbility::GetManaCost(float InLevel) const
{
	float ManaCost = 0.f;
	if (const UGameplayEffect* CostEffect = GetCostGameplayEffect())
	{
		for (FGameplayModifierInfo Mod : CostEffect->Modifiers)
		{
			if (Mod.Attribute == UDPAttributeSet::GetManaAttribute())
			{
				Mod.ModifierMagnitude.GetStaticMagnitudeIfPossible(InLevel, ManaCost);
			}
		}
	}
	return ManaCost;
}

float UDPGameplayAbility::GetCooldown(float InLevel) const
{
	float Cooldown = 0.f;
	if (const UGameplayEffect* CooldownEffect = GetCooldownGameplayEffect())
	{
		CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(InLevel, Cooldown);
	}
	return Cooldown;
}