// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "DPAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class DPROJECT_API UDPAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	static UDPAssetManager& Get();

protected:
	virtual void StartInitialLoading() override;
};
