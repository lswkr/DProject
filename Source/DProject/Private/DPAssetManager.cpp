// Fill out your copyright notice in the Description page of Project Settings.


#include "DPAssetManager.h"


#include "DPGameplayTags.h"

UDPAssetManager& UDPAssetManager::Get()
{
	check(GEngine);

	UDPAssetManager* DPAssetManager = Cast<UDPAssetManager>(GEngine->AssetManager);
	return *DPAssetManager;
}

void UDPAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	FDPGameplayTags::InitializeNativeGameplayTags();
}
