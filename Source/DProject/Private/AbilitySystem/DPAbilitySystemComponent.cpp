// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/DPAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "DPGameplayTags.h"
#include "AbilitySystem/DPAbilitySystemLibrary.h"
#include "AbilitySystem/Ability/DPGameplayAbility.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Interface/CombatInterface.h"
#include "Interface/PlayerInterface.h"

void UDPAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UDPAbilitySystemComponent::ClientEffectApplied);
}

void UDPAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		if (const UDPGameplayAbility* DPAbility = Cast<UDPGameplayAbility>(AbilitySpec.Ability))
		{
			UE_LOG(LogTemp, Warning, TEXT("Ability Name: [%s]"), *AbilitySpec.GetDebugString());
			AbilitySpec.DynamicAbilityTags.AddTag(DPAbility->StartupInputTag);
			AbilitySpec.DynamicAbilityTags.AddTag(FDPGameplayTags::Get().Abilities_Status_Equipped);
			GiveAbility(AbilitySpec);
		}
	}
	bStartupAbilitiesGiven = true;
	AbilitiesGivenDelegate.Broadcast();
}

void UDPAbilitySystemComponent::AddCharacterPassiveAbilities(
	const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupPassiveAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		AbilitySpec.DynamicAbilityTags.AddTag(FDPGameplayTags::Get().Abilities_Status_Equipped);
		GiveAbilityAndActivateOnce(AbilitySpec);
	}
}

void UDPAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;
	
	FScopedAbilityListLock ActiveScopedLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{	
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (AbilitySpec.IsActive())
			{
				InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, AbilitySpec.Handle, AbilitySpec.ActivationInfo.GetActivationPredictionKey());
			}
		}
	}
}

void UDPAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;
	FScopedAbilityListLock ActiveScopedLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag) && AbilitySpec.IsActive())
		{
			AbilitySpecInputReleased(AbilitySpec);
			InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, AbilitySpec.Handle, AbilitySpec.ActivationInfo.GetActivationPredictionKey());
		}
	}
}

void UDPAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	FScopedAbilityListLock ActiveScopedLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UDPAbilitySystemComponent::ForEachAbility(const FForEachAbility& Delegate)
{
	FScopedAbilityListLock ActivateScopeLock(*this);
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!Delegate.ExecuteIfBound(AbilitySpec))
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to execute delegate in %hs"), __FUNCTION__);
		}
	}
}

FGameplayTag UDPAbilitySystemComponent::GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability)
	{
		for (FGameplayTag Tag : AbilitySpec.Ability.Get()->AbilityTags)
		{
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities"))))
			{
				return Tag;
			}
		}
	}
	return FGameplayTag();
}

FGameplayTag UDPAbilitySystemComponent::GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (FGameplayTag Tag : AbilitySpec.DynamicAbilityTags)
	{
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("InputTag"))))
		{
			return Tag;		
		}
	}
	return FGameplayTag();
}

FGameplayTag UDPAbilitySystemComponent::GetStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (FGameplayTag StatusTag : AbilitySpec.DynamicAbilityTags)
	{
		if (StatusTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities.Status"))))
		{
			return StatusTag;
		}
	}
	return FGameplayTag();
}

TArray<FGameplayTag> UDPAbilitySystemComponent::GetPostAbilitiesFromTag(const FGameplayTag& AbilityTag) const
{
	FDPGameplayTags GameplayTags = FDPGameplayTags::Get();
	UAbilityInfo* AbilityInfo = UDPAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	
	for (const FDPAbilityInfo& Info : AbilityInfo->AbilityInformation)
	{
		if (!Info.AbilityTag.IsValid()) continue;
		if (Info.AbilityTag == AbilityTag)
		{
			return Info.PostAbilityTags;
		}
	}
	return TArray<FGameplayTag>();
}

TArray<FGameplayTag> UDPAbilitySystemComponent::GetPreAbilitiesFromTag(const FGameplayTag& AbilityTag) const
{
	FDPGameplayTags GameplayTags = FDPGameplayTags::Get();
	UAbilityInfo* AbilityInfo = UDPAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	
	for (const FDPAbilityInfo& Info : AbilityInfo->AbilityInformation)
	{
		if (!Info.AbilityTag.IsValid()) continue;
		if (Info.AbilityTag == AbilityTag)
		{
			return Info.PreAbilityTags;
		}
	}
	return TArray<FGameplayTag>();
}

FDPAbilityInfo UDPAbilitySystemComponent::GetInfoFromTag(const FGameplayTag& AbilityTag) const
{
	FDPGameplayTags GameplayTags = FDPGameplayTags::Get();
	UAbilityInfo* AbilityInfo = UDPAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());

	for (const FDPAbilityInfo& Info : AbilityInfo->AbilityInformation)
	{
		if (!Info.AbilityTag.IsValid()) continue;
		if (Info.AbilityTag == AbilityTag)
		{
			return Info;
		}
	}
	return FDPAbilityInfo();
}



