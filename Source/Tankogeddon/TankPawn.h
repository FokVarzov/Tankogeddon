#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "HealthComponent.h"
#include "DamageTaker.h"
#include "TankPawn.generated.h"

class UStaticMeshComponent;
class UCameraComponent;
class USpringArmComponent;
class ATankPlayerController;
class ACannon;
class UArrowComponent;
class UBoxComponent;
class UHealthComponent;

UCLASS()
class TANKOGEDDON_API ATankPawn : public APawn, public IDamageTaker
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UStaticMeshComponent* BodyMesh;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UStaticMeshComponent* TurretMesh;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UArrowComponent* CannonSetupPoint;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UHealthComponent* HealthComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UBoxComponent* HitCollider;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Turret|Cannon")
		TSubclassOf<ACannon> CannonClass;
		
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Speed")
		float MoveSpeed = 300.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Speed")
		float MovementSmootheness = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Speed")
		float TurretRotationSpeed = 100.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Speed")
		float RotationSpeed = 50.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Speed")
		float RotationSmootheness = 1.f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		USpringArmComponent* SpringArm;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UCameraComponent* Camera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Speed")
		float InterpolationKey = 0.1f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Turret|Speed")
		float TurretRotationInterpolationKey = 0.5f;

	
	
	 

public:
	// Sets default values for this pawn's properties 
	ATankPawn();

	UFUNCTION()
		void MoveForward(float AxisValue);
	    void RotateRight(float AxisValue);

	UFUNCTION()
		virtual void TakeDamage(FDamageData DamageData) override;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

	void SetupCannon();

	UFUNCTION()
		void Die();

	UFUNCTION()
		void DamageTaken(float InDamage);


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void Fire();
	UFUNCTION()
		void FireSpecial();
	
	UFUNCTION()
	void SetupCannon(TSubclassOf<ACannon> InCannonClass);

	UFUNCTION()
		void CycleCannon();

	UFUNCTION()
		ACannon* GetActiveCannon() const;

	


protected:

	float TargetForwardAxisValue;
	float TargetRightAxisValue;
	float CurrentRightAxisValue;
	float CurrentForwardAxisValue;

	UPROPERTY()
		ATankPlayerController* TankController;

	UPROPERTY()
		ACannon* ActiveCannon;

	UPROPERTY()
		ACannon* InactiveCannon;
};

