// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include <Components/SceneComponent.h>
#include <Components/StaticMeshComponent.h>
#include <TimerManager.h>
#include <Engine/World.h>

#include "Tankogeddon.h"
#include "ActorPoolSubsystem.h"
#include "DamageTaker.h"
#include "GameStructs.h"

// Sets default values
AProjectile::AProjectile()
{
    PrimaryActorTick.bCanEverTick = false;
    USceneComponent* SceeneCpm = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = SceeneCpm;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);
    Mesh->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnMeshOverlapBegin);
    Mesh->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
    Mesh->SetHiddenInGame(true);
}

void AProjectile::Start()
{
    GetWorld()->GetTimerManager().SetTimer(MovementTimerHandle, this, &AProjectile::Move, MoveRate, true, MoveRate);
    StartLocation = GetActorLocation();
    Mesh->SetHiddenInGame(false);
    Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AProjectile::Stop()
{
    OnDestroyedTarget.Clear();
    GetWorld()->GetTimerManager().ClearTimer(MovementTimerHandle);
    Mesh->SetHiddenInGame(true);
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    UActorPoolSubsystem* Pool = GetWorld()->GetSubsystem<UActorPoolSubsystem>();
    if (Pool->IsActorInPool(this))
    {
        Pool->ReturnActor(this);
    }
    else
    {
        Destroy();
    }
}

void AProjectile::OnMeshOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    UE_LOG(LogTankogeddon, Warning, TEXT("Projectile %s collided with %s. "), *GetName(), *OtherActor->GetName());
    if (OtherActor == GetInstigator())
    {
        return;
    }

    bool bWasTargetDestroyed = false;
    if (OtherComp && OtherComp->GetCollisionObjectType() == ECollisionChannel::ECC_Destructible)
    {
        OtherActor->Destroy();
        bWasTargetDestroyed = true;
    }
    else if (IDamageTaker* DamageTaker = Cast<IDamageTaker>(OtherActor))
    {
        FDamageData DamageData;
        DamageData.DamageValue = Damage;
        DamageData.DamageMaker = this;
        DamageData.Instigator = GetInstigator();
        bWasTargetDestroyed = DamageTaker->TakeDamage(DamageData);
    }

    if (bWasTargetDestroyed && OnDestroyedTarget.IsBound())
    {
        OnDestroyedTarget.Broadcast(OtherActor);
    }

    Stop();
}

void AProjectile::Move()
{
    FVector NextPosition = GetActorLocation() + GetActorForwardVector() * MoveSpeed * MoveRate;
    SetActorLocation(NextPosition);
    if (FVector::Distance(NextPosition, StartLocation) > FlyRange)
    {
        Stop();
    }
}