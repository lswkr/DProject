// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AbilitySystem/DPAbilitySystemComponent.h"
#include "AbilitySystem/DPAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "Player/DPPlayerState.h"


void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	UDPAttributeSet* DPAS = CastChecked<UDPAttributeSet>(AttributeSet);

	for (TPair<FGameplayTag, FGameplayAttribute(*)()>& Pair : DPAS->TagsToAttributes)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
			[this, Pair](const FOnAttributeChangeData& Data)
			{
				BroadcastAttributeInfo(Pair.Key, Pair.Value());	
			}
			);
	}

	GetDPPlayerState()->OnAttributePointsChangedDelegate.AddLambda(
		[this](int32 Points)
		{
			AttributePointsChangedDelegate.Broadcast(Points);
		}
		);
	
}

void UAttributeMenuWidgetController::BroadcastInitialValue()
{
	UDPAttributeSet* DPAS = CastChecked<UDPAttributeSet>(AttributeSet);
	check(AttributeInfo);

	for (TPair<FGameplayTag, FGameplayAttribute(*)()>& Pair : DPAS->TagsToAttributes)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value());
	}
	AttributePointsChangedDelegate.Broadcast(GetDPPlayerState()->GetAttributePoints());
}

void UAttributeMenuWidgetController::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	UDPAbilitySystemComponent* DPASC = CastChecked<UDPAbilitySystemComponent>(AbilitySystemComponent);
	DPASC->UpgradeAttribute(AttributeTag);
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag,
	const FGameplayAttribute& Attribute) const
{
	FDPAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag);
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}
