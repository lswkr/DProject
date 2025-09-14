// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "GameFramework/Actor.h"
#include "DPSpawnVolume.generated.h"

enum class ECharacterClass : uint8;
class ABlockingVolume;
class ADPEnemyCharacter;
class UBoxComponent;

USTRUCT(BlueprintType)
struct FDPEnemySpawnerInfo //이 구조체로 데이터 테이블을 만들 것이다.
{
	GENERATED_BODY()//생각해보니 구조체도 클래스랑 별 다를 바가 없어서 GENERATED_BODY()붙여주는듯

	UPROPERTY(EditAnywhere)
	TSubclassOf<ADPEnemyCharacter> EnemyClass;

	UPROPERTY(EditAnywhere)
	int32 EnemyLevel = 1;

	UPROPERTY(EditAnywhere)
	ECharacterClass CharacterClass = ECharacterClass::Melee;
	
	UPROPERTY(EditAnywhere)
	int32 MinPerSpawnCount = 1;

	UPROPERTY(EditAnywhere)
	int32 MaxPerSpawnCount = 3;
};

USTRUCT(BlueprintType)
struct FDPEnemyWaveSpawnerTableRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FDPEnemySpawnerInfo> EnemySpawnerDefinitions;

	UPROPERTY(EditAnywhere)
	int32 TotalEnemyToSpawnThisMap = 1;

};

UCLASS()
class DPROJECT_API ADPSpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	ADPSpawnVolume();
	
	
	
protected:
	virtual void BeginPlay() override;
	

	UPROPERTY(BlueprintReadOnly)
	int32 SpawnedEnemiesNum = 0.f;

	UFUNCTION()
	void OnSpawnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UPROPERTY(EditAnywhere)
	FName TableName;
	
private:
	UPROPERTY()
	TArray<AActor*> TargetPointsArray;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> DoorBox;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> DoorMesh;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> SpawnBox;

	
	UFUNCTION()
	void OnEnemyDestroyed(AActor* DestroyedActor);

	UPROPERTY(EditDefaultsOnly)
	UDataTable* EnemyWaveSpawnerDataTable;

	UPROPERTY()
	int32 DeadEnemyCount = 0;

	bool HasFinishedStage() const;
};
