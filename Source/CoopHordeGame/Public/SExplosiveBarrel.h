// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "SExplosiveBarrel.generated.h"

class UStaticMeshComponent;
class USHealthComponent;
class UMaterial;
class UParticleSystem;
class URadialForceComponent;
class UDamageType;
class USoundBase;

UCLASS()
class COOPHORDEGAME_API ASExplosiveBarrel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASExplosiveBarrel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USHealthComponent* HealthComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
		UMaterial* DefaultMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
		UMaterial* ExplodeMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion")
		UParticleSystem* ExplosionEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion")
		URadialForceComponent* ForceComponent;

	UPROPERTY(ReplicatedUsing=OnRep_Exploded)
	bool bIsExploded;

	UFUNCTION()
	void OnRep_Exploded();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion")
		float BlastRadius = 600.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion")
		float BlastForce = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion")
		float BaseDamage = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion")
		TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
		USoundBase* ExplosionSound;



	//UFUNCTION();
	//void FireImpulse();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageTypeTaken, class AController* InstigatedBy, AActor* DamageCauser);

};
