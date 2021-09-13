// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class UStaticMeshComponent;

UCLASS()
class TANKOGEDDON_API AProjectile : public AActor
{
    GENERATED_BODY()

protected:
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
        UStaticMeshComponent* Mesh;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Speed")
        float MoveSpeed = 100.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Speed")
        float MoveRate = 0.005f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Speed")
        float FlyRange = 1000.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage")
        float Damage = 1.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage")
        float PushForce = 1000.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Trajectory")
        float ExplodeRadius = 50.f;

    FTimerHandle MovementTimerHandle;
    FVector StartLocation;

public:
    AProjectile();

    virtual void Start();
    void Stop();
    void Explode();

    DECLARE_MULTICAST_DELEGATE_OneParam(FOnDestoyedTarget, AActor*);
    FOnDestoyedTarget OnDestroyedTarget;

protected:
    UFUNCTION()
        void OnMeshOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
        virtual void Move();


};