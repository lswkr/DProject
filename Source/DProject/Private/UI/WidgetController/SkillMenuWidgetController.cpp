// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/SkillMenuWidgetController.h"

#include "DPGameplayTags.h"
#include "AbilitySystem/DPAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/DPPlayerState.h"


void USkillMenuWidgetController::BroadcastInitialValue()
{
	BroadcastAbilityInfo();
	SkillPointsChanged.Broadcast(GetDPPlayerState()->GetSkillPoints());
}

void USkillMenuWidgetController::BindCallbacksToDependencies()
{
	GetDPAbilitySystemComponent()->AbilityStatusChanged.AddLambda([this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 NewLevel)
	{
		if (SelectedAbility.Ability.MatchesTagExact(AbilityTag))
		{
			SelectedAbility.Status = StatusTag;
			bool bEnableSpendPoints = false;
			bool bEnableEquip = false;
			ShouldEnableButtons(StatusTag, CurrentSkillPoints, bEnableSpendPoints, bEnableEquip);
			FString Description;
			GetDPAbilitySystemComponent()->GetDescriptionByAbilityTag(AbilityTag, Description);
			SkillSquareSelectedDelegate.Broadcast(bEnableSpendPoints, bEnableEquip, Description);
		}

		if (AbilityInfo)
		{
			FDPAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
			Info.StatusTag = StatusTag;
			AbilityInfoDelegate.Broadcast(Info);
		}
	});

	GetDPAbilitySystemComponent()->AbilityEquipped.AddUObject(this, &USkillMenuWidgetController::OnAbilityEquipped);

	GetDPPlayerState()->OnSkillPointsChangedDelegate.AddLambda(
		[this](int32 SkillPoints)
		{
			SkillPointsChanged.Broadcast(SkillPoints);
			CurrentSkillPoints = SkillPoints;
			bool bEnableSpendPoints = false;
			bool bEnableEquip = false;
			ShouldEnableButtons(SelectedAbility.Status, CurrentSkillPoints, bEnableSpendPoints, bEnableEquip);
			FString Description;
			GetDPAbilitySystemComponent()->GetDescriptionByAbilityTag(SelectedAbility.Ability, Description);
			SkillSquareSelectedDelegate.Broadcast(bEnableSpendPoints, bEnableEquip,Description);

		});
}

void USkillMenuWidgetController::SkillSquareSelected(const FGameplayTag& AbilityTag)
{
	if (bWaitingForEquipSelection)
	{
		//const FGameplayTag AbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;
		StopWaitingForEquipDelegate.Broadcast();
		bWaitingForEquipSelection = false;
	}

	const FDPGameplayTags GameplayTags = FDPGameplayTags::Get();
	const int32 SkillPoints = GetDPPlayerState()->GetSkillPoints();
	
	FGameplayTag AbilityStatus;
	const bool bTagValid = AbilityTag.IsValid();
	const bool bTagNone = AbilityTag.MatchesTag(GameplayTags.Abilities_None);
	const FGameplayAbilitySpec* AbilitySpec = GetDPAbilitySystemComponent()->GetSpecFromAbilityTag(AbilityTag);
	const bool bSpecValid = AbilitySpec != nullptr;

	if (!bTagValid || bTagNone || !bSpecValid)
	{
		AbilityStatus = GameplayTags.Abilities_Status_Locked;
	}
	else
	{
		AbilityStatus = GetDPAbilitySystemComponent()->GetStatusFromSpec(*AbilitySpec);
	}

	SelectedAbility.Ability = AbilityTag;
	SelectedAbility.Status = AbilityStatus;
	bool bEnableSpendPoints = false;
	bool bEnableEquip = false;
	ShouldEnableButtons(AbilityStatus, SkillPoints, bEnableSpendPoints, bEnableEquip);
	FString Description;
	GetDPAbilitySystemComponent()->GetDescriptionByAbilityTag(AbilityTag, Description);
	SkillSquareSelectedDelegate.Broadcast(bEnableSpendPoints, bEnableEquip, Description);
}

