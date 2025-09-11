// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/DPHeroCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "DPGameplayTags.h"
#include "AbilitySystem/DPAbilitySystemComponent.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Player/DPPlayerController.h"
#include "Player/DPPlayerState.h"
#include "UI/DPHUD.h"
#include "NiagaraComponent.h"

ADPHeroCharacter::ADPHeroCharacter()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->SetUsingAbsoluteRotation(true);
	SpringArm->bDoCollisionTest = false;

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

}

void ADPHeroCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitAbilityActorInfo();
	InitializeDefaultAttributes();
	AddCharacterAbilities();
}

void ADPHeroCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	InitAbilityActorInfo();
}

void ADPHeroCharacter::AddToXP_Implementation(int32 InXP)
{
	ADPPlayerState* DPPlayerState = GetPlayerState<ADPPlayerState>();
	check(DPPlayerState);
	DPPlayerState->AddToXP(InXP);
}


void ADPHeroCharacter::LevelUp_Implementation()
{
	MulticastLevelUpParticles();
}

int32 ADPHeroCharacter::GetXP_Implementation() const
{
	const ADPPlayerState* DPPlayerState = GetPlayerState<ADPPlayerState>();
	check(DPPlayerState);
	
	return DPPlayerState->GetXP();
}

int32 ADPHeroCharacter::FindLevelForXP_Implementation(int32 XP) const
{
	const ADPPlayerState* DPPlayerState = GetPlayerState<ADPPlayerState>();
	check(DPPlayerState);
	return DPPlayerState->LevelUpInfo->FindLevelForXP(XP);
}

int32 ADPHeroCharacter::GetAttributePointsReward_Implementation(int32 InLevel) const
{
	const ADPPlayerState* DPPlayerState = GetPlayerState<ADPPlayerState>();
	check(DPPlayerState);
	
	return DPPlayerState->LevelUpInfo->LevelUpInformation[InLevel].AttributePointReward;
}

int32 ADPHeroCharacter::GetSpellPointsReward_Implementation(int32 InLevel) const
{
	const ADPPlayerState* DPPlayerState = GetPlayerState<ADPPlayerState>();
	check(DPPlayerState);

	return DPPlayerState->LevelUpInfo->LevelUpInformation[InLevel].SkillPointReward;
}

void ADPHeroCharacter::AddToPlayerLevel_Implementation(int32 InPlayerLevel)
{
	ADPPlayerState* DPPlayerState = GetPlayerState<ADPPlayerState>();
	check(DPPlayerState);

	DPPlayerState->AddToLevel(InPlayerLevel);

	if (UDPAbilitySystemComponent* DPASC = Cast<UDPAbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		DPASC->UpdateAbilityStatus(DPPlayerState->GetPlayerLevel());
	}
}

void ADPHeroCharacter::AddToSpellPoints_Implementation(int32 InSpellPoints)
{
	ADPPlayerState* DPPlayerState = GetPlayerState<ADPPlayerState>();
	check(DPPlayerState);

	DPPlayerState->AddToSkillPoints(InSpellPoints);
}

void ADPHeroCharacter::AddToAttributePoints_Implementation(int32 InAttributePoints)
{
	ADPPlayerState* DPPlayerState = GetPlayerState<ADPPlayerState>();
	check(DPPlayerState);

	DPPlayerState->AddToAttributePoints(InAttributePoints);
}

int32 ADPHeroCharacter::GetAttributePoints_Implementation() const
{
	ADPPlayerState* DPPlayerState = GetPlayerState<ADPPlayerState>();
	check(DPPlayerState);
	return DPPlayerState->GetAttributePoints();
}

int32 ADPHeroCharacter::GetSpellPoints_Implementation() const
{
	ADPPlayerState* DPPlayerState = GetPlayerState<ADPPlayerState>();
	check(DPPlayerState);
	return DPPlayerState->GetSkillPoints();
}

int32 ADPHeroCharacter::GetPlayerLevel_Implementation()
{
	const ADPPlayerState* DPPlayerState = GetPlayerState<ADPPlayerState>();
	check(DPPlayerState);
	return DPPlayerState->GetPlayerLevel();
}

