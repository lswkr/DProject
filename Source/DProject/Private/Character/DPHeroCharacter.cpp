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
#include "AbilitySystem/DPAbilitySystemLibrary.h"
#include "AbilitySystem/DPAttributeSet.h"
#include "Actor/Weapon/DPWeaponBase.h"
#include "Components/BoxComponent.h"
#include "Game/DPGameModeBase.h"
#include "Game/LoadScreenSaveGame.h"
#include "Kismet/GameplayStatics.h"

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
	LoadProgress();

	///LoadSave start
	// if (ADPGameModeBase* DPGameMode = Cast<ADPGameModeBase>(UGameplayStatics::GetGameMode(this)))
	// {
	// 	DPGameMode->LoadWorldState(GetWorld());
	// }
	///LoadSave end

	//Dev Code
	InitializeDefaultAttributes();
	AddCharacterAbilities();
	//DevCode end
	
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

void ADPHeroCharacter::Die(const FVector& DeathImpulse)
{
	Super::Die(DeathImpulse);
	FTimerDelegate DeathTimerDelegate;
	DeathTimerDelegate.BindLambda([this]()
	{
		ADPGameModeBase* DPGM = Cast<ADPGameModeBase>(UGameplayStatics::GetGameMode(this));
		if (DPGM)
		{
			DPGM->PlayerDied(this);
		}
	});
	GetWorldTimerManager().SetTimer(DeathTimer, DeathTimerDelegate, DeathTime, false);
	Camera->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	
}

void ADPHeroCharacter::ToggleWeaponCollision_Implementation(bool bShouldEnable)
{
	if (!HasAuthority()) return;
	check(Weapon);

	if (bShouldEnable)
	{
		Weapon->GetWeaponBoxComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	else
	{
		Weapon->GetWeaponBoxComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	
}

void ADPHeroCharacter::ShowTargetingCircle_Implementation(UMaterialInterface* DecalMaterial)
{
	if (ADPPlayerController* DPPlayerController = Cast<ADPPlayerController>(GetController()))
	{
		DPPlayerController->ShowTargetingCircle(DecalMaterial);
		DPPlayerController->bShowMouseCursor = false;
	}
}

void ADPHeroCharacter::HideTargetingCircle_Implementation()
{
	if (ADPPlayerController* DPPlayerController = Cast<ADPPlayerController>(GetController()))
	{
		DPPlayerController->HideTargetingCircle();
		DPPlayerController->bShowMouseCursor = true;
	}
}

void ADPHeroCharacter::SaveProgress_Implementation(const FName& CheckpointTag)
{
	ADPGameModeBase* DPGameMode = Cast<ADPGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (DPGameMode)
	{
		ULoadScreenSaveGame* SaveData = DPGameMode->RetrieveInGameSaveData();
		if (SaveData == nullptr) return;

		SaveData->PlayerStartTag = CheckpointTag;

		if (ADPPlayerState* DPPlayerState = Cast<ADPPlayerState>(GetPlayerState()))
		{
			SaveData->PlayerLevel = DPPlayerState->GetPlayerLevel();
			SaveData->XP = DPPlayerState->GetXP();
			SaveData->AttributePoints = DPPlayerState->GetAttributePoints();
			SaveData->SkillPoints = DPPlayerState->GetSkillPoints();
		}
		SaveData->Strength = UDPAttributeSet::GetStrengthAttribute().GetNumericValue(GetAttributeSet());
		SaveData->Intelligence = UDPAttributeSet::GetIntelligenceAttribute().GetNumericValue(GetAttributeSet());
		SaveData->Luck = UDPAttributeSet::GetLuckAttribute().GetNumericValue(GetAttributeSet());
		SaveData->Will = UDPAttributeSet::GetWillAttribute().GetNumericValue(GetAttributeSet());

		SaveData->bFirstTimeLoadIn = false;

		if (!HasAuthority()) return;

		UDPAbilitySystemComponent* DPASC = Cast<UDPAbilitySystemComponent>(AbilitySystemComponent);
		FForEachAbility SaveAbilityDelegate;
		SaveData->SavedAbilities.Empty();
		SaveAbilityDelegate.BindLambda([this, DPASC, SaveData](const FGameplayAbilitySpec& AbilitySpec)
		{
			const FGameplayTag AbilityTag = DPASC->GetAbilityTagFromSpec(AbilitySpec);
			UAbilityInfo* AbilityInfo = UDPAbilitySystemLibrary::GetAbilityInfo(this);
			FDPAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);

			FSavedAbility SavedAbility;
			SavedAbility.GameplayAbility = Info.Ability;
			SavedAbility.AbilityLevel = AbilitySpec.Level;
			SavedAbility.AbilitySlot = DPASC->GetSlotFromAbilityTag(AbilityTag);
			SavedAbility.AbilityStatus = DPASC->GetStatusFromAbilityTag(AbilityTag);
			SavedAbility.AbilityTag = AbilityTag;
			SavedAbility.AbilityType = Info.AbilityType;

			SaveData->SavedAbilities.AddUnique(SavedAbility);

		});
		DPASC->ForEachAbility(SaveAbilityDelegate);
		
		DPGameMode->SaveInGameProgressData(SaveData);
	}
}

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
	if(HasAuthority())
	{
		FActorSpawnParameters ActorSpawnParameters;
		ActorSpawnParameters.Owner = this;
		ActorSpawnParameters.Instigator = Cast<APawn>(this);
		ActorSpawnParameters.SpawnCollisionHandlingOverride =  ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ActorSpawnParameters.TransformScaleMethod = ESpawnActorScaleMethod::MultiplyWithRoot;
	
		FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);
		
		checkf(WeaponClass,TEXT("WeaponClass isn't selected in DPHeroCharacter. Please select WeaponClass."))
		
		Weapon = GetWorld()->SpawnActor<ADPWeaponBase>(WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator,ActorSpawnParameters);
		Weapon->SetOwner(this);
		Weapon->AttachToComponent(GetMesh(), AttachmentTransformRules, WeaponSocketName);
		
		Weapon->SetReplicates(true);
		Weapon->SetReplicateMovement(true);
	}
	

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

void ADPHeroCharacter::LoadProgress()
{
	ADPGameModeBase* DPGameMode = Cast<ADPGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (DPGameMode)
	{
		ULoadScreenSaveGame* SaveData = DPGameMode->RetrieveInGameSaveData();
		if (SaveData == nullptr) return;

		if (SaveData->bFirstTimeLoadIn)
		{
			InitializeDefaultAttributes();
			AddCharacterAbilities();
		}
		else
		{
			if (UDPAbilitySystemComponent* DPASC = Cast<UDPAbilitySystemComponent>(AbilitySystemComponent))
			{
				DPASC->AddCharacterAbilitiesFromSaveData(SaveData);
			}
			
			if (ADPPlayerState* DPPlayerState = Cast<ADPPlayerState>(GetPlayerState()))
			{
				DPPlayerState->SetLevel(SaveData->PlayerLevel);
				DPPlayerState->SetXP(SaveData->XP);
				DPPlayerState->SetAttributePoints(SaveData->AttributePoints);
				DPPlayerState->SetSkillPoints(SaveData->SkillPoints);
			}
			
			UDPAbilitySystemLibrary::InitializeDefaultAttributesFromSaveData(this, AbilitySystemComponent, SaveData);
		}
	}
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
