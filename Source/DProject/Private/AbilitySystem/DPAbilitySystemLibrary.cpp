// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/DPAbilitySystemLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "DPGameplayTags.h"
#include "Game/DPGameModeBase.h"
#include "Interface/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Player/DPPlayerState.h"
#include "UI/DPHUD.h"
#include "UI/WidgetController/DPWidgetController.h"


bool UDPAbilitySystemLibrary::MakeWidgetControllerParams(const UObject* WorldContextObject,
	FWidgetControllerParams& OutWCParams, ADPHUD*& OutDPHUD)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		OutDPHUD = Cast<ADPHUD>(PC->GetHUD());
		if (OutDPHUD)
		{
			UE_LOG(LogTemp, Warning, TEXT("WidgetController Made Successfully"));
			ADPPlayerState* PS = PC->GetPlayerState<ADPPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			
			OutWCParams.AttributeSet = AS;
			OutWCParams.AbilitySystemComponent = ASC;
			OutWCParams.PlayerState = PS;
			OutWCParams.PlayerController = PC;
			return true;
		}
	}
	return false;
}

UOverlayWidgetController* UDPAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	ADPHUD* DPHUD = nullptr;
	
	if (MakeWidgetControllerParams(WorldContextObject, WCParams, DPHUD))
	{
		return DPHUD->GetOverlayWidgetController(WCParams);
	}
	return nullptr;
}

UAttributeMenuWidgetController* UDPAbilitySystemLibrary::GetAttributeMenuWidgetController(
	const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	ADPHUD* DPHUD = nullptr;
	
	if (MakeWidgetControllerParams(WorldContextObject, WCParams, DPHUD))
	{
		return DPHUD->GetAttributeMenuWidgetController(WCParams);
	}
	return nullptr;
}

USkillMenuWidgetController* UDPAbilitySystemLibrary::GetSkillMenuWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	ADPHUD* DPHUD = nullptr;
	
	if (MakeWidgetControllerParams(WorldContextObject, WCParams, DPHUD))
	{
		return DPHUD->GetSkillMenuWidgetController(WCParams);
	}
	return nullptr;
}

UAbilityInfo* UDPAbilitySystemLibrary::GetAbilityInfo(const UObject* WorldContextObject)
{
	const ADPGameModeBase* DPGameMode = Cast<ADPGameModeBase> (UGameplayStatics::GetGameMode(WorldContextObject));
	if (DPGameMode == nullptr) return nullptr;
	return DPGameMode->AbilityInfo;
}

void UDPAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject,
	ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC)
{
	AActor* AvatarActor = ASC->GetAvatarActor();
	UE_LOG(LogTemp, Warning, TEXT("%s Attributes(UDPAbilitySystemLibrary::Line 24)"), *WorldContextObject->GetName());
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	FCharacterClassDefaultInfo ClassDefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);

	FGameplayEffectContextHandle PrimaryAttributesContextHandle = ASC->MakeEffectContext();
	PrimaryAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle PrimaryAttributesSpecHandle = ASC->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttribute, Level, PrimaryAttributesContextHandle);
	UE_LOG(LogTemp, Warning, TEXT("%s's Primary Attributes(UDPAbilitySystemLibrary::Line 24)"), *ClassDefaultInfo.PrimaryAttribute->GetName());
	ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle SecondaryAttributesContextHandle = ASC->MakeEffectContext();
	SecondaryAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle SecondaryAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->SecondaryAttribute, Level, SecondaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle VitalAttributesContextHandle = ASC->MakeEffectContext();
	VitalAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle VitalAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->VitalAttribute, Level, VitalAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalAttributesSpecHandle.Data.Get());
}

void UDPAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC,
	ECharacterClass CharacterClass)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (CharacterClassInfo == nullptr) return;
	for (TSubclassOf<UGameplayAbility> AbilityClass: CharacterClassInfo->CommonAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);//CommonAbilities에는 HitReact나 Death같은 어빌리티만 있어 레벨 1로만 설정
		ASC->GiveAbility(AbilitySpec);
	}
	
	const FCharacterClassDefaultInfo& DefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
	for (TSubclassOf<UGameplayAbility> AbilityClass : DefaultInfo.StartupAbilities)
	{
		
		if (ASC->GetAvatarActor()->Implements<UCombatInterface>())
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, ICombatInterface::Execute_GetPlayerLevel(ASC->GetAvatarActor()));
			ASC->GiveAbility(AbilitySpec);
		}
	}
}

