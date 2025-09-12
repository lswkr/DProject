// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/HighlightInterface.h"
#include "DPEntrance.generated.h"

class USphereComponent;

UCLASS()
class DPROJECT_API ADPEntrance : public AActor, public IHighlightInterface
{
	GENERATED_BODY()
	
public:	
	ADPEntrance();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/* Highlight Interface */
	virtual void SetMoveToLocation_Implementation(FVector& OutDestination) override;
	virtual void HighlightActor_Implementation() override;
	virtual void UnhighlightActor_Implementation() override;
	/* End Highlight Interface */
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> MoveToComponent;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> DestinationMap;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> RespawnMap;
	
	UPROPERTY(EditAnywhere)
	FName DestinationPlayerStartTag;

	UPROPERTY(EditAnywhere)
	FName RespawnPlayerStartTag;

	UPROPERTY(EditAnywhere)
	bool bIsRespawnEntrance = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> EntranceMesh;
	
	UPROPERTY(EditAnywhere)
	FName EntrancePlayerStartTag;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere;

	UPROPERTY(VisibleAnywhere)
	int32 CustomDepthStencilOverride = 252;

};
