// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SRangedWeapon.generated.h"

class USkeletalMeshComponent;
class UDamageType;
class UParticleSystem;
class UCameraShakeBase;
class UAnimMontage;
class USoundBase;
class USGunAnimationPack;

//A custom struct that contains information of a single Hit Scan Weapon Line trace
USTRUCT()
struct FHitScanTrace
{
	GENERATED_BODY()

public:
	UPROPERTY()
	int8 ForceReplication;
	UPROPERTY()
	TEnumAsByte<EPhysicalSurface> SurfaceType;
	UPROPERTY()
	FVector_NetQuantize TraceTo;
};

UCLASS()
class COOPHORDEGAME_API ASRangedWeapon : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASRangedWeapon();

	virtual void BeginPlay() override;

protected:
	// Called when the game starts or when spawned


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USkeletalMeshComponent* MeshComponent;

	

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		float HitDamage = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		USGunAnimationPack* GunAnimationPack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		UParticleSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		UParticleSystem* DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		UParticleSystem* FleshImpactEffect;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		UParticleSystem* TracerEffect;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		FName MuzzleSocket;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		FName TracerTargetName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		UAnimMontage* ReloadAnimation;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		USoundBase* FireSound;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		USoundBase* ImpactSound;

	void PlayFireEffects(FVector TraceEnd);

	void PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint);

	virtual void Fire();

	//YOU MUST USE CameraShakeBase when setting up camera shake - as UMatineeCame1raShake will fail to convert 
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<UCameraShakeBase> FireCameraShake;

	FTimerHandle TimerHandle_TimeBetweenShots;

	float LastFireTime;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	bool bFullAuto = true;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (ClampMin = 1))
	int32 RateOfFire;
	//Randomised bullet spread in degrees
	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (ClampMin = 0))
	float BulletSpread;

	float SecondsBetweenShots;

	FRotator TargetRotation;
	UPROPERTY(BlueprintReadWrite, Category = "Weapon", Replicated)
	int32 CurrentAmmo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	int32 DefaultAmmo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float ReloadTime = 1.f;

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerFire();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerStopFire();

	UPROPERTY(ReplicatedUsing=OnRep_HitScanTrace)
	FHitScanTrace HitScanTrace;

	UFUNCTION()
	void OnRep_HitScanTrace();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerReload(int32 NewAmmo);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
		virtual void StartFire();

		virtual void StopFire();

		void Reload(int32 NewAmmo);

		

		float GetReloadTime();

		int32 GetCurrentAmmo();

		int32 GetDefaultAmmo();

};
