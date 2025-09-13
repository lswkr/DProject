// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/TargetingCircle.h"

#include "Components/DecalComponent.h"


ATargetingCircle::ATargetingCircle()
{
	PrimaryActorTick.bCanEverTick = true;
	
	TargetingCircleDecal = CreateDefaultSubobject<UDecalComponent>("TargetingCircleDecal");
	TargetingCircleDecal->SetupAttachment(GetRootComponent());
}


void ATargetingCircle::BeginPlay()
{
	Super::BeginPlay();
	
}


void ATargetingCircle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

