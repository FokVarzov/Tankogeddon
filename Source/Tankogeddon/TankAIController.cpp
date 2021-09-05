// Fill out your copyright notice in the Description page of Project Settings.


#include "TankAIController.h"
#include "TankPawn.h"
#include <DrawDebugHelpers.h>
#include <Engine/World.h>

void ATankAIController::BeginPlay()
{
    Super::BeginPlay();
    Initalize();
}

void ATankAIController::Initalize()
{
    TankPawn = Cast<ATankPawn>(GetPawn());

    if (TankPawn)
    {
        MovementAccuracy = TankPawn->GetMovementAccurency();
        PatrollingPoints = TankPawn->GetPatrollingPoints();
        CurrentPatrolPointIndex = PatrollingPoints.Num() == 0 ? INDEX_NONE : 0;
    }
}

void ATankAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!TankPawn)
    {
        Initalize();
    }

    if (!TankPawn)
    {
        return;
    }

    if (CurrentPatrolPointIndex == INDEX_NONE)
    {
        TankPawn->MoveForward(0.f);
        return;
    }

    TankPawn->MoveForward(1.f);

    float RotationValue = GetRotationValue();
    //UE_LOG(LogTemp, Warning, TEXT("AI Rotation forwardAngle: %f rightAngle: %f rotationValue: %f"), forwardAngle, rightAngle, rotationValue);
    TankPawn->RotateRight(RotationValue);
}

float ATankAIController::GetRotationValue()
{
    FVector CurrentPoint = PatrollingPoints[CurrentPatrolPointIndex];
    FVector PawnLocation = TankPawn->GetActorLocation();
    if (FVector::Distance(CurrentPoint, PawnLocation) <= MovementAccuracy)
    {
        CurrentPatrolPointIndex++;
        if (CurrentPatrolPointIndex >= PatrollingPoints.Num())
        {
            CurrentPatrolPointIndex = 0;
        }
    }

    FVector MoveDirection = CurrentPoint - PawnLocation;
    MoveDirection.Normalize();
    FVector ForwardDirection = TankPawn->GetActorForwardVector();
    FVector RightDirection = TankPawn->GetActorRightVector();

    DrawDebugLine(GetWorld(), PawnLocation, CurrentPoint, FColor::Green, false, 0.1f, 0, 5);

    float ForwardAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(ForwardDirection, MoveDirection)));
    float RightAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(RightDirection, MoveDirection)));

    float RotationValue = 0;
    if (ForwardAngle > 5.f)
    {
        RotationValue = 1.f;
    }
    if (RightAngle > 90.f)
    {
        RotationValue = -RotationValue;
    }

    return RotationValue;
}