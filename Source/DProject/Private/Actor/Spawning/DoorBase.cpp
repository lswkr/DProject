// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Spawning/DoorBase.h"

// Sets default values
ADoorBase::ADoorBase()
{
	PrimaryActorTick.bCanEverTick = false;
	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(DoorMesh);
}

void ADoorBase::BeginPlay()
{
	Super::BeginPlay();
	
}

