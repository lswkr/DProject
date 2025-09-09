// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/DPAbilitySystemGlobals.h"

#include "DPAbilityTypes.h"

FGameplayEffectContext* UDPAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FDPGameplayEffectContext();
}
