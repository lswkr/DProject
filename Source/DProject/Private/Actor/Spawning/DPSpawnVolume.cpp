// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Spawning/DPSpawnVolume.h"

#include "NavigationSystem.h"
#include "Components/BoxComponent.h"
#include "Interface/PlayerInterface.h"
#include "Character/DPEnemyCharacter.h"
#include "Engine/TargetPoint.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ADPSpawnVolume::ADPSpawnVolume()
{
	PrimaryActorTick.bCanEverTick = false;
	
	SpawnBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnBox"));
	SetRootComponent(SpawnBox);
	SpawnBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SpawnBox->SetCollisionObjectType(ECC_WorldStatic);
	SpawnBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	SpawnBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	
	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	DoorBox = CreateDefaultSubobject<UBoxComponent>(TEXT("DoorBox"));
	DoorBox->SetCollisionResponseToAllChannels(ECR_Block);
	DoorBox->SetupAttachment(DoorMesh);
}

// Called when the game starts or when spawned
void ADPSpawnVolume::BeginPlay()
{
	Super::BeginPlay();
	SpawnBox->OnComponentBeginOverlap.AddDynamic(this, &ADPSpawnVolume::OnSpawnBoxOverlap);
}



void ADPSpawnVolume::OnSpawnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->Implements<UPlayerInterface>()) return; //플레이어 아니면 바로 리턴

	if (TargetPointsArray.IsEmpty())
	{
		//레벨에 배치해놓은 타겟 포인트 이용할 것임
		UGameplayStatics::GetAllActorsOfClass(this, ATargetPoint::StaticClass(), TargetPointsArray);//TargetPointsArray는 TargetPoint반환받을 어레이이다.
	}

	UE_LOG(LogTemp, Warning, TEXT("TableName = %s"), *TableName.ToString());
	const FName TName = TableName;
	FDPEnemyWaveSpawnerTableRow* FoundRow = EnemyWaveSpawnerDataTable->FindRow<FDPEnemyWaveSpawnerTableRow>(TName, FString());

	if (!FoundRow)
	{
		UE_LOG(LogTemp, Warning, TEXT("FoundRow doesn't exist "));
		return;
	}
	FActorSpawnParameters SpawnParam;
	SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	for (const FDPEnemySpawnerInfo& SpawnerInfo : FoundRow->EnemySpawnerDefinitions)
	{
		const int32 NumToSpawn = FMath::RandRange(SpawnerInfo.MinPerSpawnCount,SpawnerInfo.MaxPerSpawnCount);
		SpawnedEnemiesNum += NumToSpawn;

		for (int32 i = 0;i < NumToSpawn; i++)
		{
			const int32 RandomTargetPointIndex = FMath::RandRange(0,TargetPointsArray.Num()-1);
			const FVector SpawnOrigin = TargetPointsArray[RandomTargetPointIndex]->GetActorLocation();
			const FRotator SpawnRotation = TargetPointsArray[RandomTargetPointIndex]->GetActorForwardVector().ToOrientationRotator();

			
			FVector RandomLocation;//아래 함수로 참조받아서 값 받는 변수
			UNavigationSystemV1::K2_GetRandomLocationInNavigableRadius(this, SpawnOrigin, RandomLocation, 400.f);//400반경 내에 아무렇게나

			RandomLocation += FVector(0.f, 0.f, 150.f);
			FTransform SpawnTransform;
			SpawnTransform.SetLocation(RandomLocation);
			
			ADPEnemyCharacter* Enemy = GetWorld()->SpawnActorDeferred<ADPEnemyCharacter>(SpawnerInfo.EnemyClass, SpawnTransform);
			Enemy->SetLevel(SpawnerInfo.EnemyLevel);
			Enemy->SetCharacterClass(SpawnerInfo.CharacterClass);
			Enemy->FinishSpawning(SpawnTransform);
			Enemy->SpawnDefaultController();
			Enemy->OnDestroyed.AddUniqueDynamic(this, &ADPSpawnVolume::OnEnemyDestroyed);
			
		}
		
	}
	
}

void ADPSpawnVolume::OnEnemyDestroyed(AActor* DestroyedActor)
{
	DeadEnemyCount++;
	if (HasFinishedStage())
	{
		DoorBox->SetCollisionResponseToAllChannels(ECR_Ignore);
		DoorMesh->SetVisibility(false);
		SpawnedEnemiesNum = 0;
	}
}

bool ADPSpawnVolume::HasFinishedStage() const
{
	return DeadEnemyCount>=SpawnedEnemiesNum;
}

