// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameStructs.h"
#include "Cannon.generated.h"

class UStaticMeshComponent;
class UArrowComponent;
class AProjectile;
class UParticleSystemComponent;
class UAudioComponent;
class UForceFeedbackEffect;
class UMatineeCameraShake;


UCLASS()
class TANKOGEDDON_API ACannon : public AActor
{
    GENERATED_BODY()

protected:
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
        UStaticMeshComponent* Mesh;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
        UArrowComponent* ProjectileSpawnPoint;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
        UParticleSystemComponent* ShootEffect;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
        UAudioComponent* ShotAudioEffect;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Fire params")
        TSubclassOf<AProjectile> ProjectileClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
        UForceFeedbackEffect* ShootForceEffect;

    UPROPERTY(EditAnywhere)
        TSubclassOf<UMatineeCameraShake> ShootShake;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Fire params")
        float FireRate = 1.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Fire params")
        float FireRange = 1000.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Fire params")
        int32 MaxAmmo = 10;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (ClampMin = 1), Category = "Fire params")
        int32 NumShotsInSeries = 1;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (EditCondition = "NumShotsInSeries > 1", EditConditionHides), Category = "Fire params")
        float SeriesLength = 1.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Fire params")
        float FireDamage = 1.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Fire params")
        bool bHasSpecialFire = false;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Fire params")
        ECannonType Type = ECannonType::FireProjectile;

    FTimerHandle ReloadTimerHandle;
    FTimerHandle SeriesTimerHandle;

    bool bReadyToFire = false;

public:
    ACannon();

    void Fire();
    void FireSpecial();

    bool IsReadyToFire() const;
    bool HasSpecialFire() const;

    void SetVisibility(bool bIsVisible);
    void AddAmmo(int32 InNumAmmo);

    DECLARE_MULTICAST_DELEGATE_OneParam(FOnDestoyedTarget, AActor*);
    FOnDestoyedTarget OnDestroyedTarget;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    void Reload();
    void Shot();

    void NotifyTargetDestroyed(AActor* Target);

    int32 NumAmmo = 0;
    int32 ShotsLeft = 0;
};