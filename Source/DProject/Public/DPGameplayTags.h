// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"


struct FDPGameplayTags
{
public:
	static const FDPGameplayTags& Get() { return GameplayTags; }
	static void InitializeNativeGameplayTags();

	/* Player Tags */
	FGameplayTag Player_Block_InputPressed;
	FGameplayTag Player_Block_InputReleased;
	FGameplayTag Player_Block_InputHeld;
	FGameplayTag Player_Block_CursorTrace;

	/* Input Tags */
	FGameplayTag InputTag_1;
	FGameplayTag InputTag_2;
	FGameplayTag InputTag_3;
	FGameplayTag InputTag_4;
	FGameplayTag InputTag_LMB;
	FGameplayTag InputTag_RMB;

	/* Event Tags */
	FGameplayTag Event_HitReact;
	
	/* Primary Attributes */
	FGameplayTag Attributes_Primary_Strength;
	FGameplayTag Attributes_Primary_Intelligence;
	FGameplayTag Attributes_Primary_Luck;
	FGameplayTag Attributes_Primary_Will;
	
	/* Secondary Attributes */
	FGameplayTag Attributes_Secondary_Attack;
	FGameplayTag Attributes_Secondary_Defense;
	FGameplayTag Attributes_Secondary_CriticalHitChance;
	FGameplayTag Attributes_Secondary_BlockChance;
	FGameplayTag Attributes_Secondary_HealthRegeneration;
	FGameplayTag Attributes_Secondary_ManaRegeneration;
	
	/* Vital Attributes */
	FGameplayTag Attributes_Vital_MaxHealth;
	FGameplayTag Attributes_Vital_MaxMana;
	FGameplayTag Attributes_Vital_Health;
	FGameplayTag Attributes_Vital_Mana;

	/* Meta Attributes */
	FGameplayTag Attributes_Meta_IncomingXP;
	
	/* Effects */
	FGameplayTag Effects_HitReact;

	/* Socket */
	FGameplayTag CombatSocket_Weapon;
	FGameplayTag CombatSocket_RightHand;
	FGameplayTag CombatSocket_LeftHand;
	FGameplayTag CombatSocket_Muzzle;
	
	/* Abilities */
	FGameplayTag Abilities_None;
	FGameplayTag Abilities_Attack;
	FGameplayTag Abilities_HitReact;
	
	FGameplayTag Abilities_Attack_DefaultAttack;
	FGameplayTag Abilities_Attack_Dash;

	FGameplayTag Abilities_Passive_ListenForEvent;
	
	FGameplayTag Debuff_Bleeding;
	FGameplayTag Debuff_Stun;

	/* Status */
	FGameplayTag Abilities_Status_Eligible;
	FGameplayTag Abilities_Status_Equipped;
	FGameplayTag Abilities_Status_Locked;
	FGameplayTag Abilities_Status_Unlocked;
	FGameplayTag Abilities_Status_PreQualified;

	/* DamageType */
	FGameplayTag Damage;
	FGameplayTag Damage_Physical;
	FGameplayTag Damage_Slash;


	
	FGameplayTag Debuff_Chance;
	FGameplayTag Debuff_Damage;
	FGameplayTag Debuff_Duration;
	FGameplayTag Debuff_Frequency;

	/* Montage Tag */
	FGameplayTag Montage_Attack_1;
	FGameplayTag Montage_Attack_2;
	FGameplayTag Montage_Attack_3;
	FGameplayTag Montage_Attack_4;
	
	FGameplayTag SkillBaseDamage;
	
	TMap<FGameplayTag, FGameplayTag> DamageTypesToDebuffs;

private:
	static FDPGameplayTags GameplayTags;
};