void USkillMenuWidgetController::SpendPointButtonPressed()
{
	if (GetDPAbilitySystemComponent())
	{
		GetDPAbilitySystemComponent()->ServerSpendSpellPoint(SelectedAbility.Ability);
	}
}

void USkillMenuWidgetController::SquareDeselect()
{
	if (bWaitingForEquipSelection)
	{
	//	const FGameplayTag AbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;
		StopWaitingForEquipDelegate.Broadcast();
		bWaitingForEquipSelection = false;
	}

	SelectedAbility.Ability = FDPGameplayTags::Get().Abilities_None;
	SelectedAbility.Status = FDPGameplayTags::Get().Abilities_Status_Locked;

	SkillSquareSelectedDelegate.Broadcast(false,false,FString());
}

void USkillMenuWidgetController::EquipButtonPressed()
{
//	const FGameplayTag AbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;
	WaitForEquipDelegate.Broadcast();
	bWaitingForEquipSelection = true;

	const FGameplayTag SelectedStatus = GetDPAbilitySystemComponent()->GetStatusFromAbilityTag(SelectedAbility.Ability);
	if (SelectedStatus.MatchesTagExact(FDPGameplayTags::Get().Abilities_Status_Equipped))
	{
		SelectedSlot = GetDPAbilitySystemComponent()->GetSlotFromAbilityTag(SelectedAbility.Ability);
	}
}

void USkillMenuWidgetController::SkillRowSquarePressed(const FGameplayTag& SlotTag/*, const FGameplayTag& AbilityType*/)
{
	if (!bWaitingForEquipSelection) return;

	//const FGameplayTag& SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;
//	if (!SelectedAbilityType.MatchesTagExact(AbilityType)) return;//이거 타입이랑 별 상관 없을듯
	
	GetDPAbilitySystemComponent()->ServerEquipAbility(SelectedAbility.Ability, SlotTag);
}

void USkillMenuWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status,
	const FGameplayTag& Slot, const FGameplayTag& PreviousSlot)
{
	bWaitingForEquipSelection = false;

	const FDPGameplayTags GameplayTags = FDPGameplayTags::Get();

	FDPAbilityInfo LastSlotInfo;	//이전에 있던 자리 처리
	LastSlotInfo.StatusTag = GameplayTags.Abilities_Status_Unlocked;
	LastSlotInfo.InputTag = PreviousSlot;
	LastSlotInfo.AbilityTag = GameplayTags.Abilities_None;
	AbilityInfoDelegate.Broadcast(LastSlotInfo);

	FDPAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);	//새 자리 처리
	Info.StatusTag = Status;
	Info.InputTag = Slot;
	AbilityInfoDelegate.Broadcast(Info);
	
//	const FGameplayTag AbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;
	StopWaitingForEquipDelegate.Broadcast();
	SkillSquareReassignedDelegate.Broadcast(AbilityTag);
	SquareDeselect();
}

void USkillMenuWidgetController::ShouldEnableButtons(const FGameplayTag& AbilityStatus, int32 SkillPoints,
	bool& bShouldEnableSkillPointsButton, bool& bShouldEnableEquipButton)
{
	const FDPGameplayTags GameplayTags = FDPGameplayTags::Get();

	bShouldEnableSkillPointsButton = false;
	bShouldEnableEquipButton = false;
	if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Equipped))
	{
		bShouldEnableEquipButton = true;
		if (SkillPoints>0)
		{
			bShouldEnableSkillPointsButton = true;
		}
	}
	else if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Eligible))
	{
		if (SkillPoints>0)
		{
			
			bShouldEnableSkillPointsButton = true;
		}
	}
	else if (AbilityStatus.MatchesTag(GameplayTags.Abilities_Status_Unlocked))
	{
		bShouldEnableEquipButton = true;
		if (SkillPoints>0)
		{
			bShouldEnableSkillPointsButton = true;
		}
	}
}