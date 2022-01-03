// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPickUp.generated.h"
class USphereComponent;
class UDecalComponent;
class ASPowerUp;
UCLASS()
class COOPHORDEGAME_API ASPickUp : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPickUp();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* SphereComponent;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UDecalComponent* DecalComponent;
	UPROPERTY(EditInstanceOnly, Category = "Power Up")
	TSubclassOf<ASPowerUp> PowerUp;
	UFUNCTION()
	void RespawnPickup();

	ASPowerUp* PowerUpInstance;

	UPROPERTY(EditInstanceOnly, Category = "Power Up")
	float CooldownDuration;

	FTimerHandle RespawnTimer;
public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

};
