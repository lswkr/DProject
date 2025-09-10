// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "DPPlayerController.generated.h"


/**
 * 
 */

class UInputAction;
class UInputMappingContext;
class USplineComponent;
class UDPInputConfig;
class IHighlightInterface;
class UNiagaraSystem;
class UDPAbilitySystemComponent;

struct FInputActionValue;


enum class ETargetingStatus : uint8
{
	Targeting,
	NotTargeting
};


UCLASS()
class DPROJECT_API ADPPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ADPPlayerController();
	virtual void PlayerTick(float DeltaTime) override;

	// UFUNCTION(Client, Reliable)
	// void ShowDamageNumber(float DamageAmount, ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit);

	// UFUNCTION(BlueprintCallable)
	// void ShowMagicCircle(UMaterialInterface* DecalMaterial = nullptr);
	//
	// UFUNCTION(BlueprintCallable)
	// void HideMagicCircle();

	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	
private:
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputMappingContext> DPContext;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> MoveAction;
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> ClickAction;
	

	void Input_Move(const FInputActionValue& InputActionValue);

	void InputPressed_RMB(const FInputActionValue& InputActionValue);
	void InputHeld_RMB(const FInputActionValue& InputActionValue);
	void InputReleased_RMB(const FInputActionValue& InputActionValue);
	
	void CursorTrace();

	TObjectPtr<AActor> LastActor;
	TObjectPtr<AActor> ThisActor;

	FHitResult CursorHit;
	static void HighlightActor(AActor* InActor);
	static void UnhighlightActor(AActor* InActor);

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UDPInputConfig> InputConfig;
	
	UPROPERTY()
	TObjectPtr<UDPAbilitySystemComponent> DPAbilitySystemComponent;
	
	UDPAbilitySystemComponent* GetASC();
	
	FVector CachedDestination = FVector::ZeroVector;
	float FollowTime = 0.f;
	bool bAutoRunning = false;
	ETargetingStatus TargetingStatus = ETargetingStatus::NotTargeting;
	UPROPERTY(EditDefaultsOnly)
	float ShortPressThreshold = 0.5f;
	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius = 50.f;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> Spline;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraSystem> ClickNiagaraSystem;
	
	void AutoRun();

	// UPROPERTY(EditDefaultsOnly)
	// TSubclassOf<UDamageTextComponent> DamageTextComponentClass;

	// UPROPERTY(EditDefaultsOnly)
	// TSubclassOf<AMagicCircle> MagicCircleClass;
	//
	// UPROPERTY()
	// TObjectPtr<AMagicCircle> MagicCircle;

//	void UpdateMagicCircleLocation();
};
