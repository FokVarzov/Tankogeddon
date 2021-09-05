// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePawnAIController.h"
#include "BasePawn.h"

void ABasePawnAIController::BeginPlay()
{
    Super::BeginPlay();

    MyPawn = Cast<ABasePawn>(GetPawn());
    PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
}

void ABasePawnAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!MyPawn || !PlayerPawn)
    {
        MyPawn = Cast<ABasePawn>(GetPawn());
        PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();

        return;
    }


    if (!MyPawn || !PlayerPawn)
    {
        return;
    }

    if (DetectCanFire())
    {
        Fire();
    }
    else
    {
        RotateToPlayer();
    }
}

void ABasePawnAIController::RotateToPlayer()
{
    if (IsPlayerInRange())
    {
        MyPawn->SetTurretTarget(PlayerPawn->GetActorLocation());
    }
}

bool ABasePawnAIController::IsPlayerInRange()
{
    return FVector::Distance(MyPawn->GetActorLocation(), PlayerPawn->GetActorLocation()) <= TargetingRange;
}

bool ABasePawnAIController::DetectCanFire()
{
    if (!DetectPlayerVisibility())
    {
        return false;
    }

    FVector TargetingDir = MyPawn->GetTurretForwardVector();
    FVector DirToPlayer = PlayerPawn->GetActorLocation() - MyPawn->GetActorLocation();
    DirToPlayer.Normalize();
    float AimAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(TargetingDir, DirToPlayer)));
    return AimAngle <= Accuracy;
}

void ABasePawnAIController::Fire()
{
    MyPawn->Fire();
}

bool ABasePawnAIController::DetectPlayerVisibility()
{
    FVector PlayerPos = PlayerPawn->GetActorLocation();
    FVector EyesPos = MyPawn->GetEyesPosition();

    FHitResult HitResult;
    FCollisionQueryParams traceParams = FCollisionQueryParams(FName(TEXT("FireTrace")), true, this);
    traceParams.bTraceComplex = true;
    traceParams.AddIgnoredActor(MyPawn);
    traceParams.bReturnPhysicalMaterial = false;

    if (GetWorld()->LineTraceSingleByChannel(HitResult, EyesPos, PlayerPos, ECollisionChannel::ECC_Visibility, traceParams))
    {
        if (HitResult.Actor.Get())
        {
            // DrawDebugLine(GetWorld(), EyesPos, HitResult.Location, FColor::Cyan, false, 0.5f, 0, 10);
            return HitResult.Actor.Get() == PlayerPawn;
        }
    }

    // DrawDebugLine(GetWorld(), EyesPos, PlayerPos, FColor::Cyan, false, 0.5f, 0, 10);
    return false;
}