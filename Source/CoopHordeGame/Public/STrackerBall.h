// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "STrackerBall.generated.h"
class USHealthComponent;
class USoundBase;
class USoundCue;
class UParticleSystem;
class URadialForceComponent;
class USphereComponent;
UCLASS()
class COOPHORDEGAME_API ASTrackerBall : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASTrackerBall();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
		UStaticMeshComponent* MeshComp;

	UFUNCTION()
		FVector GetNextPathPoint();

	FVector NextPathPoint;

	FVector LastLocation;

	UPROPERTY(EditDefaultsOnly, Category = "Tracker Bot")
		float MovementForce;
	UPROPERTY(EditDefaultsOnly, Category = "Tracker Bot")
		bool bUseVelocityChange;
	UPROPERTY(EditDefaultsOnly, Category = "Tracker Bot")
		float RequiredDistanceToTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USHealthComponent* HealthComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
		URadialForceComponent* ForceComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USphereComponent* SphereComponent;

	UFUNCTION()
		void HandleTakeDamage(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	//Dynamic Material To Pulse
	UMaterialInstanceDynamic* MatInst;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion")
		UParticleSystem* ExplosionEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
		USoundBase* ExplosionSound;

	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
		USoundBase* ExplosionWarning;

	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
		float ExplosionDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
		float BlastRadius;

	void SelfDestruct();
	bool bExploded;
	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
	float SelfDestructInterval;

	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
		float SelfDestructDamagePerInterval;
	FTimerHandle SelfDestruct_Timer;
	bool bStartedSelfDestruct;
	FTimerHandle PackNumberCheckTimer;
	UPROPERTY(BlueprintReadOnly)
	int32 PackNumber;

	UFUNCTION()
		void DamageSelf();

	UFUNCTION()
		void CalcPowerLevel();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

};
