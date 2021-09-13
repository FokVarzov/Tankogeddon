// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePawn.h"
#include <Components/StaticMeshComponent.h>
#include <Components/ArrowComponent.h>
#include <Components/BoxComponent.h>
#include "HealthComponent.h"
#include "Tankogeddon.h"
#include "Cannon.h"
#include <Kismet/KismetMathLibrary.h>
#include <Kismet/GameplayStatics.h>
#include "AmmoBox.h"

// Sets default values
ABasePawn::ABasePawn()
{
    // Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body Mesh"));
    BodyMesh->bEditableWhenInherited = true;
    RootComponent = BodyMesh;

    TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Turret Mesh"));
    TurretMesh->SetupAttachment(BodyMesh);
    TurretMesh->bEditableWhenInherited = true;

    CannonSetupPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("Cannon setup point"));
    CannonSetupPoint->AttachToComponent(TurretMesh, FAttachmentTransformRules::KeepRelativeTransform);
    CannonSetupPoint->bEditableWhenInherited = true;

    HitCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Hit collider"));
    HitCollider->SetupAttachment(BodyMesh);
    HitCollider->bEditableWhenInherited = true;

    HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health Component"));
    HealthComponent->OnDie.AddDynamic(this, &ABasePawn::Die);
    HealthComponent->OnDamaged.AddDynamic(this, &ABasePawn::DamageTaken);
    HealthComponent->bEditableWhenInherited = true;

    //ForceStrength = 1000.0f;
}

// Called when the game starts or when spawned
void ABasePawn::BeginPlay()
{
    Super::BeginPlay();

    SetupCannon(CannonClass);
}

void ABasePawn::Destroyed()
{
    if (ActiveCannon)
    {
        ActiveCannon->Destroy();
    }

    if (InactiveCannon)
    {
        InactiveCannon->Destroy();
    }
}

void ABasePawn::TargetDestroyed(AActor* Target)
{

}

void ABasePawn::Fire()
{
    if (ActiveCannon)
    {
        ActiveCannon->Fire();
    }
}

void ABasePawn::FireSpecial()
{
    if (ActiveCannon)
    {
        ActiveCannon->FireSpecial();
    }
}

void ABasePawn::SetupCannon(TSubclassOf<ACannon> InCannonClass)
{
    if (ActiveCannon)
    {
        ActiveCannon->Destroy();
        ActiveCannon = nullptr;
    }

    FActorSpawnParameters Params;
    Params.Instigator = this;
    Params.Owner = this;
    ActiveCannon = GetWorld()->SpawnActor<ACannon>(InCannonClass, Params);
    ActiveCannon->AttachToComponent(CannonSetupPoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
    ActiveCannon->OnDestroyedTarget.AddUObject(this, &ABasePawn::TargetDestroyed);
}

void ABasePawn::CycleCannon()
{
    Swap(ActiveCannon, InactiveCannon);
    if (ActiveCannon)
    {
        ActiveCannon->SetVisibility(true);
    }

    if (InactiveCannon)
    {
        InactiveCannon->SetVisibility(false);
    }
}

ACannon* ABasePawn::GetActiveCannon() const
{
    return ActiveCannon;
}

bool ABasePawn::TakeDamage(FDamageData DamageData)
{
    return HealthComponent->TakeDamage(DamageData);
}

// Called every frame
void ABasePawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

  //  FVector Right = BodyMesh->GetRightVector();
   // BodyMesh->AddForce(Right * ForceStrength * BodyMesh->GetMass());

    if (bIsTurretTargetSet)
    {
        FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TurretTarget);
        FRotator CurrRotation = TurretMesh->GetComponentRotation();
        TargetRotation.Pitch = CurrRotation.Pitch;
        TargetRotation.Roll = CurrRotation.Roll;
        TurretMesh->SetWorldRotation(FMath::RInterpConstantTo(CurrRotation, TargetRotation, DeltaTime, TurretRotationSpeed));
    }
    else
    {
        FRotator NewRotation = TurretMesh->GetComponentRotation();
        NewRotation.Yaw += TurretRotationSpeed * DeltaTime * TurretRotationAxis;
        TurretMesh->SetWorldRotation(NewRotation);
    }
}

FVector ABasePawn::GetTurretForwardVector()
{
    return TurretMesh->GetForwardVector();
}

void ABasePawn::SetTurretRotationAxis(float AxisValue)
{
    TurretRotationAxis = AxisValue;
    bIsTurretTargetSet = false;
}

void ABasePawn::SetTurretTarget(FVector TargetPosition)
{
    TurretTarget = TargetPosition;
    bIsTurretTargetSet = true;
}

FVector ABasePawn::GetEyesPosition()
{
    return CannonSetupPoint->GetComponentLocation();
}

void ABasePawn::Die()
{
    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DestuctionParticleSystem, GetActorTransform());
    UGameplayStatics::PlaySoundAtLocation(GetWorld(), DestructionSound, GetActorLocation());
    if (DestructionBonusBox)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.bNoFail = true;
        GetWorld()->SpawnActor<AAmmoBox>(DestructionBonusBox, GetActorTransform(), SpawnParams);
    }

    Destroy();
}

void ABasePawn::DamageTaken(float InDamage)
{
    
    
    UE_LOG(LogTankogeddon, Log, TEXT("Pawn %s taken damage:%f "), *GetName(), InDamage);
}