// Fill out your copyright notice in the Description page of Project Settings.


#include "ForceField.h"
#include <Components/StaticMeshComponent.h>
#include <Components/BoxComponent.h>
#include <Kismet/GameplayStatics.h>
#include <GameFramework/Controller.h>
#include <Engine/World.h>

// Sets default values
AForceField::AForceField()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;

    USceneComponent* SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = SceneComp;

    BuildingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Building Mesh"));
    BuildingMesh->SetupAttachment(SceneComp);

    DestroyedMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Destroyed Mesh"));
    DestroyedMesh->SetupAttachment(SceneComp);
    //DestroyedMesh->SetVisibility(false);

    TriggerCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger collider"));
    TriggerCollider->SetupAttachment(SceneComp);

    TriggerCollider->OnComponentBeginOverlap.AddDynamic(this, &AForceField::OnTriggerOverlapBegin);

    SetActiveMesh();
}

void AForceField::SetIsActivated(bool NewIsActivated)
{
    IsActivated = NewIsActivated;

    SetActiveMesh();
}

// Called when the game starts or when spawned
void AForceField::BeginPlay()
{
    Super::BeginPlay();

    SetActiveMesh();
}

void AForceField::SetActiveMesh()
{
    BuildingMesh->SetHiddenInGame(!IsActivated);
    DestroyedMesh->SetHiddenInGame(IsActivated);
}

void AForceField::OnTriggerOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!IsActivated)
    {
        return;
    }
}
