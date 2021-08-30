#include "Projectile.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "Components/SceneComponent.h"
#include "Engine/World.h"

#include "Tankogeddon.h"
#include "ActorPoolSubsystem.h"
#include "DamageTaker.h"
#include "GameStructs.h"

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
}

void AProjectile::Stop()
{
	GetWorld()->GetTimerManager().ClearTimer(MovementTimerHandle);
	Mesh->SetHiddenInGame(true);

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

void AProjectile::OnMeshOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Projectile %s collided with %s. "), *GetName(), *OtherActor->GetName());
	
	if (OtherComp && OtherComp->GetCollisionObjectType() == ECollisionChannel::ECC_Destructible)
	{
		OtherActor->Destroy();
	}
	else if (IDamageTaker* DamageTaker = Cast<IDamageTaker>(OtherActor))
	{
		AActor* MyInstigator = GetInstigator();
		if (OtherActor != MyInstigator)
		{
			FDamageData DamageData;
			DamageData.DamageValue = Damage;
			DamageData.DamageMaker = this;
			DamageData.Instigator = MyInstigator;
			DamageTaker->TakeDamage(DamageData);
		}
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
