// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "DPGameplayAbility.generated.h"

/**
 * 
 */

class ADPCharacterBase;

UCLASS()
class DPROJECT_API UDPGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	FGameplayTag StartupInputTag;

	virtual FString GetDescription(int32 Level);
	static FString GetLockedDescription(int32 Level);

	UFUNCTION(BlueprintPure, Category = "Ability")
	ADPCharacterBase* GetAvatarCharacterFromActorInfo();
	
protected:
	float GetManaCost(float InLevel = 1.f) const;
	float GetCooldown(float InLevel = 1.f) const;

private:
	TWeakObjectPtr<ADPCharacterBase> CachedAvatarCharacter;
};
