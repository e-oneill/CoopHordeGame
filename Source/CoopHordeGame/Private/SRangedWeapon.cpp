// Fill out your copyright notice in the Description page of Project Settings.


#include "SRangedWeapon.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "CoopHordeGame/CoopHordeGame.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
#include "SGunAnimationPack.h"
#include "SCharacter.h"

static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(TEXT("COOP.DebugWeapons"), DebugWeaponDrawing, TEXT("Draw Debug Lines for Weapons"), ECVF_Cheat);

// Sets default values
ASRangedWeapon::ASRangedWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;
	RateOfFire = 600;
	GunAnimationPack = CreateDefaultSubobject<USGunAnimationPack>(TEXT("GunAnimationPack"));

	bReplicates = true;
	NetUpdateFrequency = 66.0f;
	MinNetUpdateFrequency = 33.0f;

	BulletSpread = 1.0f;
}

void ASRangedWeapon::BeginPlay()
{
	Super::BeginPlay();
	CurrentAmmo = DefaultAmmo;
	SecondsBetweenShots = 60.f / RateOfFire;
	UE_LOG(LogTemp, Log, TEXT("Weapon Initialised, Current Ammo is %d"), CurrentAmmo);
}

// Called when the game starts or when spawned


void ASRangedWeapon::Fire()
{
	//Trace a hit line from pawn eyes to crosshair location

	if (!HasAuthority())
	{
		ServerFire();
		//return;
	}
	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		ASCharacter* MyChar = Cast<ASCharacter>(MyOwner);
		if (MyChar)
		{
			if (MyChar->CurrentAmmo > 0 || MyChar->bHasInfiniteAmmo)
			{
				FVector EyeLocation;
				FRotator EyeRotation;
				MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

				FVector MuzzleLocation = MeshComponent->GetSocketLocation("MuzzleFlashSocket");
				FRotator MuzzleRotation = MeshComponent->GetSocketRotation("MuzzleFlashSocket");
				FVector ShotDirection = EyeRotation.Vector();
				float HalfRad = FMath::DegreesToRadians(BulletSpread);
				ShotDirection = FMath::VRandCone(ShotDirection, HalfRad, HalfRad);

				FVector TraceEnd = EyeLocation + (ShotDirection * 10000);
				//Creating Variable to Track the final point of the hitscan (e.g. if we hit something, we'll override this)
				FVector TracerEndPoint = TraceEnd;
				FCollisionQueryParams QueryParams;
				QueryParams.AddIgnoredActor(MyOwner);
				QueryParams.AddIgnoredActor(this);
				QueryParams.bTraceComplex = true;
				QueryParams.bReturnPhysicalMaterial = true;
				EPhysicalSurface SurfaceType = SurfaceType_Default;

				FHitResult Hit;
				if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
				{
					//Blocking hit, process damage
					SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());
					AActor* HitActor = Hit.GetActor();
					TracerEndPoint = Hit.ImpactPoint;

					float ActualDamage = HitDamage;
					if (SurfaceType == SURFACE_FLESHVULNERABLE)
					{
						ActualDamage *= 3.f;
					}
					UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), MyOwner, DamageType);

					PlayImpactEffects(SurfaceType, Hit.ImpactPoint);

				}
				PlayFireEffects(TracerEndPoint);
				if (DebugWeaponDrawing > 0)
				{
					DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, 1.0f, 0, 1.0f);
				}


				//CurrentAmmo--;

				TargetRotation = EyeRotation;
				TargetRotation.Pitch += 2.f;


				if (HasAuthority())
				{
					HitScanTrace.ForceReplication++;
					HitScanTrace.TraceTo = TracerEndPoint;
					HitScanTrace.SurfaceType = SurfaceType;
				}
				LastFireTime = GetWorld()->TimeSeconds;
				if (!MyChar->bHasInfiniteAmmo)
				{
					MyChar->DeductAmmo();
				}
			}
		}
	}
}

