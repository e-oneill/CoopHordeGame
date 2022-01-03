// Fill out your copyright notice in the Description page of Project Settings.


#include "SPickUp.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "SCharacter.h"
#include "SPowerUp.h"
#include "TimerManager.h"
// Sets default values
ASPickUp::ASPickUp()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetSphereRadius(75.0f);
	RootComponent = SphereComponent;

	DecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComponent"));
	DecalComponent->SetRelativeRotation(FRotator(90, 0, 0));
	DecalComponent->DecalSize = FVector(64, 75, 75);
	DecalComponent->SetupAttachment(RootComponent);
	bReplicates = true;
}

// Called when the game starts or when spawned
void ASPickUp::BeginPlay()
{
	Super::BeginPlay();
	if (PowerUp == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Power Up Class for %s has not been selected. Please check the blueprint"), *GetName());
		return;
	}
	if (HasAuthority())
	{
		RespawnPickup();
	}

}

void ASPickUp::RespawnPickup()
{
	FActorSpawnParameters SpawnParams;
	PowerUpInstance = GetWorld()->SpawnActor<ASPowerUp>(PowerUp, GetTransform(), SpawnParams);
}

void ASPickUp::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	ASCharacter* PlayerPawn = Cast<ASCharacter>(OtherActor);
	if (PlayerPawn && PowerUpInstance && HasAuthority())
	{

		if (PlayerPawn->IsPlayerControlled())
		{
			PowerUpInstance->ActivatePowerUp(PlayerPawn);
			PowerUpInstance = nullptr;


			GetWorldTimerManager().SetTimer(RespawnTimer, this, &ASPickUp::RespawnPickup, CooldownDuration);
		}
	}
}

// Called every frame
//void ASPickUp::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}