// void ADPHeroCharacter::ToggleWeaponCollision_Implementation(bool bShouldEnable)
// {
// 	if (!HasAuthority()) return;
// 	check(Weapon);
//
// 	if (bShouldEnable)
// 	{
// 		Weapon->GetCollisionBoxComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
// 	}
// 	else
// 	{
// 		Weapon->GetCollisionBoxComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
// 	}
// 	
// }

// void ADPHeroCharacter::ToggleBodyCollision_Implementation(bool bShouldEnable)
// {
// 	if (!HasAuthority()) return;
// 	if (bShouldEnable)
// 	{
// 		BodyCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
// 		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
// 	}
// 	else
// 	{
// 		BodyCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
// 		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
// 	}
// 	
// }

void ADPHeroCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	GetMesh()->HideBoneByName(TEXT("sword_bottom"), EPhysBodyOp::PBO_None);//스워드 및 방패 숨기기
	GetMesh()->HideBoneByName(TEXT("sword_top"), EPhysBodyOp::PBO_None);//스워드 및 방패 숨기기
	
	// // BodyCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnBodyHit);
	//
	// if(HasAuthority())
	// {
	// 	FActorSpawnParameters ActorSpawnParameters;
	// 	ActorSpawnParameters.Owner = this;
	// 	ActorSpawnParameters.Instigator = Cast<APawn>(this);
	// 	ActorSpawnParameters.SpawnCollisionHandlingOverride =  ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	// 	ActorSpawnParameters.TransformScaleMethod = ESpawnActorScaleMethod::MultiplyWithRoot;
	//
	// 	FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);
	// 	
	// 	// checkf(WeaponClass,TEXT("WeaponClass isn't selected in DPHeroCharacter. Please select WeaponClass."))
	// 	//
	// 	// Weapon = GetWorld()->SpawnActor<ADPWeaponBase>(WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator,ActorSpawnParameters);
	// 	// Weapon->SetOwner(this);
	// 	// Weapon->AttachToComponent(GetMesh(), AttachmentTransformRules, WeaponName);
	// 	//
	// 	// Weapon->SetReplicates(true);
	// 	// Weapon->SetReplicateMovement(true);
	// }
	//

}

void ADPHeroCharacter::OnBodyHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	UE_LOG(LogTemp, Warning, TEXT("%s detected"), *OtherActor->GetName());
	
	FGameplayEventData Data;
	Data.Instigator = this;
	Data.Target = OtherActor;

	//적군일 때에만 하도록
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		this,
		FDPGameplayTags::Get().Event_HitReact,
		Data
	);

	
}

void ADPHeroCharacter::InitializeDefaultAttributes() const
{
	Super::InitializeDefaultAttributes();
	ApplyEffectToSelf(DefaultRegeneratedAttributes,1);
}

void ADPHeroCharacter::InitAbilityActorInfo()
{
	ADPPlayerState* DPPlayerState = GetPlayerState<ADPPlayerState>();
	check(DPPlayerState);

	DPPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(DPPlayerState, this);
	Cast<UDPAbilitySystemComponent> (DPPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();

	AbilitySystemComponent = DPPlayerState->GetAbilitySystemComponent();
	AttributeSet = DPPlayerState->GetAttributeSet();

	OnASCRegistered.Broadcast(AbilitySystemComponent);
	AbilitySystemComponent->RegisterGameplayTagEvent(FDPGameplayTags::Get().Debuff_Stun, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ADPHeroCharacter::StunTagChanged);

	if (ADPPlayerController* DPPlayerController = Cast<ADPPlayerController>(GetController()))
	{
		if (ADPHUD* DPHUD = Cast<ADPHUD>(DPPlayerController->GetHUD()))
		{
			DPHUD -> InitOverlay(DPPlayerController, DPPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}
}

void ADPHeroCharacter::MulticastLevelUpParticles_Implementation() const
{
	if (IsValid(LevelUpNiagaraComponent))
	{
		LevelUpNiagaraComponent->SetWorldLocation(GetActorLocation());
		LevelUpNiagaraComponent->Activate(true);
	}
}
