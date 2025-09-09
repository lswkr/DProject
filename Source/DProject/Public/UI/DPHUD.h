// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "DPHUD.generated.h"

/**
 * 
 */

class UAttributeMenuWidgetController;
class UOverlayWidgetController;
class USkillMenuWidgetController;
class APlayerController;
class APlayerState;
class UAbilitySystemComponent;
class UAttributeSet;
class UDPUserWidget;

struct FWidgetControllerParams;

UCLASS()
class DPROJECT_API ADPHUD : public AHUD
{
	GENERATED_BODY()

public:
	UOverlayWidgetController* GetOverlayWidgetController(const FWidgetControllerParams& WCParams);
	UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const FWidgetControllerParams& WCParams);
	USkillMenuWidgetController* GetSkillMenuWidgetController(const FWidgetControllerParams& WCParams);

	void InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS);

private:
	UPROPERTY()
	TObjectPtr<UDPUserWidget> OverlayWidget;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UDPUserWidget> OverlayWidgetClass;

	UPROPERTY()
	TObjectPtr<UOverlayWidgetController> OverlayWidgetController;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;
	
	UPROPERTY()
	UAttributeMenuWidgetController* AttributeMenuWidgetController;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UAttributeMenuWidgetController> AttributeMenuWidgetControllerClass;
	
	UPROPERTY()
	USkillMenuWidgetController* SkillMenuWidgetController;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<USkillMenuWidgetController> SkillMenuWidgetControllerClass;

};