FGameplayTag UDPAbilitySystemComponent::GetStatusFromAbilityTag(const FGameplayTag& AbilityTag)
{
	if (const FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag))
	{
		return GetStatusFromSpec(*Spec);
	}
	return FGameplayTag();
}

FGameplayTag UDPAbilitySystemComponent::GetSlotFromAbilityTag(const FGameplayTag& AbilityTag)
{
	if (const FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag))
	{
		return GetInputTagFromSpec(*Spec);
	}
	return FGameplayTag();
}

bool UDPAbilitySystemComponent::SlotIsEmpty(const FGameplayTag& Slot)
{
	FScopedAbilityListLock ActiveScopedLock(*this);
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilityHasSlot(AbilitySpec, Slot))
		{
			return false;
		}
	}
	return true;
}

bool UDPAbilitySystemComponent::AbilityHasSlot(const FGameplayAbilitySpec& Spec, const FGameplayTag& Slot)
{
	return Spec.DynamicAbilityTags.HasTagExact(Slot);
}

bool UDPAbilitySystemComponent::AbilityHasAnySlot(const FGameplayAbilitySpec& Spec)
{
	return Spec.DynamicAbilityTags.HasTag(FGameplayTag::RequestGameplayTag(FName("InputTag")));
}

bool UDPAbilitySystemComponent::AbilityHasSlot(FGameplayAbilitySpec* Spec, const FGameplayTag& Slot)
{
	for (FGameplayTag Tag : Spec->DynamicAbilityTags)
	{
		if (Tag.MatchesTagExact(Slot))
		{
			return true;
		}
	}
	return false;
}

FGameplayAbilitySpec* UDPAbilitySystemComponent::GetSpecWithSlot(const FGameplayTag& Slot)
{
	FScopedAbilityListLock ActiveScopedLock(*this);

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(Slot))
		{
			return &AbilitySpec;
		}
	}
	return nullptr;
}


void UDPAbilitySystemComponent::AssignSlotToAbility(FGameplayAbilitySpec& Spec, const FGameplayTag& Slot)
{
	ClearSlot(&Spec);
	Spec.DynamicAbilityTags.AddTag(Slot);
}

FGameplayAbilitySpec* UDPAbilitySystemComponent::GetSpecFromAbilityTag(const FGameplayTag& AbilityTag)
{
	FScopedAbilityListLock ActiveScopedLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		for (FGameplayTag Tag : AbilitySpec.Ability.Get()->AbilityTags)
		{
			if (Tag.MatchesTag(AbilityTag))
			{
				return &AbilitySpec;
			}
		}
	}
	return nullptr;
}

void UDPAbilitySystemComponent::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		if (IPlayerInterface::Execute_GetAttributePoints(GetAvatarActor()) > 0)
		{
			ServerUpgradeAttribute(AttributeTag);
		}
	}
}

void UDPAbilitySystemComponent::ServerUpgradeAttribute_Implementation(const FGameplayTag& AttributeTag)
{
	FGameplayEventData Payload;
	Payload.EventTag = AttributeTag;
	Payload.EventMagnitude = 1.f;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), AttributeTag, Payload);

	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		IPlayerInterface::Execute_AddToAttributePoints(GetAvatarActor(), -1);
	}
}

void UDPAbilitySystemComponent::UpdateAbilityStatus(int32 Level) 
{
	FDPGameplayTags GameplayTags = FDPGameplayTags::Get();

	UAbilityInfo* AbilityInfo = UDPAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	for (const FDPAbilityInfo& Info : AbilityInfo->AbilityInformation)
	{
		if (!Info.AbilityTag.IsValid()) continue;
		if (Level<Info.LevelRequirement) continue;
		
		 if (GetSpecFromAbilityTag(Info.AbilityTag) == nullptr)
		 {
		 	if (Info.PreAbilityTags.IsEmpty())
		 	{
		 		UE_LOG(LogTemp, Warning, TEXT("Player Level: %d, PostAbility %s is added in UpdateAbilityStatus ."), Level, *Info.AbilityTag.ToString());
		 		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Info.Ability, 1);
		 		AbilitySpec.DynamicAbilityTags.AddTag(GameplayTags.Abilities_Status_Eligible);
		 		GiveAbility(AbilitySpec);
		 		MarkAbilitySpecDirty(AbilitySpec);
		 		ClientUpdateAbilityStatus(Info.AbilityTag, GameplayTags.Abilities_Status_Eligible, 1);
		 	
		 		return;
		 	}
		 	UE_LOG(LogTemp, Warning, TEXT("Player Level: %d, Ability %s is not qualified."), Level, *Info.AbilityTag.ToString());
		 	return;
		 }
		 FGameplayAbilitySpec AbilitySpec = *GetSpecFromAbilityTag(Info.AbilityTag);
		 if (AbilitySpec.DynamicAbilityTags.HasTagExact(GameplayTags.Abilities_Status_PreQualified))
		 {
		 	AbilitySpec.DynamicAbilityTags.RemoveTag(GameplayTags.Abilities_Status_PreQualified); //Prequalified삭제
		 	AbilitySpec.DynamicAbilityTags.AddTag(GameplayTags.Abilities_Status_Eligible); //Eligible
		 	ClientUpdateAbilityStatus(Info.AbilityTag, GameplayTags.Abilities_Status_Eligible, 1);
		 	UE_LOG(LogTemp, Warning, TEXT("Player Level: %d, PostAbility %s is added in UpdateAbilityStatus."), Level, *Info.AbilityTag.ToString());
		 }
		
	}
}

