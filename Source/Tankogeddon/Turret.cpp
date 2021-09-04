// Fill out your copyright notice in the Description page of Project Settings.


#include "Turret.h"
#include <Components/StaticMeshComponent.h>
#include <Components/ArrowComponent.h>
#include <Components/BoxComponent.h>
#include <GameFramework/Controller.h>
#include <Kismet/KismetMathLibrary.h>

#include "Tankogeddon.h"
#include "Cannon.h"
#include "HealthComponent.h"

// Sets default values
ATurret::ATurret()
{
    UStaticMesh* TurretMeshTemp = LoadObject<UStaticMesh>(this, *TurretMeshPath);
    if (TurretMeshTemp)
    {
        TurretMesh->SetStaticMesh(TurretMeshTemp);
    }

    UStaticMesh* BodyMeshTemp = LoadObject<UStaticMesh>(this, *BodyMeshPath);
    if (BodyMeshTemp)
    {
        BodyMesh->SetStaticMesh(BodyMeshTemp);
    }
}

int32 ATurret::GetScores() const
{
    return DestructionScores;
}