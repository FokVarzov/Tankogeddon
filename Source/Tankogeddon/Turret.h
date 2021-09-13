// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DamageTaker.h"
#include "BasePawn.h"
#include "Scorable.h"
#include "Turret.generated.h"

class UStaticMeshComponent;
class UArrowComponent;
class UBoxComponent;
class UHealthComponent;
class ACannon;

UCLASS()
class TANKOGEDDON_API ATurret : public ABasePawn, public IScorable
{
    GENERATED_BODY()

protected:
    const FString BodyMeshPath = "StaticMesh'/Game/CSC/Meshes/SM_CSC_Tower1.SM_CSC_Tower1'";
    const FString TurretMeshPath = "StaticMesh'/Game/CSC/Meshes/SM_CSC_Gun1.SM_CSC_Gun1'";

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Scorable")
        int32 DestructionScores = 5;

public:
    ATurret();

    virtual void PostInitializeComponents() override;

    virtual int32 GetScores() const override;

};
