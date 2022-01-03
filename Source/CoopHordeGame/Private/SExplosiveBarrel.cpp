// Fill out your copyright notice in the Description page of Project Settings.


#include "SExplosiveBarrel.h"
#include "Particles/ParticleSystem.h"
#include "Materials/Material.h"
#include "SHealthComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
// Sets default values
ASExplosiveBarrel::ASExplosiveBarrel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetMaterial(0, DefaultMaterial);
	RootComponent = MeshComponent;
	
	HealthComponent = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComponent"));
	HealthComponent->OnHealthChanged.AddDynamic(this, &ASExplosiveBarrel::OnHealthChanged);

	ForceComponent = CreateDefaultSubobject<URadialForceComponent>(TEXT("ForceComponent"));
	ForceComponent->ForceStrength = BlastForce;
	ForceComponent->ImpulseStrength = BlastForce;
	ForceComponent->Radius = BlastRadius;
	ForceComponent->bIgnoreOwningActor = true;
	ForceComponent->bAutoActivate = false;
	//ForceComponent->bImpulseVelChange = true;
	bReplicates = true;
	SetReplicateMovement(true);
	
}

// Called when the game starts or when spawneds
void ASExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASExplosiveBarrel::OnRep_Exploded()
{
	if (ExplosionEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation(), GetActorRotation());
	}
	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
	}

	MeshComponent->SetMaterial(0, ExplodeMaterial);
	
}


// Called every frame
void ASExplosiveBarrel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASExplosiveBarrel::OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageTypeTaken, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (bIsExploded)
	{
		return;
	}
	if (Health <= 0)
	{
		bIsExploded = true;
		OnRep_Exploded();
		//Barrel Explodes
		MeshComponent->SetSimulatePhysics(true);
		FVector Upwards = MeshComponent->GetUpVector() * 400.f;
		TArray<AActor*> IgnoreActors;
		MeshComponent->AddImpulse(Upwards, NAME_None, true);
		float InnerRadius = BlastRadius / 2.f;
		//UGameplayStatics::ApplyRadialDamage(GetWorld(), BaseDamage, GetActorLocation(), BlastRadius, DamageType, IgnoreActors);
		UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(), BaseDamage, 1.0f, GetActorLocation(), InnerRadius, BlastRadius, 1.0f, DamageType, IgnoreActors);
		MakeNoise(1.0f, this->GetInstigator());
		
		ForceComponent->FireImpulse();
		SetLifeSpan(10.0f);
	}
}



void ASExplosiveBarrel::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASExplosiveBarrel, bIsExploded);
}