UCharacterClassInfo* UDPAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	const ADPGameModeBase* DPGameMode = Cast<ADPGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (DPGameMode == nullptr) return nullptr;
	UE_LOG(LogTemp, Warning, TEXT("GameMode Exist (UDPAbilitySystemLibrary::line71)"));
	return DPGameMode->CharacterClassInfo;
}

void UDPAbilitySystemLibrary::GetLivePlayerWithinRadius(const UObject* WorldContextObject,
	TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float Radius,
	const FVector& SphereOrigin)
{
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorsToIgnore);
	
	TArray<FOverlapResult> Overlaps;
	
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		World->OverlapMultiByObjectType(Overlaps, SphereOrigin, FQuat::Identity, FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects), FCollisionShape::MakeSphere(Radius), SphereParams);
		for (FOverlapResult& Overlap : Overlaps)
		{
			
			if (Overlap.GetActor()->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(Overlap.GetActor()))
			{
				OutOverlappingActors.AddUnique(ICombatInterface::Execute_GetAvatarActor(Overlap.GetActor()));
			}
		}
	}	
}

bool UDPAbilitySystemLibrary::IsHostile(AActor* FirstActor, AActor* SecondActor)
{
	const bool bBothArePlayers = FirstActor->ActorHasTag(FName("Player")) && SecondActor->ActorHasTag(FName("Player"));
	const bool bBothAreEnemies = FirstActor->ActorHasTag(FName("Enemy")) && SecondActor->ActorHasTag(FName("Enemy"));

	return !(bBothArePlayers || bBothAreEnemies);
}

FGameplayEffectContextHandle UDPAbilitySystemLibrary::ApplyDamageEffect(const FDamageEffectParams& DamageEffectParams)
{
	const FDPGameplayTags& DPGameplayTags = FDPGameplayTags::Get();
	const AActor* SourceAvatarActor = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();


	FGameplayEffectContextHandle EffectContextHandle =  DamageEffectParams.SourceAbilitySystemComponent->MakeEffectContext();
	EffectContextHandle.AddSourceObject(SourceAvatarActor);
	SetDeathImpulse(EffectContextHandle, DamageEffectParams.DeathImpulse);

	if (EffectContextHandle.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("EffectContextHandle is valid"));

	}

	const FGameplayEffectSpecHandle SpecHandle = DamageEffectParams.SourceAbilitySystemComponent->MakeOutgoingSpec(DamageEffectParams.DamageGameplayEffectClass, DamageEffectParams.AbilityLevel,EffectContextHandle);

	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageEffectParams.DamageType, DamageEffectParams.BaseDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DPGameplayTags.Debuff_Chance, DamageEffectParams.DebuffChance);//chance값 만들기
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DPGameplayTags.Debuff_Damage, DamageEffectParams.DebuffDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DPGameplayTags.Debuff_Duration, DamageEffectParams.DebuffDuration);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DPGameplayTags.Debuff_Frequency, DamageEffectParams.DebuffFrequency);

	if (!SpecHandle.Data.IsValid())
	{
		
		return FGameplayEffectContextHandle();
	}
	if (!IsValid(DamageEffectParams.TargetAbilitySystemComponent))
	{
		UE_LOG(LogTemp, Warning, TEXT("TargetAbilitySystemComponent is not valid"));
		return EffectContextHandle;
	}
	DamageEffectParams.TargetAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	return EffectContextHandle;
}

TArray<FRotator> UDPAbilitySystemLibrary::EvenlySpacedRotators(const FVector& Forward, const FVector& Axis,
	float Spread, int32 NumRotators)
{
	TArray<FRotator> Rotators;
	
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-Spread / 2.f, Axis);
	if (NumRotators > 1)
	{
		const float DeltaSpread = Spread / (NumRotators - 1);
		for (int32 i = 0; i < NumRotators; i++)
		{
			const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
			Rotators.Add(Direction.Rotation());
		}
	}
	else
	{
		Rotators.Add(Forward.Rotation());
	}
	return Rotators;
}

int32 UDPAbilitySystemLibrary::GetXPRewardForClassAndLevel(const UObject* WorldContextObject, int32 CharacterLevel)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (CharacterClassInfo == nullptr) return 0;

	const float XPReward = CharacterClassInfo->XPReward.GetValueAtLevel(CharacterLevel);

	return static_cast<int32>(XPReward);
}

