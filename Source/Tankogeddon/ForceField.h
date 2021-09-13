// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ForceField.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class UPointLightComponent;
class UPrimitiveComponent;

UCLASS()
class TANKOGEDDON_API AForceField : public AActor
{
    GENERATED_BODY()

protected:
   
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
        UStaticMeshComponent* BuildingMesh;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
        UStaticMeshComponent* DestroyedMesh;

   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InactiveGate")
        bool IsActivated = false;

   UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
       UBoxComponent* TriggerCollider;

public:
    // Sets default values for this actor's properties
    AForceField();

    void SetIsActivated(bool NewIsActivated);

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    void SetActiveMesh();

    UFUNCTION()
        void OnTriggerOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


};