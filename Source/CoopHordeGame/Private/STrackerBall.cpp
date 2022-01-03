// Fill out your copyright notice in the Description page of Project Settings.


#include "STrackerBall.h"
#include "SCharacter.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "TimerManager.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "SHealthComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "EngineUtils.h"

// Sets default values
ASTrackerBall::ASTrackerBall()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;
	bUseVelocityChange = true;
	MovementForce = 1000.f;
	RequiredDistanceToTarget = 100;
	MeshComp->SetCanEverAffectNavigation(false);
	MeshComp->SetSimulatePhysics(true);

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComponent->SetSphereRadius(200);
	SphereComponent->SetupAttachment(RootComponent);
	SphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);


	HealthComponent = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComponent"));
	HealthComponent->OnHealthChanged.AddDynamic(this, &ASTrackerBall::HandleTakeDamage);
	ExplosionDamage = 100.f;
	BlastRadius = 200.f;

	ForceComponent = CreateDefaultSubobject<URadialForceComponent>(TEXT("ForceComponent"));
	ForceComponent->SetupAttachment(MeshComp);
	ForceComponent->ForceStrength = ExplosionDamage * 5.f;
	ForceComponent->ImpulseStrength = ExplosionDamage * 5.f;
	ForceComponent->Radius = BlastRadius;
	ForceComponent->bIgnoreOwningActor = true;
	ForceComponent->bAutoActivate = false;

	SelfDestructInterval = 0.4f;
	SelfDestructDamagePerInterval = 20;

	bReplicates = true;
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void ASTrackerBall::BeginPlay()
{
	Super::BeginPlay();

	//Find initial move to
	if (HasAuthority())
	{
		NextPathPoint = GetNextPathPoint();
		//GetWorldTimerManager().SetTimer(PackNumberCheckTimer, this, &ASTrackerBall::CalcPowerLevel, 1.0f, true, 0.0f);
	}
	

}

FVector ASTrackerBall::GetNextPathPoint()
{
	AActor* BestTarget = nullptr;
	float NearestDistance = FLT_MAX;
	//Hack to get the location of player 1
	for (TActorIterator<ASCharacter> It(GetWorld()); It; ++It)
	{
		ASCharacter* TestCharacter = *It;
		if (TestCharacter == nullptr || USHealthComponent::IsFriendly(TestCharacter, this))
		{
			continue;
		}

		USHealthComponent* HC = Cast<USHealthComponent>(TestCharacter->GetComponentByClass(USHealthComponent::StaticClass()));
		if (HC && HC->GetHealth() > 0)
		{
			float Distance = (TestCharacter->GetActorLocation() - GetActorLocation()).Size();
				if (NearestDistance > Distance)
				{
					NearestDistance = Distance;
					BestTarget = Cast<AActor>(TestCharacter);
				}
		}
	}
	if (BestTarget)
	{
		UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), BestTarget);

		if (NavPath)
		{
			if (NavPath->PathPoints.Num() > 1)
			{
				//Return next point of path
				return NavPath->PathPoints[1];
			}
		}
	}
	

	//Failed to find path
	return GetActorLocation();
}

void ASTrackerBall::HandleTakeDamage(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	//Explode on Hitpoints == 0
	//Pulse the material on Hit
	if (MatInst == nullptr)
	{
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
	}
	if (MatInst)
	{
		MatInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
	}
	if (Health <= 0)
	{
		SelfDestruct();
	}

	//UE_LOG(LogTemp, Log, TEXT("Current Health of Tracker Bot is %s"), *FString::SanitizeFloat(Health));
}



void ASTrackerBall::SelfDestruct()
{
	if (!bExploded)
	{
		bExploded = true;
		if (ExplosionEffect)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
		}
		if (ExplosionSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
		}

		MeshComp->SetVisibility(false);
		MeshComp->SetSimulatePhysics(false);
		MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		

		if (HasAuthority())
		{
			TArray<AActor*> IgnoredActors;
			IgnoredActors.Add(this);
			float ActualDamage = ExplosionDamage + (ExplosionDamage * ((float)PackNumber / 10.f));
			UGameplayStatics::ApplyRadialDamage(this, ActualDamage, GetActorLocation(), BlastRadius, nullptr, IgnoredActors, this, GetInstigatorController(), true);
			ForceComponent->FireImpulse();
			SetLifeSpan(1.0f);
		}
	}
	return;
}

