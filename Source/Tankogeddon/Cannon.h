#pragma once

#include "CoreMinimal.h"
#include "GameStructs.h"
#include "GameFramework/Actor.h"
#include "Cannon.generated.h"

class UStaticMechComponent;
class UArrowComponent;

UCLASS()
class TANKOGEDDON_API ACannon : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UStaticMeshComponent* Mesh;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UArrowComponent* ProjectileSpawnPoint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Fire params")
		float FireRate = 1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Fire params")
		float FireRange = 1000;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Fire params")
		float FireDamage = 1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Fire params")
		ECannonType Type = ECannonType::FireProjectile;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ammo")
		float LoadedAmmo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ammo")
		float AmmoPool;




	FTimerHandle ReloadTimerHandle;

	bool bReadyToFire = false;

public:
	ACannon();

	void Fire();

	void FireSpecial();

    

	bool IsReadyToFire();

protected:
	virtual void BeginPlay() override;
	//virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

// Reload your weapon
	void Reload();
};
