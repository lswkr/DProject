// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "Interface/HighlightInterface.h"
#include "Interface/SaveInterface.h"
#include "DPSpawnPoint.generated.h"

/**
 * 
 */
class USphereComponent;

UCLASS()
class DPROJECT_API ADPSpawnPoint : public APlayerStart, public ISaveInterface, public IHighlightInterface
{
	GENERATED_BODY()

public:
	ADPSpawnPoint(const FObjectInitializer& ObjectInitializer);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> SpawnPointMesh;
};
