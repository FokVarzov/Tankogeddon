// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BasePawnAIController.generated.h"


class APawn;
class ABasePawn;

/**
 *
 */
UCLASS()
class TANKOGEDDON_API ABasePawnAIController : public AAIController
{
    GENERATED_BODY()

protected:
    UPROPERTY()
        ABasePawn* MyPawn;

    UPROPERTY()
        APawn* PlayerPawn;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Targeting")
        float TargetingRange = 1000.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Targeting")
        float Accuracy = 10.f;

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    void RotateToPlayer();
    bool IsPlayerInRange();
    bool DetectCanFire();
    void Fire();
    bool DetectPlayerVisibility();
};
