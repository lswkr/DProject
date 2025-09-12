// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/LoadSave/DPSpawnPoint.h"


ADPSpawnPoint::ADPSpawnPoint(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	
	SpawnPointMesh = CreateDefaultSubobject<UStaticMeshComponent>("SpawnPointMesh");
	SpawnPointMesh->SetupAttachment(GetRootComponent());
	//CheckpointMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SpawnPointMesh->SetCollisionResponseToAllChannels(ECR_Block);
	
}
