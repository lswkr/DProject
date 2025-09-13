// Fill out your copyright notice in the Description page of Project Settings.


#include "DPGameplayTags.h"

#include "GameplayTagsManager.h"

FDPGameplayTags FDPGameplayTags::GameplayTags;

void FDPGameplayTags::InitializeNativeGameplayTags()
{
	
	/* Player Tags */
	GameplayTags.Player_Block_InputPressed = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Player.Block.InputPressed"),
		FString("Block Input Pressed Callback for Input")
		);
	
	GameplayTags.Player_Block_InputReleased = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Player.Block.InputReleased"),
		FString("Block Input Released Callback for Input")
		);
	
	GameplayTags.Player_Block_InputHeld = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Player.Block.InputHeld"),
		FString("Block Input Held Callback for Input")
		);

	GameplayTags.Player_Block_CursorTrace = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Player.Block.CursorTrace"),
		FString("Block CursorTrace Callback for Input")
		);

	/* Input Tags */
	GameplayTags.InputTag_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.1"),
		FString("Input Tag for 1")
		);
	
	GameplayTags.InputTag_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.2"),
		FString("Input Tag for 2")
		);
	
	GameplayTags.InputTag_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.3"),
		FString("Input Tag for 3")
		);
	
	GameplayTags.InputTag_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.4"),
		FString("Input Tag for 4")
		);
	
	GameplayTags.InputTag_LMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.LMB"),
		FString("Input Tag for LMB")
		);
	
	GameplayTags.InputTag_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.RMB"),
		FString("Input Tag for RMB")
		);

	/* EventTags */
		
	GameplayTags.Event_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Event.HitReact"),
		FString("Send event to self if Hits enemy")
		);

	GameplayTags.Event_Montage_PointsExplosion = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Event.Montage.PointsExplosion"),
		FString("Montage of PointsExplosion")
		);
	
	/* Primary Attributes */
	GameplayTags.Attributes_Primary_Strength = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Strength"),
		FString("Increase MaxHP and Attack Power")
		);
	
	GameplayTags.Attributes_Primary_Intelligence = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Intelligence"),
		FString("Increase MaxMP and Magic Power")
		);
	
	GameplayTags.Attributes_Primary_Luck = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Luck"),
		FString("Increase Critical Hit Chance and Block Chance")
		);

	GameplayTags.Attributes_Primary_Will = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Will"),
		FString("Increase Defense Power and Amount of Health/Mana Regenerated")
		);

	
	/* Secondary Attributes */
	GameplayTags.Attributes_Secondary_Attack = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Secondary.Attack"),
	FString("Attack Power")
	);
	
	GameplayTags.Attributes_Secondary_Defense = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Secondary.Defense"),
	FString("Defense Power")
	);

	GameplayTags.Attributes_Secondary_CriticalHitChance = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Secondary.CriticalHitChance"),
	FString("Chance to Increase Damage")
	);

	GameplayTags.Attributes_Secondary_BlockChance = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Secondary.BlockChance"),
	FString("Chance to Block")
	);

	GameplayTags.Attributes_Secondary_HealthRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Secondary.HealthRegeneration"),
	FString("Amount of Health Regenerated Every 1 Second")
	);

	GameplayTags.Attributes_Secondary_ManaRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Secondary.ManaRegeneration"),
	FString("Amount of Mana Regenerated Every 1 Second")
	);

	
	/* Vital Attributes */
	GameplayTags.Attributes_Vital_MaxHealth = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Vital.MaxHealth"),
	FString("Maximum Amount of Health Obtainable")
	);

	GameplayTags.Attributes_Vital_MaxMana = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Vital.MaxMana"),
	FString("Maximum Amount of Mana Obtainable")
	);

	GameplayTags.Attributes_Vital_Health = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Vital.Health"),
	FString("Amount of Current Health")
	);

	GameplayTags.Attributes_Vital_Mana = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Vital.Mana"),
	FString("Amount of Current Mana")
	);

	/* Meta Attributes */
		GameplayTags.Attributes_Meta_IncomingXP = UGameplayTagsManager::Get().AddNativeGameplayTag(
    	FName("Attributes.Meta.IncomingXP"),
    	FString("IncomingXP Meta Attributes")
    	);
	/* Effects */
	GameplayTags.Effects_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Effects.HitReact"),
	FString("Tag Granted when Hit Reacting")
	);

	/* Abilities */
	GameplayTags.Abilities_None = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Abilities.None"),
			FString("No Ability - like the nullptr for Ability Tags")
			);


	GameplayTags.Abilities_Attack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Attack"),
		FString("Attack Ability Tag")
		);

	GameplayTags.Abilities_Attack_Dash = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Attack.Dash"),
		FString("Dash Attack")
		);
	
	GameplayTags.Abilities_Attack_PointsExplosion = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Abilities.Attack.PointsExplosion"),
	FString("Points Explosion")
	);

	GameplayTags.Abilities_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.HitReact"),
		FString("Hit React Ability Tag")
		);

	GameplayTags.Abilities_Passive_ListenForEvent = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Passive.ListenForEvent"),
		FString("Listen For Event")
		);
	

	/* Abilities|Name */
	GameplayTags.Abilities_Attack_DefaultAttack = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Abilities.Attack.DefaultAttack"),
			FString("DefaultAttack Ability Tag")
			);
	
	/* Abilities|Status */

	GameplayTags.Abilities_Status_Eligible = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Abilities.Status.Eligible"),
	FString("Eligible Status")
	);
	
	GameplayTags.Abilities_Status_Equipped = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Abilities.Status.Equipped"),
	FString("Equipped Status")
	);

	GameplayTags.Abilities_Status_Locked = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Abilities.Status.Locked"),
	FString("Locked Status")
	);
	
	GameplayTags.Abilities_Status_Unlocked = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Abilities.Status.Unlocked"),
	FString("Unlocked Status")
	);

	/* DamageType */
	GameplayTags.Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Damage"),
	FString("Damage")
	);

	GameplayTags.Damage_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Damage.Physical"),
			FString("Physical Damage Type")
			);

	GameplayTags.Damage_Slash = UGameplayTagsManager::Get().AddNativeGameplayTag(
		   FName("Damage.Slash"),
		   FString("Slash Damage Type")
		   );

	GameplayTags.Damage_Slash = UGameplayTagsManager::Get().AddNativeGameplayTag(
		   FName("Damage.Slash"),
		   FString("Slash Damage Type")
		   );

	
	GameplayTags.CombatSocket_RightHand = UGameplayTagsManager::Get().AddNativeGameplayTag(
			   FName("CombatSocket.RightHand"),
			   FString("Right Hand Socket")
			   );

	GameplayTags.CombatSocket_LeftHand = UGameplayTagsManager::Get().AddNativeGameplayTag(
			   FName("CombatSocket.LeftHand"),
			   FString("Left Hand Socket")
			   );

	GameplayTags.CombatSocket_Muzzle = UGameplayTagsManager::Get().AddNativeGameplayTag(
			   FName("CombatSocket.Muzzle"),
			   FString("Muzzle Socket")
			   );
	
	GameplayTags.Debuff_Stun = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Debuff.Stun"),
	FString("Debuff for Physical Damage")
	);
	GameplayTags.Debuff_Bleeding = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Debuff.Bleeding"),
	FString("Debuff for Slash Damage")
	);
	GameplayTags.Debuff_Chance = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Debuff.Chance"),
	FString("Debuff Chance")
	);
	GameplayTags.Debuff_Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Debuff.Damage"),
			FString("Debuff Damage")
		);

	GameplayTags.Debuff_Duration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Duration"),
		FString("Debuff Duration")
	);

	GameplayTags.Debuff_Frequency = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Frequency"),
		FString("Debuff Frequency")
	);
	
	/* Montage Tag */
	GameplayTags.Montage_Attack_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Attack.1"),
		FString("Montage Attack 1")
	);
	
	GameplayTags.Montage_Attack_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Attack.2"),
		FString("Montage Attack 2")
	);
	
	GameplayTags.Montage_Attack_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Attack.3"),
		FString("Montage Attack 3")
	);
	
	GameplayTags.Montage_Attack_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Attack.4"),
		FString("Montage Attack 4")
	);

	
	GameplayTags.SkillBaseDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillBaseDamage"),
		FString("BaseDamage for each skill")
	);

	
	/* Map of Damage Types to Debuffs */
	GameplayTags.DamageTypesToDebuffs.Add(GameplayTags.Damage_Physical, GameplayTags.Debuff_Stun);
	GameplayTags.DamageTypesToDebuffs.Add(GameplayTags.Damage_Slash, GameplayTags.Debuff_Bleeding);

}