void ASTrackerBall::DamageSelf()
{
	UGameplayStatics::ApplyDamage(this, SelfDestructDamagePerInterval, GetInstigatorController(), this, nullptr);
}

void ASTrackerBall::CalcPowerLevel()
{
	const float Radius = 600;
	FCollisionShape CollShape;
	CollShape.SetSphere(Radius);
	TArray<FOverlapResult> Overlaps;
	FCollisionObjectQueryParams QueryParams;
	QueryParams.AddObjectTypesToQuery(ECC_PhysicsBody);
	QueryParams.AddObjectTypesToQuery(ECC_Pawn);
	GetWorld()->OverlapMultiByObjectType(Overlaps, GetActorLocation(), FQuat::Identity, QueryParams, CollShape);
	DrawDebugSphere(GetWorld(), GetActorLocation(), Radius, 12, FColor::White, false, 1.0f);
	int32 NrOfBots = 0;
	for (FOverlapResult Result: Overlaps)
	{
		ASTrackerBall* TrackerBall = Cast<ASTrackerBall>(Result.GetActor());
		if (TrackerBall)
		{
			NrOfBots++;
		}
	}

	const int32 MaxPackNumber = 4;
	PackNumber = FMath::Clamp(NrOfBots, 0, MaxPackNumber);

	if (MatInst == nullptr)
	{
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
	}
	if (MatInst)
	{
		float Alpha = PackNumber / (float)MaxPackNumber;
		MatInst->SetScalarParameterValue("PowerLevelAlpha", Alpha);
	}

	DrawDebugString(GetWorld(), GetActorLocation(), FString::FromInt(PackNumber), nullptr, FColor::White, 1.0f, false);
}

// Called every frame
void ASTrackerBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bExploded)
	{
		float DistanceToTarget = (GetActorLocation() - NextPathPoint).Size();
		float DistanceFromLastLocation = (GetActorLocation() - LastLocation).Size();
	
		if (DistanceToTarget <= RequiredDistanceToTarget)
		{
			NextPathPoint = GetNextPathPoint();
			//DrawDebugString(GetWorld(), GetActorLocation(), "Target Reached");
		}
		else
		{
			//Keep rolling towards next target
			FVector ForceDirection = NextPathPoint - GetActorLocation();
			ForceDirection.Normalize();
			ForceDirection *= MovementForce;
			MeshComp->AddForce(ForceDirection, NAME_None, bUseVelocityChange);
			//DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + ForceDirection, 32, FColor::Yellow, false, 0.0f, 0, 1.0f);
		}
		if (DistanceFromLastLocation <= RequiredDistanceToTarget / 2)
		{
			NextPathPoint = GetNextPathPoint();
			//DrawDebugString(GetWorld(), GetActorLocation(), "AI Stuck");
		}
		LastLocation = GetActorLocation();
	}
	//DrawDebugSphere(GetWorld(), NextPathPoint, 20, 12, FColor::Yellow, false, 0.0f, 1.0f);

}

void ASTrackerBall::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	if (!bStartedSelfDestruct && !bExploded)
	{
		ASCharacter* PlayerPawn = Cast<ASCharacter>(OtherActor);
		if (PlayerPawn && !USHealthComponent::IsFriendly(OtherActor, this))
		{
			if (ExplosionWarning)
			{
				UGameplayStatics::SpawnSoundAttached(ExplosionWarning, RootComponent);
			}
			bStartedSelfDestruct = true;
			//Cast succeeded, so we overlapped with a player;
			if (HasAuthority())
			{
				GetWorldTimerManager().SetTimer(SelfDestruct_Timer, this, &ASTrackerBall::DamageSelf, SelfDestructInterval, true, 0.0f);
			}

		}
	}
}

// Called to bind functionality to input

