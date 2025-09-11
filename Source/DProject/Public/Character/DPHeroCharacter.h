// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/DPCharacterBase.h"
#include "Interface/PlayerInterface.h"
#include "DPHeroCharacter.generated.h"

/**
 * 
 */

class ADPPlayerState;
class UCameraComponent;
class USpringArmComponent;
class ADPWeaponBase;
class UBoxComponent;
class UNiagaraComponent;

UCLASS()
class DPROJECT_API ADPHeroCharacter : public ADPCharacterBase, public IPlayerInterface

{
	GENERATED_BODY()

public:
	ADPHeroCharacter();
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;


	/* Player Interface*/
	virtual void AddToXP_Implementation(int32 InXP) override;
	virtual void LevelUp_Implementation() override;
	virtual int32 GetXP_Implementation() const override;
	virtual int32 FindLevelForXP_Implementation(int32 XP) const override;
	virtual int32 GetAttributePointsReward_Implementation(int32 InLevel) const override;
	virtual int32 GetSpellPointsReward_Implementation(int32 InLevel) const override;
	virtual void AddToPlayerLevel_Implementation(int32 InPlayerLevel) override;
	virtual void AddToSpellPoints_Implementation(int32 InSpellPoints) override;
	virtual void AddToAttributePoints_Implementation(int32 InAttributePoints) override;
	virtual int32 GetAttributePoints_Implementation() const override;
	virtual int32 GetSpellPoints_Implementation() const override;
	virtual void ToggleWeaponCollision_Implementation(bool bShouldEnable) override;
	// virtual void ToggleBodyCollision_Implementation(bool bShouldEnable) override;
	/* End Player Interface*/

	/* Combat Interface */
	virtual int32 GetPlayerLevel_Implementation() override;
	/* End Combat Interface */

	UPROPERTY(EditDefaultsOnly)
	float DeathTime = 5.f;

	FTimerHandle DeathTimer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> LevelUpNiagaraComponent;
	
	// UFUNCTION(BlueprintCallable)
	// ADPWeaponBase* GetWeapon() const {return Weapon;}
	//
	// UPROPERTY(BlueprintAssignable)
	// FOnGetInPortal GetInPortalDelegate;
	//

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnBodyHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultRegeneratedAttributes;

	virtual void InitializeDefaultAttributes() const override;
	
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBoxComponent> BodyCollisionBox;
	
	UPROPERTY()
	TObjectPtr<ADPWeaponBase> Weapon;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<ADPWeaponBase> WeaponClass;

	UPROPERTY(EditDefaultsOnly)
	FName WeaponSocketName;
	
	virtual void InitAbilityActorInfo() override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastLevelUpParticles() const;
	

};