void UDPAbilitySystemComponent::ServerSpendSpellPoint_Implementation(const FGameplayTag& AbilityTag)
{
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		if (GetAvatarActor()->Implements<UPlayerInterface>())
		{
			IPlayerInterface::Execute_AddToSpellPoints(GetAvatarActor(), -1);
		}
		
		const FDPGameplayTags GameplayTags = FDPGameplayTags::Get();
		FGameplayTag Status = GetStatusFromSpec(*AbilitySpec);
		TArray<FGameplayTag> PostAbilitiesTags = GetPostAbilitiesFromTag(AbilityTag);
		
		if (Status.MatchesTagExact(GameplayTags.Abilities_Status_Eligible))
		{
			AActor* Player = GetAvatarActor();
			int32 PlayerLevel = 0;
			
			if (Player->Implements<UCombatInterface>())
			{
				PlayerLevel = ICombatInterface::Execute_GetPlayerLevel(Player);
			}
			
			AbilitySpec->DynamicAbilityTags.RemoveTag(GameplayTags.Abilities_Status_Eligible);//현재 어빌리티의 Eligible을 해제
			AbilitySpec->DynamicAbilityTags.AddTag(GameplayTags.Abilities_Status_Unlocked);
			Status = GameplayTags.Abilities_Status_Unlocked;

			for (const FGameplayTag& PostAbilityTag : PostAbilitiesTags) //Eligible이 될 만한 PostAbility찾기
			{
				FDPAbilityInfo PostAbilityInfo = GetInfoFromTag(PostAbilityTag);
				
				if (!PostAbilityInfo.AbilityTag.IsValid()) continue;
				
				int32 PostAbilityLevel = PostAbilityInfo.LevelRequirement;
				
				TArray<FGameplayTag> PreAbilitiesTags = PostAbilityInfo.PreAbilityTags;
				
				bool bPreSatisfied = true;
				for (const FGameplayTag& PreAbility : PreAbilitiesTags)
				{
					FGameplayTag PreStatus = GetStatusFromAbilityTag(PreAbility);
					if (PreStatus.MatchesTagExact(GameplayTags.Abilities_Status_Eligible) || PreStatus.MatchesTagExact(GameplayTags.Abilities_Status_Locked)||PreStatus == FGameplayTag())
					{
						bPreSatisfied = false;
						UE_LOG(LogTemp, Warning, TEXT("Player Level: %d, PostAbility %s is not qualified."), PlayerLevel, *PostAbilityTag.ToString());
						break;
					}
				}
				//둘 다 충족해야 Eligible

				bool LevelQualified = PostAbilityLevel <= PlayerLevel;
				if (bPreSatisfied && LevelQualified)
				{
					if (GetSpecFromAbilityTag(PostAbilityTag) == nullptr)
					{
						UE_LOG(LogTemp, Warning, TEXT("Player Level: %d, PostAbility %s is added in Spend Skill Point ."), PlayerLevel, *PostAbilityTag.ToString());
						FGameplayAbilitySpec PostAbilitySpec = FGameplayAbilitySpec(PostAbilityInfo.Ability, 1);
						PostAbilitySpec.DynamicAbilityTags.AddTag(GameplayTags.Abilities_Status_Eligible);
						GiveAbility(PostAbilitySpec);
						MarkAbilitySpecDirty(PostAbilitySpec);
						ClientUpdateAbilityStatus(PostAbilityInfo.AbilityTag, GameplayTags.Abilities_Status_Eligible, 1);
					}
				}
				//presatisfied이고 플레이어레벨이 요구조건보다 낮으면 PreQualified.
				else if (bPreSatisfied && !LevelQualified)
				{
					UE_LOG(LogTemp, Warning, TEXT("Player Level: %d, PostAbility %s is Prequalified."), PlayerLevel, *PostAbilityTag.ToString());
					FGameplayAbilitySpec PostAbilitySpec = FGameplayAbilitySpec(PostAbilityInfo.Ability, 1);
					PostAbilitySpec.DynamicAbilityTags.AddTag(GameplayTags.Abilities_Status_PreQualified);
					GiveAbility(PostAbilitySpec);
					MarkAbilitySpecDirty(PostAbilitySpec);
					ClientUpdateAbilityStatus(PostAbilityInfo.AbilityTag, GameplayTags.Abilities_Status_PreQualified, 1);
				}
				
			}
			
		}
		else if (Status.MatchesTagExact(GameplayTags.Abilities_Status_Equipped)||Status.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))
		{
			AbilitySpec->Level++;	
		}
		ClientUpdateAbilityStatus(AbilityTag, Status, AbilitySpec->Level);
		MarkAbilitySpecDirty(*AbilitySpec);
			
	}
}

