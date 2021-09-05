// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePawnAIController.h"
#include "TankAIController.generated.h"


class ATankPawn;


/**
 *
 */
UCLASS()
class TANKOGEDDON_API ATankAIController : public ABasePawnAIController
{
    GENERATED_BODY()

protected:
    UPROPERTY()
        ATankPawn* TankPawn;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Move params|Patrol points", Meta = (MakeEditWidget = true))
        TArray<FVector> PatrollingPoints;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Move params|Accurency")
        float MovementAccuracy;

    int32 CurrentPatrolPointIndex = INDEX_NONE;

protected:
    virtual void BeginPlay() override;

    void Initalize();

    virtual void Tick(float DeltaTime) override;

    float GetRotationValue();
};
