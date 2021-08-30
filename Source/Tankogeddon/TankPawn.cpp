

#include "TankPawn.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/ArrowComponent.h"
#include <Components/BoxComponent.h>

#include "TankPlayerController.h"
#include "Tankogeddon.h"
#include "Cannon.h"
#include "HealthComponent.h"


// Sets default values
ATankPawn::ATankPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tank body"));
	RootComponent = BodyMesh;

	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tank turret"));
	TurretMesh->SetupAttachment(BodyMesh);

	CannonSetupPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("Cannon setup point"));
	CannonSetupPoint->AttachToComponent(TurretMesh, FAttachmentTransformRules::KeepRelativeTransform);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring arm"));
	SpringArm->SetupAttachment(BodyMesh);
	SpringArm->bDoCollisionTest = false;
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritYaw = false;
	SpringArm->bInheritRoll = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health component"));
	HealthComponent->OnDie.AddDynamic(this, &ATankPawn::Die);
	HealthComponent->OnDamaged.AddDynamic(this, &ATankPawn::DamageTaken);

	HitCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Hit collider"));
	HitCollider->SetupAttachment(BodyMesh);
	
}

// Called when the game starts or when spawned
void ATankPawn::BeginPlay()
{
	Super::BeginPlay();
	TankController = Cast<ATankPlayerController>(GetController());
	SetupCannon(CannonClass);
}

void ATankPawn::SetupCannon(TSubclassOf<ACannon> InCannonClass)
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
}

void ATankPawn::CycleCannon()
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

ACannon* ATankPawn::GetActiveCannon() const
{
	return ActiveCannon;
}

// Called every frame
void ATankPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Tank movement
	CurrentForwardAxisValue = FMath::FInterpTo(CurrentForwardAxisValue, TargetForwardAxisValue, DeltaTime, MovementSmootheness);
	FVector currentLocation = GetActorLocation();
	FVector forwardVector = GetActorForwardVector();

	FVector movePosition = currentLocation + forwardVector * CurrentForwardAxisValue * MoveSpeed * DeltaTime;
	
	FHitResult* SweepHitResult = nullptr;
	SetActorLocation(movePosition, true, SweepHitResult);
	if (SweepHitResult)
	{
		CurrentForwardAxisValue = 0.f;
	}

	// Tank rotation
	CurrentRightAxisValue = FMath::FInterpTo(CurrentRightAxisValue, TargetRightAxisValue,DeltaTime, RotationSmootheness);

	UE_LOG(LogTemp, Warning, TEXT("CurrentRightAxisValue = %f TargetRightAxisValue = %f"), 
	CurrentRightAxisValue, TargetRightAxisValue);

	float yawRotation = RotationSpeed * CurrentRightAxisValue * DeltaTime;
	FRotator currentRotation = GetActorRotation();

	yawRotation = currentRotation.Yaw + yawRotation;

	FRotator newRotation = FRotator(0, yawRotation, 0);
	SetActorRotation(newRotation);

	// Turret rotation
	if (TankController)
	{
		FVector mousePos = TankController->GetMousePos();
		FRotator targetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), mousePos);
		FRotator currRotation = TurretMesh->GetComponentRotation();
		targetRotation.Pitch = currRotation.Pitch;
		targetRotation.Roll = currRotation.Roll;
		TurretMesh->SetWorldRotation(FMath::RInterpConstantTo(currRotation, targetRotation,DeltaTime, TurretRotationSpeed));
	}

}


void ATankPawn::MoveForward(float AxisValue)
{
	TargetForwardAxisValue = AxisValue;
}


void ATankPawn::RotateRight(float AxisValue)
{
	TargetRightAxisValue = AxisValue;
}

void ATankPawn::Fire()
{
	if (ActiveCannon)
	{
		ActiveCannon->Fire();
	}

}
void ATankPawn::FireSpecial()
{
	if (ActiveCannon)
	{
		ActiveCannon->FireSpecial();
	}

}

void ATankPawn::Destroyed()
{
	if (ActiveCannon)
	{
		ActiveCannon->Destroy();
	}
}

void ATankPawn::TakeDamage(FDamageData DamageData)
{
	HealthComponent->TakeDamage(DamageData);
}

void ATankPawn::Die()
{
	Destroy();
}

void ATankPawn::DamageTaken(float InDamage)
{
	UE_LOG(LogTankogeddon, Warning, TEXT("Tank %s taked damage:%f Health:%f"), *GetName(), InDamage, HealthComponent->GetHealth());
}
