// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DamageTaker.h"
#include "BasePawn.h"
#include "TankPawn.generated.h"



class USpringArmComponent;
class UCameraComponent;
class ATankPlayerController;
class ATargetPoint;


UCLASS()
class TANKOGEDDON_API ATankPawn : public ABasePawn
{
    GENERATED_BODY()

public:
    // Sets default values for this pawn's properties
    ATankPawn();

protected:
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
        USpringArmComponent* SpringArm;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
        UCameraComponent* Camera;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Speed")
        float MoveSpeed = 100.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Speed")
        float MovementSmootheness = 0.1f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Speed")
        float RotationSpeed = 100.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Speed")
        float RotationSmootheness = 0.1f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Move params|Patrol points", Meta = (MakeEditWidget = true))
        TArray<ATargetPoint*> PatrollingPoints;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Move params|Accurency")
        float MovementAccuracy = 50.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
        UForceFeedbackEffect* HitForceEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
        TSubclassOf<UMatineeCameraShake> HitShake;

    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    virtual void TargetDestroyed(AActor* Target) override;

    virtual void DamageTaken(float DamageValue) override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    UFUNCTION()
        void MoveForward(float AxisValue);

    UFUNCTION()
        void RotateRight(float AxisValue);

    UFUNCTION()
        TArray<FVector> GetPatrollingPoints();

    UFUNCTION()
        void SetPatrollingPoints(const TArray<ATargetPoint*>& NewPatrollingPoints);

    UFUNCTION()
        float GetMovementAccurency()
    {
        return MovementAccuracy;
    };

private:
    float TargetForwardAxisValue = 0.f;
    float CurrentForwardAxisValue = 0.f;
    float TargetRightAxisValue = 0.f;
    float CurrentRightAxisValue = 0.f;

    int32 AccumulatedScores = 0;
};