void UDPAbilitySystemComponent::ServerEquipAbility_Implementation(const FGameplayTag& AbilityTag,
	const FGameplayTag& Slot)
{
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		const FDPGameplayTags& GameplayTags = FDPGameplayTags::Get();
		const FGameplayTag& PrevSlot = GetInputTagFromSpec(*AbilitySpec);
		const FGameplayTag& Status = GetStatusFromSpec(*AbilitySpec);

		const bool bStatusValid = Status == GameplayTags.Abilities_Status_Equipped || Status == GameplayTags.Abilities_Status_Unlocked;
		if (bStatusValid)
		{
			
			if (!SlotIsEmpty(Slot))
			{
				
				
				FGameplayAbilitySpec* SpecWithSlot = GetSpecWithSlot(Slot);
				if (SpecWithSlot)
				{
					if (AbilityTag.MatchesTagExact(GetAbilityTagFromSpec(*SpecWithSlot)))
					{
						ClientEquipAbility(AbilityTag, GameplayTags.Abilities_Status_Equipped, Slot, PrevSlot);
						return;
					}
					ClearSlot(SpecWithSlot);
				}
			}

			if (!AbilityHasAnySlot(*AbilitySpec))
			{
				AbilitySpec->DynamicAbilityTags.RemoveTag(GetStatusFromSpec(*AbilitySpec));
				AbilitySpec->DynamicAbilityTags.AddTag(GameplayTags.Abilities_Status_Equipped);
			}
			AssignSlotToAbility(*AbilitySpec, Slot);
			MarkAbilitySpecDirty(*AbilitySpec);
		}
		ClientEquipAbility(AbilityTag, GameplayTags.Abilities_Status_Equipped, Slot, PrevSlot);
	}
}

void UDPAbilitySystemComponent::ClientEquipAbility_Implementation(const FGameplayTag& AbilityTag,
	const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot)
{
	AbilityEquipped.Broadcast(AbilityTag, Status, Slot, PreviousSlot);
}

bool UDPAbilitySystemComponent::GetDescriptionByAbilityTag(const FGameplayTag& AbilityTag, FString& OutDescription)
{
	if (const FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		if (UDPGameplayAbility* DPAbility = Cast<UDPGameplayAbility>(AbilitySpec->Ability))
		{
			OutDescription = DPAbility->GetDescription(AbilitySpec->Level);
			return true;
		}
	}

	const UAbilityInfo* AbilityInfo =UDPAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	if (!AbilityTag.IsValid() || AbilityTag.MatchesTag(FDPGameplayTags::Get().Abilities_None))
	{
		OutDescription = FString();
	}
	else
	{
		OutDescription = UDPGameplayAbility::GetLockedDescription(AbilityInfo->FindAbilityInfoForTag(AbilityTag).LevelRequirement);
	}
	return false;
}

void UDPAbilitySystemComponent::ClearSlot(FGameplayAbilitySpec* Spec)
{
	const FGameplayTag Slot = GetInputTagFromSpec(*Spec);
	Spec->DynamicAbilityTags.RemoveTag(Slot);
}

void UDPAbilitySystemComponent::ClearAbilitiesOfSlot(const FGameplayTag& Slot)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (AbilityHasSlot(&Spec, Slot))
		{
			ClearSlot(&Spec);
		}
	}
}



void UDPAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	if (!bStartupAbilitiesGiven)
	{
		bStartupAbilitiesGiven = true;
		AbilitiesGivenDelegate.Broadcast();
	}
}

void UDPAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
	const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);

	EffectAssetTags.Broadcast(TagContainer);
}

void UDPAbilitySystemComponent::ClientUpdateAbilityStatus_Implementation(const FGameplayTag& AbilityTag,
	const FGameplayTag& StatusTag, int32 AbilityLevel)
{
	AbilityStatusChanged.Broadcast(AbilityTag, StatusTag, AbilityLevel/* , PrerequisiteAbilityTag*/);
}