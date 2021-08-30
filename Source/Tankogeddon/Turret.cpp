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
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;

    BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Turret body"));
    RootComponent = BodyMesh;

    TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Turret turret"));
    TurretMesh->AttachToComponent(BodyMesh, FAttachmentTransformRules::KeepRelativeTransform);

    CannonSetupPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("Cannon setup point"));
    CannonSetupPoint->AttachToComponent(TurretMesh, FAttachmentTransformRules::KeepRelativeTransform);

    HitCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Hit collider"));
    HitCollider->SetupAttachment(TurretMesh);

    HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health Component"));
    HealthComponent->OnDie.AddDynamic(this, &ATurret::Die);
    HealthComponent->OnDamaged.AddDynamic(this, &ATurret::DamageTaken);

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

void ATurret::TakeDamage(FDamageData DamageData)
{
    HealthComponent->TakeDamage(DamageData);
}

// Called when the game starts or when spawned
void ATurret::BeginPlay()
{
    Super::BeginPlay();

    FActorSpawnParameters Params;
    Params.Owner = this;
    Cannon = GetWorld()->SpawnActor<ACannon>(CannonClass, Params);
    Cannon->AttachToComponent(CannonSetupPoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

    PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();

    FTimerHandle TargetingTimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TargetingTimerHandle, this, &ATurret::Targeting, TargetingRate, true, TargetingRate);
}

void ATurret::Destroyed()
{
    if (Cannon)
    {
        Cannon->Destroy();
    }
}

void ATurret::Targeting()
{
    if (IsPlayerInRange())
    {
        RotateToPlayer();
    }

    if (CanFire() && Cannon && Cannon->IsReadyToFire())
    {
        Fire();
    }
}

void ATurret::RotateToPlayer()
{
    FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PlayerPawn->GetActorLocation());
    FRotator CurrRotation = TurretMesh->GetComponentRotation();
    TargetRotation.Pitch = CurrRotation.Pitch;
    TargetRotation.Roll = CurrRotation.Roll;
    TurretMesh->SetWorldRotation(FMath::RInterpConstantTo(CurrRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), TargetingSpeed));
}

bool ATurret::IsPlayerInRange()
{
    return FVector::Distance(PlayerPawn->GetActorLocation(), GetActorLocation()) <= TargetingRange;
}

bool ATurret::CanFire()
{
    FVector TargetingDir = TurretMesh->GetForwardVector();
    FVector DirToPlayer = PlayerPawn->GetActorLocation() - GetActorLocation();
    DirToPlayer.Normalize();
    float AimAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(TargetingDir, DirToPlayer)));
    return AimAngle <= Accurency;
}

void ATurret::Fire()
{
    if (Cannon)
    {
        Cannon->Fire();
    }
}

void ATurret::Die()
{
    Destroy();
}

void ATurret::DamageTaken(float InDamage)
{
    UE_LOG(LogTankogeddon, Warning, TEXT("Turret %s taken damage:%f "), *GetName(), InDamage);
}
