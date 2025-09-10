// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/DPEnemyCharacter.h"

#include "DPGameplayTags.h"
#include "AbilitySystem/DPAbilitySystemComponent.h"
#include "AbilitySystem/DPAbilitySystemLibrary.h"
#include "AbilitySystem/DPAttributeSet.h"
//#include "Actor/Projectile/DPProjectileBase.h"
#include "AI/DPAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/WidgetComponent.h"
#include "DProject/DProject.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/Widget/DPUserWidget.h"

ADPEnemyCharacter::ADPEnemyCharacter()
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	AbilitySystemComponent = CreateDefaultSubobject<UDPAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bUseControllerDesiredRotation = true;

	AttributeSet = CreateDefaultSubobject<UDPAttributeSet> (TEXT("AttributeSet"));
	
	HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	HealthBar->SetupAttachment(GetRootComponent());

	MoveToComponent = CreateDefaultSubobject<USceneComponent>("MoveToComponent");
	MoveToComponent -> SetupAttachment(GetRootComponent());
	
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	GetMesh()->MarkRenderStateDirty();

	BaseWalkSpeed = 250.f;
}

void ADPEnemyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (!HasAuthority()) return;

	DPAIController = Cast<ADPAIController> (NewController);
	DPAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	DPAIController->RunBehaviorTree(BehaviorTree);
	DPAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), false);
	DPAIController->GetBlackboardComponent()->SetValueAsBool(FName("RangedAttacker"), CharacterClass == ECharacterClass::Range);
}

void ADPEnemyCharacter::HighlightActor_Implementation()
{
	GetMesh()->SetRenderCustomDepth(true);
}

void ADPEnemyCharacter::UnhighlightActor_Implementation()
{
	GetMesh()->SetRenderCustomDepth(false);
}

void ADPEnemyCharacter::SetMoveToLocation_Implementation(FVector& OutDestination)
{
	OutDestination = MoveToComponent->GetComponentLocation();
}

int32 ADPEnemyCharacter::GetPlayerLevel_Implementation()
{
	return Level;
}

void ADPEnemyCharacter::Die(const FVector& DeathImpulse)
{
	SetLifeSpan(LifeSpan);
	if (DPAIController) DPAIController->GetBlackboardComponent()->SetValueAsBool(FName("Dead"), true);
	//TODO : 보상 스폰
	Super::Die(DeathImpulse);
}

FVector ADPEnemyCharacter::GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag)
{
	const FDPGameplayTags& GameplayTags = FDPGameplayTags::Get();
	
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_LeftHand))
	{
		return GetMesh()->GetSocketLocation(LeftHandSocketName);
	}
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_RightHand))
	{
		return GetMesh()->GetSocketLocation(RightHandSocketName);
	}
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_Muzzle))
	{
		return GetMesh()->GetSocketLocation(MuzzleSocketName);
	}

	return FVector();
}

void ADPEnemyCharacter::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bHitReacting = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;
	if (DPAIController && DPAIController->GetBlackboardComponent())
	{
		DPAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), bHitReacting);
	}
}

void ADPEnemyCharacter::SetCombatTarget_Implementation(AActor* InCombatTarget)
{
	CombatTarget = InCombatTarget;
}

AActor* ADPEnemyCharacter::GetCombatTarget_Implementation()
{
	return CombatTarget;
}
//
// FProjectileInfo ADPEnemyCharacter::GetProjectileInfo_Implementation() const
// {
// 	if (CharacterClass == ECharacterClass::Range || CharacterClass == ECharacterClass::Complex)
// 	{
// 		return ProjectileInfo;	
// 	}
// 	return FProjectileInfo();
// }

//
// float ADPEnemyCharacter::GetFireDistance_Implementation() const
// {
// 	return FireDistance;
// }

void ADPEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;

	InitAbilityActorInfo();
	if (HasAuthority()) 
	{
		UDPAbilitySystemLibrary::GiveStartupAbilities(this, AbilitySystemComponent, CharacterClass);
	}
	if (UDPUserWidget* DPUserWidget = Cast<UDPUserWidget>(HealthBar->GetUserWidgetObject()))
	{
		DPUserWidget->SetWidgetController(this); //위젯 컨트롤러는 Enemy자기 자신
	}

	//값 초기화 후 위젯 바인딩
	if (const UDPAttributeSet* DPAS = Cast<UDPAttributeSet>(AttributeSet))
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(DPAS->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChanged.Broadcast(Data.NewValue);
			}
		);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(DPAS->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			}
		);
		
		AbilitySystemComponent->RegisterGameplayTagEvent(FDPGameplayTags::Get().Effects_HitReact, EGameplayTagEventType::NewOrRemoved).AddUObject(
			this,
			&ADPEnemyCharacter::HitReactTagChanged
		);
		//초기값 브로드캐스팅
		UE_LOG(LogTemp, Warning, TEXT("Enemy Health: %f"), DPAS->GetHealth());
		OnHealthChanged.Broadcast(DPAS->GetHealth());
		OnMaxHealthChanged.Broadcast(DPAS->GetMaxHealth());
	}
}

void ADPEnemyCharacter::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UDPAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
	AbilitySystemComponent->RegisterGameplayTagEvent(FDPGameplayTags::Get().Debuff_Stun, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ADPEnemyCharacter::StunTagChanged);


	if (HasAuthority())
	{
		InitializeDefaultAttributes();
	}
	OnASCRegistered.Broadcast(AbilitySystemComponent);
}

void ADPEnemyCharacter::InitializeDefaultAttributes() const
{
	UDPAbilitySystemLibrary::InitializeDefaultAttributes(this, CharacterClass, Level, AbilitySystemComponent);
}

void ADPEnemyCharacter::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	Super::StunTagChanged(CallbackTag, NewCount);
	
	if (DPAIController && DPAIController->GetBlackboardComponent())
	{
		DPAIController->GetBlackboardComponent()->SetValueAsBool(FName("Stunned"), bIsStunned);
	}
}
