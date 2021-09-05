// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DamageTaker.h"
#include "TankFactory.generated.h"


class UStaticMeshComponent;
class UArrowComponent;
class UBoxComponent;
class UHealthComponent;
class ATankPawn;
class ATargetPoint;
class AMapLoader;


UCLASS()
class TANKOGEDDON_API ATankFactory : public AActor, public IDamageTaker
{
    GENERATED_BODY()

protected:
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
        UStaticMeshComponent* BuildingMesh;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
        UArrowComponent* TankSpawnPoint;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
        UBoxComponent* HitCollider;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
        UHealthComponent* HealthComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn tanks params")
        TSubclassOf<ATankPawn> SpawnTankClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spawn tanks params")
        float SpawnTankRate = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn tanks params")
        TArray<ATargetPoint*> TankWayPoints;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn tanks params")
        AMapLoader* LinkedMapLoader;

public:
    // Sets default values for this actor's properties
    ATankFactory();

    UFUNCTION()
        virtual bool TakeDamage(FDamageData DamageData) override;


protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    void SpawnNewTank();

    UFUNCTION()
        void Die();

    UFUNCTION()
        void DamageTaked(float DamageValue);
};