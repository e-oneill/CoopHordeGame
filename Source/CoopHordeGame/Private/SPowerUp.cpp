// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerUp.h"
#include "TimerManager.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "SCharacter.h"
#include "Net/UnrealNetwork.h"
// Sets default values
ASPowerUp::ASPowerUp()
{
	PowerUpInterval = 0.0f;
	TotalNrOfTicks = 0;

	PickupRotator = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("PickupRotator"));
	FRotator rotator;
	rotator.Yaw = -90;
	bIsActivated = false;
	PickupRotator->RotationRate = rotator;
	bReplicates = true;
}


// Called when the game starts or when spawned
void ASPowerUp::BeginPlay()
{
	Super::BeginPlay();
}

void ASPowerUp::OnTickPowerUp()
{
	
	if (TicksProcessed >= TotalNrOfTicks)
	{
		//This is our last tick, call expired and clear timer
		OnExpired();
		GetWorldTimerManager().ClearTimer(PowerUpTickHandle);
		bIsActivated = false;
		OnRep_PowerUpActive();

		return;
	}
	TicksProcessed++;
	OnPowerUpTicked();
}

void ASPowerUp::OnRep_PickedUp()
{
	OnPowerUpHasActivator(Activator);
}

void ASPowerUp::ActivatePowerUp(ASCharacter* PlayerPawn)
{
	OnActivated();
	bIsActivated = true;
	Activator = PlayerPawn;
	OnRep_PowerUpActive();
	OnRep_PickedUp();
	if (PowerUpInterval > 0.0f)
	{
		GetWorldTimerManager().SetTimer(PowerUpTickHandle, this, &ASPowerUp::OnTickPowerUp, PowerUpInterval, true);
	}
	else
	{
		OnTickPowerUp();
	}
}


void ASPowerUp::OnRep_PowerUpActive()
{
	OnPowerUpStateChanged(bIsActivated);
}



void ASPowerUp::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPowerUp, bIsActivated);
	DOREPLIFETIME(ASPowerUp, Activator);

}