bool UDPAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FDPGameplayEffectContext* DPEffectContext = static_cast<const FDPGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return DPEffectContext->IsBlockedHit();
	}
	return false;
}

bool UDPAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FDPGameplayEffectContext* DPEffectContext = static_cast<const FDPGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return DPEffectContext->IsCriticalHit();
	}
	return false;
}

bool UDPAbilitySystemLibrary::IsSuccessfulDebuff(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FDPGameplayEffectContext* DPEffectContext = static_cast<const FDPGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return DPEffectContext->IsSuccessfulDebuff();
	}
	return false;
}

float UDPAbilitySystemLibrary::GetDebuffDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FDPGameplayEffectContext* DPEffectContext = static_cast<const FDPGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return DPEffectContext->GetDebuffDamage();
	}
	return 0.f;
}

float UDPAbilitySystemLibrary::GetDebuffDuration(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FDPGameplayEffectContext* DPEffectContext = static_cast<const FDPGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return DPEffectContext->GetDebuffDuration();
	}
	return 0.f;
}

float UDPAbilitySystemLibrary::GetDebuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FDPGameplayEffectContext* DPEffectContext = static_cast<const FDPGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return DPEffectContext->GetDebuffFrequency();
	}
	return 0.f;
}

FGameplayTag UDPAbilitySystemLibrary::GetDamageType(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FDPGameplayEffectContext* DPEffectContext = static_cast<const FDPGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		if (DPEffectContext->GetDamageType().IsValid())
		{
			return *DPEffectContext->GetDamageType();
		}
	}
	return FGameplayTag();
}

FVector UDPAbilitySystemLibrary::GetDeathImpulse(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FDPGameplayEffectContext* DPEffectContext = static_cast<const FDPGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return DPEffectContext->GetDeathImpulse();
	}
	return FVector::ZeroVector;
}

void UDPAbilitySystemLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit)
{
	if (FDPGameplayEffectContext* DPEffectContext = static_cast<FDPGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		DPEffectContext->SetIsCriticalHit(bInIsCriticalHit);
	}
}

void UDPAbilitySystemLibrary::SetIsBlockedHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit)
{
	if (FDPGameplayEffectContext* DPEffectContext = static_cast<FDPGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		DPEffectContext->SetIsBlockedHit(bInIsBlockedHit);
	}
}

void UDPAbilitySystemLibrary::SetIsSuccessfulDebuff(FGameplayEffectContextHandle& EffectContextHandle,
	bool bInSuccessfulDebuff)
{
	if (FDPGameplayEffectContext* DPEffectContext = static_cast<FDPGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		DPEffectContext->SetIsCriticalHit(bInSuccessfulDebuff);
	}
}

void UDPAbilitySystemLibrary::SetDebuffDamage(FGameplayEffectContextHandle& EffectContextHandle, float InDamage)
{
	if (FDPGameplayEffectContext* DPEffectContext = static_cast<FDPGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		DPEffectContext->SetDebuffDamage(InDamage);
	}
}

void UDPAbilitySystemLibrary::SetDebuffDuration(FGameplayEffectContextHandle& EffectContextHandle, float InDuration)
{
	if (FDPGameplayEffectContext* DPEffectContext = static_cast<FDPGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		DPEffectContext->SetDebuffDuration(InDuration);
	}
}

void UDPAbilitySystemLibrary::SetDebuffFrequency(FGameplayEffectContextHandle& EffectContextHandle, float InFrequency)
{
	if (FDPGameplayEffectContext* DPEffectContext = static_cast<FDPGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		DPEffectContext->SetDebuffFrequency(InFrequency);
	}
}

void UDPAbilitySystemLibrary::SetDamageType(FGameplayEffectContextHandle& EffectContextHandle,
	const FGameplayTag& InDamageType)
{
	if (FDPGameplayEffectContext* DPEffectContext = static_cast<FDPGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		const TSharedPtr<FGameplayTag> DamageType = MakeShared<FGameplayTag>(InDamageType);
		DPEffectContext->SetDamageType(DamageType);
	}
}



void UDPAbilitySystemLibrary::SetDeathImpulse(FGameplayEffectContextHandle& EffectContextHandle,
                                              const FVector& InImpulse)
{
	if (FDPGameplayEffectContext* DPEffectContext = static_cast<FDPGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		DPEffectContext->SetDeathImpulse(InImpulse);
	}
}
