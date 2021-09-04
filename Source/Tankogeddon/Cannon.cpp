// Fill out your copyright notice in the Description page of Project Settings.


#include "Cannon.h"
#include <Components/SceneComponent.h>
#include <Components/StaticMeshComponent.h>
#include <Components/ArrowComponent.h>
#include <Engine/Engine.h>
#include <TimerManager.h>
#include <Engine/World.h>

#include "Tankogeddon.h"
#include "Projectile.h"
#include <DrawDebugHelpers.h>
#include "ActorPoolSubsystem.h"
#include "DamageTaker.h"
#include <Particles/ParticleSystemComponent.h>
#include <Components/AudioComponent.h>
#include <Camera/CameraShake.h>
#include <GameFramework/ForceFeedbackEffect.h>

// Sets default values
ACannon::ACannon()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;

    USceneComponent* RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = RootComp;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cannon mesh"));
    Mesh->SetupAttachment(RootComponent);
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    ProjectileSpawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("Spawn point"));
    ProjectileSpawnPoint->SetupAttachment(Mesh);

    ShootEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Shoot effect"));
    ShootEffect->SetupAttachment(ProjectileSpawnPoint);

    ShotAudioEffect = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio effect"));
    ShotAudioEffect->SetupAttachment(ProjectileSpawnPoint);
}

void ACannon::Fire()
{
    if (!IsReadyToFire())
    {
        return;
    }

    bReadyToFire = false;
    --NumAmmo;
    ShotsLeft = NumShotsInSeries;
    Shot();

    ShootEffect->ActivateSystem();
    ShotAudioEffect->Play();

    if (GetOwner() && GetOwner() == GetWorld()->GetFirstPlayerController()->GetPawn())
    {
        if (ShootForceEffect)
        {
            FForceFeedbackParameters ShootForceEffectParams;
            ShootForceEffectParams.bLooping = false;
            ShootForceEffectParams.Tag = "ShootForceEffectParams";
            GetWorld()->GetFirstPlayerController()->ClientPlayForceFeedback(ShootForceEffect, ShootForceEffectParams);
        }

        if (ShootShake)
        {
            GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(ShootShake);
        }
    }


    UE_LOG(LogTankogeddon, Log, TEXT("Fire! Ammo left: %d"), NumAmmo);
}

void ACannon::FireSpecial()
{
    if (!HasSpecialFire() || !IsReadyToFire())
    {
        return;
    }

    bReadyToFire = false;
    --NumAmmo;

    if (Type == ECannonType::FireProjectile)
    {
        GEngine->AddOnScreenDebugMessage(10, 1, FColor::Green, TEXT("Fire special - projectile"));
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(10, 1, FColor::Green, TEXT("Fire special - trace"));
    }

    GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &ACannon::Reload, 1.f / FireRate, false);
    UE_LOG(LogTankogeddon, Log, TEXT("FireSpecial! Ammo left: %d"), NumAmmo);
}

bool ACannon::IsReadyToFire() const
{
    return bReadyToFire && NumAmmo > 0 && ShotsLeft == 0;
}

bool ACannon::HasSpecialFire() const
{
    return bHasSpecialFire;
}

void ACannon::SetVisibility(bool bIsVisible)
{
    Mesh->SetHiddenInGame(!bIsVisible);
}

void ACannon::AddAmmo(int32 InNumAmmo)
{
    NumAmmo = FMath::Clamp(NumAmmo + InNumAmmo, 0, MaxAmmo);
    UE_LOG(LogTankogeddon, Log, TEXT("AddAmmo(%d)! NumAmmo: %d"), InNumAmmo, NumAmmo);
}

// Called when the game starts or when spawned
void ACannon::BeginPlay()
{
    Super::BeginPlay();

    bReadyToFire = true;
    ShotsLeft = 0;
    NumAmmo = MaxAmmo;
}

void ACannon::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    GetWorld()->GetTimerManager().ClearTimer(ReloadTimerHandle);
    GetWorld()->GetTimerManager().ClearTimer(SeriesTimerHandle);
}

void ACannon::Reload()
{
    bReadyToFire = true;
}

void ACannon::Shot()
{
    check(ShotsLeft > 0)
        if (Type == ECannonType::FireProjectile)
        {
            GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1, FColor::Green, TEXT("Fire - projectile"));

            UActorPoolSubsystem* Pool = GetWorld()->GetSubsystem<UActorPoolSubsystem>();
            FTransform SpawnTransform(ProjectileSpawnPoint->GetComponentRotation(), ProjectileSpawnPoint->GetComponentLocation(), FVector::OneVector);
            AProjectile* Projectile = Cast<AProjectile>(Pool->RetreiveActor(ProjectileClass, SpawnTransform));
            if (Projectile)
            {
                Projectile->SetInstigator(GetInstigator());
                Projectile->OnDestroyedTarget.AddUObject(this, &ACannon::NotifyTargetDestroyed);
                Projectile->Start();
            }
        }
        else
        {
            GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1, FColor::Green, TEXT("Fire - trace"));

            FHitResult HitResult;
            FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("FireTrace")), true, this);
            TraceParams.bTraceComplex = true;
            TraceParams.bReturnPhysicalMaterial = false;

            FVector Start = ProjectileSpawnPoint->GetComponentLocation();
            FVector End = ProjectileSpawnPoint->GetForwardVector() * FireRange + Start;
            if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility, TraceParams))
            {
                DrawDebugLine(GetWorld(), Start, HitResult.Location, FColor::Red, false, 0.5f, 0, 5);
                bool bWasTargetDestroyed = false;
                if (HitResult.Component.IsValid() && HitResult.Component->GetCollisionObjectType() == ECollisionChannel::ECC_Destructible)
                {
                    HitResult.Actor.Get()->Destroy();
                    bWasTargetDestroyed = true;
                }
                else if (IDamageTaker* DamageTaker = Cast<IDamageTaker>(HitResult.Actor))
                {
                    AActor* MyInstigator = GetInstigator();
                    if (HitResult.Actor != MyInstigator)
                    {
                        FDamageData DamageData;
                        DamageData.DamageValue = FireDamage;
                        DamageData.DamageMaker = this;
                        DamageData.Instigator = MyInstigator;
                        bWasTargetDestroyed = DamageTaker->TakeDamage(DamageData);
                    }
                }

                if (bWasTargetDestroyed)
                {
                    NotifyTargetDestroyed(HitResult.Actor.Get());
                }
            }
            else
            {
                DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 0.5f, 0, 5);
            }

        }

    if (--ShotsLeft > 0)
    {
        const float NextShotTime = SeriesLength / (NumShotsInSeries - 1);
        GetWorld()->GetTimerManager().SetTimer(SeriesTimerHandle, this, &ACannon::Shot, NextShotTime, false);
    }
    else
    {
        GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &ACannon::Reload, 1.f / FireRate, false);
    }
}

void ACannon::NotifyTargetDestroyed(AActor* Target)
{
    if (OnDestroyedTarget.IsBound())
    {
        OnDestroyedTarget.Broadcast(Target);
    }
}