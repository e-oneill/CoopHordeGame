// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPowerUp.generated.h"

class URotatingMovementComponent;
class ASCharacter;
UCLASS()
class COOPHORDEGAME_API ASPowerUp : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPowerUp();

protected:
	UPROPERTY(ReplicatedUsing=OnRep_PowerUpActive)
	bool bIsActivated;
	UFUNCTION()
	void OnRep_PowerUpActive();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Time between Power Up ticks
	UPROPERTY(EditDefaultsOnly, Category = "Power Ups");
	float PowerUpInterval;

	// Total times we apply the Power Up effect
	UPROPERTY(EditDefaultsOnly, Category = "Power Ups");
	int32 TotalNrOfTicks;

	int32 TicksProcessed;
	UPROPERTY(EditDefaultsOnly, Category = "Power Ups");
	URotatingMovementComponent* PickupRotator;

	FTimerHandle PowerUpTickHandle;
	UFUNCTION()
	void OnTickPowerUp();

	UFUNCTION(BlueprintImplementableEvent)
	void OnPowerUpStateChanged(bool bNewIsActive);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PickedUp)
	ASCharacter* Activator;

	UFUNCTION(BlueprintImplementableEvent)
	void OnPowerUpHasActivator(ASCharacter* NewActivator);

	UFUNCTION()
		void OnRep_PickedUp();
public:	
	void ActivatePowerUp(ASCharacter* PlayerPawn);

	UFUNCTION(BlueprintImplementableEvent, Category = "Power Ups")
		void OnActivated();
	UFUNCTION(BlueprintImplementableEvent, Category = "Power Ups")
		void OnExpired();
	UFUNCTION(BlueprintImplementableEvent, Category = "Power Ups")
		void OnPowerUpTicked();
};
