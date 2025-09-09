// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/DPPlayerController.h"

#include "Components/SplineComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Input/DPInputComponent.h"

ADPPlayerController::ADPPlayerController()
{
	bReplicates = true;
	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
}

void ADPPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	
	Super::PlayerTick(DeltaTime);
	// CursorTrace();
	// AutoRun();
	//
}

void ADPPlayerController::BeginPlay()
{
	Super::BeginPlay();

	Super::BeginPlay();
	check(AuraContext);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	check(Subsystem)
	
	if (Subsystem)
	{
		Subsystem->AddMappingContext(AuraContext, 0);
	}

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void ADPPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* DPInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	DPInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADPPlayerController::Input_Move);
	// DPInputComponent->BindAction(ClickAction, ETriggerEvent::Started, this, &ADPPlayerController::InputPressed_RMB);
	// DPInputComponent->BindAction(ClickAction, ETriggerEvent::Completed, this, &ADPPlayerController::InputReleased_RMB);
	// DPInputComponent->BindAction(ClickAction, ETriggerEvent::Triggered, this, &ADPPlayerController::InputHeld_RMB);

	//DPInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);

}

void ADPPlayerController::Input_Move(const FInputActionValue& InputActionValue)
{
	bAutoRunning = false; //키보드로 움직이는 순간 AutoRun 멈추도록

	const FVector2D MovementVector = InputActionValue.Get<FVector2D>();
	const FRotator MovementRotation(0.f, this->GetControlRotation().Yaw, 0.f);

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		if (MovementVector.Y != 0.f)
		{
			const FVector ForwardDirection =MovementRotation.RotateVector(FVector::ForwardVector);
			ControlledPawn->AddMovementInput(ForwardDirection, MovementVector.Y);
		}
		if (MovementVector.X != 0.f)
		{
			const FVector RightDirection = MovementRotation.RotateVector(FVector::RightVector);
			ControlledPawn->AddMovementInput(RightDirection, MovementVector.X);
		}
	}
}
//
// void ADPPlayerController::CursorTrace()
// {
// }
//
// void ADPPlayerController::AutoRun()
// {
// }
//
