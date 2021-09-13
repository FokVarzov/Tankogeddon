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
#include <Components/PrimitiveComponent.h>

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

    else
    {
        UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(OtherComp);
        if (PrimComp && PrimComp->IsSimulatingPhysics())
        {
            FVector ForceVector = GetActorForwardVector();
            PrimComp->AddImpulseAtLocation(ForceVector * PushForce, SweepResult.ImpactPoint);
        }
    }

    if (bWasTargetDestroyed && OnDestroyedTarget.IsBound())
    {
        OnDestroyedTarget.Broadcast(OtherActor);
    }

    Explode();
    Stop();
}

void AProjectile::Explode()
{
    FVector StartPos = GetActorLocation();
    FVector EndPos = StartPos + FVector(0.1f);

    FCollisionShape Shape = FCollisionShape::MakeSphere(ExplodeRadius);
    FCollisionQueryParams Params = FCollisionQueryParams::DefaultQueryParam;
    Params.AddIgnoredActor(this);
    Params.bTraceComplex = true;
    Params.TraceTag = "Explode Trace";
    TArray<FHitResult> AttackHit;

    FQuat Rotation = FQuat::Identity;

    bool SweepResult = GetWorld()->SweepMultiByChannel
    (
        AttackHit,
        StartPos,
        EndPos,
        Rotation,
        ECollisionChannel::ECC_Visibility,
        Shape,
        Params
    );

    GetWorld()->DebugDrawTraceTag = "Explode Trace";

    if (SweepResult)
    {
        for (FHitResult HitResult : AttackHit)
        {
            AActor* OtherActor = HitResult.GetActor();
            if (!OtherActor)
            {
                continue;
            }

            IDamageTaker* DamageTakerActor = Cast<IDamageTaker>(OtherActor);
            if (DamageTakerActor)
            {
                FDamageData DamageData;
                DamageData.DamageValue = Damage;
                DamageData.Instigator = GetOwner();
                DamageData.DamageMaker = this;

                DamageTakerActor->TakeDamage(DamageData);
            }
            else
            {
                UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(OtherActor->GetRootComponent());
                if (PrimComp)
                {
                    if (PrimComp->IsSimulatingPhysics())
                    {
                        FVector ForceVector = OtherActor->GetActorLocation() - GetActorLocation();
                        ForceVector.Normalize();
                        PrimComp->AddImpulse(ForceVector * PushForce, NAME_None, true);
                    }
                }
            }

        }
    }

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
