
#include "Cannon.h"
#include "Components/ArrowComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "TimerManager.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

ACannon::ACannon()
{
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = RootComp;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cannon mesh"));
	Mesh->SetupAttachment(RootComponent);

	ProjectileSpawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("Spawn point"));
	ProjectileSpawnPoint->SetupAttachment(Mesh);

	LoadedAmmo = 10;
	AmmoPool = 10;
}

void ACannon::Fire()
{
	if (LoadedAmmo <= 0)
	{
		return;
	}

	LoadedAmmo = LoadedAmmo - 1;

	if (!bReadyToFire)
	{
		return;
	}
	bReadyToFire = false;

	if (Type == ECannonType::FireProjectile)
	{
		GEngine->AddOnScreenDebugMessage(10, 1, FColor::Green, "Fire - projectile");
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(10, 1, FColor::Green, "Fire - trace" );
	}

	GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &ACannon::Reload, 0.3 / FireRate, false);
}

void ACannon::FireSpecial()
{
	
	if (!bReadyToFire)
	{
		return;
	}
	bReadyToFire = false;

	if (Type == ECannonType::FireProjectile)
	{
		GEngine->AddOnScreenDebugMessage(10, 1, FColor::Yellow, "Fire - BigBadaBoom");
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(10, 1, FColor::Yellow, "Fire - beam");
	}

	GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &ACannon::Reload, 2 / FireRate, false);
}


bool ACannon::IsReadyToFire()
{
	return bReadyToFire;
}



void ACannon::BeginPlay()
{
	Super::BeginPlay();
	Reload();
}
/*void ACannon::EndPlay()
{
	Super::EndPlay(EndPlayReason);
	GetWorld()->GetTimerManager().ClearTimer(ReloadTimerHandle);
}
*/

void ACannon::Reload()
{
	bReadyToFire = true;
}