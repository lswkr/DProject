// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/LoadSave/DPEntrance.h"

#include "Components/SphereComponent.h"
#include "Game/DPGameModeBase.h"
#include "Game/LoadScreenSaveGame.h"
#include "Interface/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"

ADPEntrance::ADPEntrance()
{
 	PrimaryActorTick.bCanEverTick = false;

	EntranceMesh = CreateDefaultSubobject<UStaticMeshComponent>("EntranceMesh");
	EntranceMesh->SetupAttachment(GetRootComponent());
	EntranceMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	EntranceMesh->SetCollisionResponseToAllChannels(ECR_Block);

	EntranceMesh->SetCustomDepthStencilValue(CustomDepthStencilOverride);
	EntranceMesh->MarkRenderStateDirty();

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	Sphere->SetupAttachment(EntranceMesh);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	MoveToComponent = CreateDefaultSubobject<USceneComponent>("MoveToComponent");
	MoveToComponent->SetupAttachment(GetRootComponent());

}

void ADPEntrance::BeginPlay()
{
	Super::BeginPlay();

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &ADPEntrance::OnSphereOverlap);
}

void ADPEntrance::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->Implements<UPlayerInterface>())
	{
		if (ADPGameModeBase* DPGM = Cast<ADPGameModeBase>(UGameplayStatics::GetGameMode(this)))
		{
			if (bIsRespawnEntrance) //리스폰 구역 출입구면 리스폰 지역 저장
			{
				ULoadScreenSaveGame* SaveData = DPGM->RetrieveInGameSaveData();
				const UWorld* World = GetWorld();
				FString MapName = World->GetMapName();
				SaveData->RespawnMapAssetName = MapName;//애셋 그대로의 이름
				MapName.RemoveFromStart(World->StreamingLevelsPrefix);
				SaveData->RespawnMapName = MapName;//StreamingLevelsPrefix를 제거한 이름
			}
			//저장
			DPGM->SaveWorldState(GetWorld(), DestinationMap.ToSoftObjectPath().GetAssetName());
			
		}
		//저장 및 새 맵 로드
		IPlayerInterface::Execute_SaveProgress(OtherActor, EntrancePlayerStartTag);
		UGameplayStatics::OpenLevelBySoftObjectPtr(this, DestinationMap);
	}
}

void ADPEntrance::SetMoveToLocation_Implementation(FVector& OutDestination)
{
	OutDestination = MoveToComponent->GetComponentLocation();
}

void ADPEntrance::HighlightActor_Implementation()
{
	EntranceMesh->SetRenderCustomDepth(true);
}

void ADPEntrance::UnhighlightActor_Implementation()
{
	EntranceMesh->SetRenderCustomDepth(false);
}


