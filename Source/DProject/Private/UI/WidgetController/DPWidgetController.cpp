// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/DPWidgetController.h"


#include "AbilitySystem/DPAbilitySystemComponent.h"
#include "AbilitySystem/DPAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/DPPlayerController.h"
#include "Player/DPPlayerState.h"


void UDPWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}

void UDPWidgetController::BroadcastInitialValue()
{
}

void UDPWidgetController::BindCallbacksToDependencies()
{
}

void UDPWidgetController::BroadcastAbilityInfo()
{
	if (!GetDPAbilitySystemComponent()->bStartupAbilitiesGiven) return; //StartupAbility없으면 return;
	UE_LOG(LogTemp, Log, TEXT("bStartupAbilitiesGiven == true"));
	FForEachAbility BroadcastDelegate;//람다 함수는 UDPAbilitySystemComponent::ForEachAbility에서 Activatable Abilities들 Execute된다.
	BroadcastDelegate.BindLambda([this](const FGameplayAbilitySpec& AbilitySpec)
	{
		FDPAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(DPAbilitySystemComponent->GetAbilityTagFromSpec(AbilitySpec));
		Info.InputTag = DPAbilitySystemComponent->GetInputTagFromSpec(AbilitySpec);
		Info.StatusTag = DPAbilitySystemComponent->GetStatusFromSpec(AbilitySpec);
		AbilityInfoDelegate.Broadcast(Info);
	}
	);
	GetDPAbilitySystemComponent()->ForEachAbility(BroadcastDelegate);
	
}

ADPPlayerController* UDPWidgetController::GetDPPlayerController()
{
	if (DPPlayerController == nullptr)
	{
		DPPlayerController = Cast<ADPPlayerController>(PlayerController);
	}
	return DPPlayerController;
}

ADPPlayerState* UDPWidgetController::GetDPPlayerState()
{
	if (DPPlayerState == nullptr)
	{
		DPPlayerState = Cast<ADPPlayerState>(PlayerState);
	}
	return DPPlayerState;
}

UDPAbilitySystemComponent* UDPWidgetController::GetDPAbilitySystemComponent()
{
	if (DPAbilitySystemComponent == nullptr)
	{
		DPAbilitySystemComponent = Cast<UDPAbilitySystemComponent>(AbilitySystemComponent);
	}
	return DPAbilitySystemComponent;
}

UDPAttributeSet* UDPWidgetController::GetDPAttributeSet()
{
	if (DPAttributeSet == nullptr)
	{
		DPAttributeSet = Cast<UDPAttributeSet>(AttributeSet);
	}
	return DPAttributeSet;
}
