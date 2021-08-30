
#include "Cannon.h"
#include "Components/ArrowComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "TimerManager.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Projectile.h"
#include "DrawDebugHelpers.h"
#include "ActorPoolSubsystem.h"
#include "HealthComponent.h"
#include "DamageTaker.h"


ACannon::ACannon()
{
	PrimaryActorTick.bCanEverTick = false;
	USceneComponent* RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = RootComp;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cannon mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileSpawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("Spawn point"));
	ProjectileSpawnPoint->SetupAttachment(Mesh);
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
		
	//UE_LOG(LogTankogeddon, Log, TEXT("Fire! Ammo left: %d"), NumAmmo);
}

void ACannon::FireSpecial()
{
	if (!bHasSpecialFire || !IsReadyToFire())
	{
		return;
	}
		
	bReadyToFire = false; 

	if (Type == ECannonType::FireProjectile)
	{
		GEngine->AddOnScreenDebugMessage(10, 1, FColor::Yellow,TEXT ("Fire - RapidFire"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(10, 1, FColor::Yellow,TEXT ("Fire - beam"));
	}

	GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &ACannon::Reload, 1.f / FireRate, false);
}


bool ACannon::IsReadyToFire()const
{
	return bReadyToFire && NumAmmo > 00 && ShotsLeft == 0;
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
	//UE_LOG(LogTankogeddon, Log, TEXT("AddAmmo(%d)! NumAmmo: %d"), InNumAmmo, NumAmmo);
}


void ACannon::BeginPlay()
{
	Super::BeginPlay();

	bReadyToFire = true;
	NumAmmo = MaxAmmo;
	ShotsLeft = 0;
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
				Projectile->Start();
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1, FColor::Green, TEXT("Fire - trace"));
			FHitResult HitResult;
			FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("Firetrace")), true, this);
			TraceParams.bTraceComplex = true;
			TraceParams.bReturnPhysicalMaterial = false;

			FVector start = ProjectileSpawnPoint->GetComponentLocation();
			FVector end = ProjectileSpawnPoint->GetForwardVector() * FireRange + start;
			if (GetWorld()->LineTraceSingleByChannel(HitResult, start, end, ECollisionChannel::ECC_Visibility, TraceParams))
			{
				DrawDebugLine(GetWorld(), start, HitResult.Location, FColor::Red, false, 0.5f, 0, 5);
				if (HitResult.Component.IsValid() && HitResult.Component->GetCollisionObjectType() == ECollisionChannel::ECC_Destructible)
				{
					
					HitResult.Actor.Get() -> Destroy();
				}
				
			}

			else DrawDebugLine(GetWorld(), start, end, FColor::Red, false, 0.5f, 0, 5);
		}   

	    if (--ShotsLeft > 0)
	    {
			const float NextShotTime = SeriesLenght / (NumShotsInSeries - 1);
			GetWorld()->GetTimerManager().SetTimer(SeriesTimerHandle, this, &ACannon::Shot, NextShotTime, false);
		}
		else
		{
			GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &ACannon::Reload, 1.f / FireRate, false);
		}

	
}
