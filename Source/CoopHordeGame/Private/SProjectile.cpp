// Fill out your copyright notice in the Description page of Project Settings.


#include "SProjectile.h"
#include "SCharacter.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "DrawDebugHelpers.h"

// Sets default values
ASProjectile::ASProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &ASProjectile::OnHit);	// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	ForceComponent = CreateDefaultSubobject<URadialForceComponent>(TEXT("ForceComponent"));
	ForceComponent->ForceStrength = BaseDamage*10.f;
	ForceComponent->ImpulseStrength = BaseDamage * 10.f;
	ForceComponent->Radius = BlastRadius;
	ForceComponent->bIgnoreOwningActor = true;
	ForceComponent->bAutoActivate = false;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = ProjectileVelocity;
	ProjectileMovement->MaxSpeed = ProjectileVelocity;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = BlastDelay + 1.0f;
	CreateTime = 0.f;
	bReplicates = true;
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void ASProjectile::BeginPlay()
{
	Super::BeginPlay();
	//FTimerHandle Timer;
	//GetWorldTimerManager().SetTimer(Timer, this, &ASProjectile::ExplodeProjectile, BlastDelay, false);
	CreateTime = GetWorld()->TimeSeconds;
}

void ASProjectile::ExplodeProjectile()
{
	if (ImpactEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, GetActorLocation(), GetActorRotation());
	}
	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
	}
		TArray<AActor*> IgnoreActors;
		DrawDebugSphere(GetWorld(), GetActorLocation(), BlastRadius, 12, FColor::Red, false, 2.0f);
		float InnerRadius = BlastRadius / 2.f;
		//UGameplayStatics::ApplyRadialDamage(GetWorld(), BaseDamage, GetActorLocation(), BlastRadius, DamageType, IgnoreActors);
		UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(), BaseDamage, 1.0f, GetActorLocation(), InnerRadius, BlastRadius, 1.0f, DamageType, IgnoreActors, GetOwner());
		ForceComponent->FireImpulse();
		MakeNoise(1.0f, this->GetInstigator());
		Destroy();
}


void ASProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
	{
		//OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
	}
}


 //Called every frame
void ASProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetWorld()->TimeSeconds - CreateTime >= BlastDelay)
	{
		ExplodeProjectile();
	}
}

