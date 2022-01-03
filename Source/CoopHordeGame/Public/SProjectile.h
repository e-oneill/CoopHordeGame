// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SProjectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;
class URadialForceComponent;
class USoundBase;
UCLASS()
class COOPHORDEGAME_API ASProjectile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Sphere collision component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
		USphereComponent* CollisionComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	float BlastDelay;

	UPROPERTY()
	FTimerHandle BlastTimer;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
		UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
		UParticleSystem* ImpactEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
		USoundBase* ExplosionSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
		float BaseDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
		float BlastRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
		float ProjectileVelocity = 1200.f;

	float CreateTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion")
		URadialForceComponent* ForceComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		TSubclassOf<UDamageType> DamageType;

	UFUNCTION()
	void ExplodeProjectile();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//ASProjectile();

	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Returns CollisionComp subobject **/
	USphereComponent* GetCollisionComp() const { return CollisionComp; }

	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

};