void ASRangedWeapon::ServerFire_Implementation()
{
	Fire();
}

bool ASRangedWeapon::ServerFire_Validate()
{
	return true;
}

void ASRangedWeapon::ServerStopFire_Implementation()
{
	StopFire();
}

bool ASRangedWeapon::ServerStopFire_Validate()
{
	return true;
}

void ASRangedWeapon::OnRep_HitScanTrace()
{
	//Play cosmetic effects
	PlayFireEffects(HitScanTrace.TraceTo);
	PlayImpactEffects(HitScanTrace.SurfaceType, HitScanTrace.TraceTo);
}



void ASRangedWeapon::ServerReload_Implementation(int32 NewAmmo)
{
		Reload(NewAmmo);
}

bool ASRangedWeapon::ServerReload_Validate(int32 NewAmmo)
{
	return true;
}

void ASRangedWeapon::StartFire()
{
	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		ASCharacter* MyChar = Cast<ASCharacter>(MyOwner);
		if (MyChar)
		{
			if (MyChar->CurrentAmmo > 0 || MyChar->bHasInfiniteAmmo)
			{
				if (bFullAuto)
				{
					float FirstDelay = FMath::Max(LastFireTime + SecondsBetweenShots - GetWorld()->TimeSeconds, 0.0f);
					GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &ASRangedWeapon::Fire, SecondsBetweenShots, true, FirstDelay);
				}
				else
				{
					Fire();
				}

			}
		}
	}

}

void ASRangedWeapon::StopFire()
{
	if (!HasAuthority())
	{
		ServerStopFire();
		//return;
	}
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}

void ASRangedWeapon::Reload(int32 NewAmmo)
{

	CurrentAmmo = NewAmmo;
	if (!HasAuthority())
	{
		ServerReload(NewAmmo);
	}

}

float ASRangedWeapon::GetReloadTime()
{
	return ReloadTime;
}

int32 ASRangedWeapon::GetCurrentAmmo()
{
	if (CurrentAmmo)
	{
		return CurrentAmmo;
	}
	return 0;
}

int32 ASRangedWeapon::GetDefaultAmmo()
{
	return DefaultAmmo;
}

void ASRangedWeapon::PlayFireEffects(FVector TracerEndPoint)
{
	//Casting to Pawn to get the Pawn Overrides of Actor Functions - as this is a controlled actor
	APawn* MyOwner = Cast<APawn>(GetOwner());
	if (MyOwner)
	{
		APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
		if (PC)
		{

			PC->ClientStartCameraShake(FireCameraShake);

		}
	}
	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComponent, MuzzleSocket);
	}
	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}
	//Had to comment out code - seems to be a conflict between legacy particle FX and 5.0EA
	//if (TracerEffect)
	//{
	//	UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);
	//	if (TracerComp)
	//	{
	//		TracerComp->SetVectorParameter(TracerTargetName, TracerEndPoint);
	//	}
	//}
}

void ASRangedWeapon::PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint)
{
	UParticleSystem* SelectedEffect = nullptr;
	switch (SurfaceType)
	{
	case SURFACE_FLESHDEFAULT:
	case SURFACE_FLESHVULNERABLE:
		SelectedEffect = FleshImpactEffect;
		break;
	default:
		SelectedEffect = DefaultImpactEffect;
		break;
	}
	if (SelectedEffect)
	{
		FVector MuzzleLocation = MeshComponent->GetSocketLocation(MuzzleSocket);
		FVector ShotDirection = ImpactPoint - MuzzleLocation;
		ShotDirection.Normalize();
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint, ShotDirection.Rotation());
	}
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, ImpactPoint);
	}
}

// Called every frame
void ASRangedWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASRangedWeapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASRangedWeapon, CurrentAmmo);
	DOREPLIFETIME_CONDITION(ASRangedWeapon, HitScanTrace, COND_SkipOwner);
}


