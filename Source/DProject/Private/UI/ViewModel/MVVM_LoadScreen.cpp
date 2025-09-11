// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ViewModel/MVVM_LoadScreen.h"

#include "Game/DPGameInstance.h"
#include "Game/DPGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"

void UMVVM_LoadScreen::InitializeLoadSlots()
{
	LoadSlot_0 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_0->SetLoadSlotName(FString("LoadSlot_0"));
	LoadSlot_0->SlotIndex = 0;
	LoadSlots.Add(0, LoadSlot_0);
	LoadSlot_1 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlots.Add(1, LoadSlot_1);
	LoadSlot_1->SlotIndex = 1;
	LoadSlot_1->SetLoadSlotName(FString("LoadSlot_1"));
	LoadSlot_2 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlots.Add(2, LoadSlot_2);
	LoadSlot_2->SetLoadSlotName(FString("LoadSlot_2"));
	LoadSlot_2->SlotIndex = 2;

	SetNumLoadSlots(LoadSlots.Num());
}

UMVVM_LoadSlot* UMVVM_LoadScreen::GetLoadSlotViewModelByIndex(int32 Index) const
{
	return LoadSlots.FindChecked(Index);
}

void UMVVM_LoadScreen::NewSlotButtonPressed(int32 Slot, const FString& EnteredName)
{
	ADPGameModeBase* DPGameMode = Cast<ADPGameModeBase>(UGameplayStatics::GetGameMode(this));

	if (!IsValid(DPGameMode))
	{
		GEngine->AddOnScreenDebugMessage(1, 15.f, FColor::Magenta, FString("Please switch to Single Player"));
		return;
	}

	LoadSlots[Slot]->SetMapName(DPGameMode->DefaultMapName);
	LoadSlots[Slot]->SetPlayerName(EnteredName);
	LoadSlots[Slot]->SetPlayerLevel(1);
	LoadSlots[Slot]->SlotStatus = Taken;
	LoadSlots[Slot]->PlayerStartTag = DPGameMode->DefaultPlayerStartTag;
	LoadSlots[Slot]->MapAssetName = DPGameMode->DefaultMap.ToSoftObjectPath().GetAssetName();

	DPGameMode->SaveSlotData(LoadSlots[Slot], Slot);
	LoadSlots[Slot]->InitializeSlot();

	UDPGameInstance* DPGameInstance = Cast<UDPGameInstance>(DPGameMode->GetGameInstance());
	DPGameInstance->LoadSlotName = LoadSlots[Slot]->GetLoadSlotName();
	DPGameInstance->LoadSlotIndex = LoadSlots[Slot]->SlotIndex;
	DPGameInstance->PlayerStartTag = DPGameMode->DefaultPlayerStartTag;
}

void UMVVM_LoadScreen::NewGameButtonPressed(int32 Slot)
{
}

void UMVVM_LoadScreen::SelectSlotButtonPressed(int32 Slot)
{
}

void UMVVM_LoadScreen::DeleteButtonPressed()
{
}

void UMVVM_LoadScreen::PlayButtonPressed()
{
}

void UMVVM_LoadScreen::LoadData()
{
}

void UMVVM_LoadScreen::SetNumLoadSlots(int32 InNumLoadSlots)
{
}
