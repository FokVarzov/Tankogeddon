// Fill out your copyright notice in the Description page of Project Settings.


#include "TankFactory.h"
#include <Components/SceneComponent.h>
#include <Components/StaticMeshComponent.h>
#include <Components/ArrowComponent.h>
#include <Components/BoxComponent.h>
#include "Tankogeddon.h"
#include "HealthComponent.h"
#include "TankPawn.h"
#include <Kismet/GameplayStatics.h>
#include "MapLoader.h"

// Sets default values
ATankFactory::ATankFactory()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;

    USceneComponent* SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = SceneComp;

    BuildingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Building Mesh"));
    BuildingMesh->SetupAttachment(SceneComp);

    TankSpawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("Cannon setup point"));
    TankSpawnPoint->AttachToComponent(SceneComp, FAttachmentTransformRules::KeepRelativeTransform);

    HitCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Hit collider"));
    HitCollider->SetupAttachment(SceneComp);

    HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health component"));
    HealthComponent->OnDie.AddDynamic(this, &ATankFactory::Die);
    HealthComponent->OnDamaged.AddDynamic(this, &ATankFactory::DamageTaked);
}

bool ATankFactory::TakeDamage(FDamageData DamageData)
{
    return HealthComponent->TakeDamage(DamageData);
}

// Called when the game starts or when spawned
void ATankFactory::BeginPlay()
{
    Super::BeginPlay();

    if (LinkedMapLoader)
    {
        LinkedMapLoader->SetIsActivated(false);
    }

    FTimerHandle TargetingTimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TargetingTimerHandle, this, &ATankFactory::SpawnNewTank, SpawnTankRate, true);
}

void ATankFactory::SpawnNewTank()
{
    FTransform SpawnTransform(TankSpawnPoint->GetComponentRotation(), TankSpawnPoint->GetComponentLocation(), FVector(1.f));
    ATankPawn* NewTank = GetWorld()->SpawnActorDeferred<ATankPawn>(SpawnTankClass, SpawnTransform, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
    //
    NewTank->SetPatrollingPoints(TankWayPoints);
    //
    UGameplayStatics::FinishSpawningActor(NewTank, SpawnTransform);
}

void ATankFactory::Die()
{
    if (LinkedMapLoader)
    {
        LinkedMapLoader->SetIsActivated(true);
    }
    Destroy();
}

void ATankFactory::DamageTaked(float DamageValue)
{
    UE_LOG(LogTankogeddon, Log, TEXT("Factory %s taked damage:%f Health:%f"), *GetName(), DamageValue, HealthComponent->GetHealth());
}
