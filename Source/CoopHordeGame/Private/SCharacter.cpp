// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"
#include "SRangedWeapon.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "TimerManager.h"
#include "CoopHordeGame/CoopHordeGame.h"
#include "SHealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "LoadoutComponent.h"
#include "Items/SLoadoutItemObjectWeapon.h"
// Sets default values
ASCharacter::ASCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	//MeshComponent->SetupAttachment(RootComponent);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bUsePawnControlRotation = true;



	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);
	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCamera->SetupAttachment(SpringArm);

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetAutoActivate(false);
	FirstPersonCamera->SetupAttachment(GetMesh(), TEXT("head"));

	HealthComponent = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComponent"));

	Loadout = CreateDefaultSubobject<ULoadoutComponent>(TEXT("Loadout"));

	UCharacterMovementComponent* CM = GetCharacterMovement();

	SprintSpeed = 2.f;

	ActiveCamera = ThirdPersonCamera;
	ZoomedFOV = 65.f;
	ZoomInterpSpeed = 20;
	WeaponAttachSocketName = "weaponSocket";
	bHasInfiniteAmmo = false;
	bReplicates = true;
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();

	DefaultFOV = ThirdPersonCamera->FieldOfView;

	//Spawn a Default Weapon

	if (HasAuthority())
	{
		for (int i = 0; i < Loadout->Items.Num(); i++)
		{
			USLoadoutItemObject* Item = Loadout->Items[i];
			EquipppedWeapon = Cast<USLoadoutItemObjectWeapon>(Item);
			if (EquipppedWeapon)
			{
				TotalAmmo = EquipppedWeapon->Ammo;
				break;
			}
		}

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		CurrentWeapon = GetWorld()->SpawnActor<ASRangedWeapon>(EquipppedWeapon, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (CurrentWeapon)
		{
			CurrentWeapon->SetOwner(this);
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
			CurrentAmmo = CurrentWeapon->GetCurrentAmmo();
			DefaultAmmo = CurrentWeapon->GetDefaultAmmo();
		}
	}

	HealthComponent->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);
}

void ASCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector() * Value);

}

void ASCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector() * Value);
}

void ASCharacter::BeginCrouch()
{
	Crouch();
}

void ASCharacter::EndCrouch()
{
	UnCrouch();
}



void ASCharacter::BeginJump()
{
	Jump();
}

void ASCharacter::EndJump()
{
	StopJumping();
}

void ASCharacter::Sprint()
{
	GetCharacterMovement()->MaxWalkSpeed *= SprintSpeed;
}

void ASCharacter::StopSprint()
{
	GetCharacterMovement()->MaxWalkSpeed /= SprintSpeed;
}

void ASCharacter::StartFire()
{
	if (CurrentWeapon && !bIsReloading)
	{
		CurrentWeapon->StartFire();
	}
}

void ASCharacter::DeductAmmo()
{
	CurrentAmmo--;
	TotalAmmo--;
	if (!Loadout->IsNetSimulating())
	{
		EquipppedWeapon->Ammo--;
	}
	
}

void ASCharacter::StopFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire();
		//CurrentAmmo = CurrentWeapon->GetCurrentAmmo();
	}

}

void ASCharacter::ServerSwitchWeapon_Implementation(int32 LoadoutIndex)
{
	SwitchWeapon(LoadoutIndex);
}

bool ASCharacter::ServerSwitchWeapon_Validate(int32 LoadoutIndex)
{
	return true;
}

void ASCharacter::SwitchWeapon(int32 LoadoutIndex)
{
	if (!HasAuthority())
	{
		ServerSwitchWeapon(LoadoutIndex);
		return;
	}
	

		if (Loadout->Items.Num() > LoadoutIndex)
		{
			USLoadoutItemObject* Item = Loadout->Items[LoadoutIndex];
			EquipppedWeapon = nullptr;
			EquipppedWeapon = Cast<USLoadoutItemObjectWeapon>(Item);
			if (EquipppedWeapon)
			{
				TSubclassOf<ASRangedWeapon> NewWeapon = EquipppedWeapon->Weapon;
				TotalAmmo = EquipppedWeapon->Ammo;
	
				if (NewWeapon)
				{
					CurrentWeapon->Destroy();
					FActorSpawnParameters SpawnParams;
					SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
					CurrentWeapon = GetWorld()->SpawnActor<ASRangedWeapon>(NewWeapon, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
					if (CurrentWeapon)
					{
						CurrentWeapon->SetOwner(this);
						CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
						CurrentAmmo = CurrentWeapon->GetCurrentAmmo();
						if (CurrentAmmo > TotalAmmo)
						{
							CurrentAmmo = TotalAmmo;
						}
						DefaultAmmo = CurrentWeapon->GetDefaultAmmo();
					}
				}
			}
		}
	
}


void ASCharacter::SwitchOne()
{
	//Pass
	SwitchWeapon(0);
	OnSwitchWeapon();
}

void ASCharacter::SwitchTwo()
{
	SwitchWeapon(1);
	OnSwitchWeapon();
}

void ASCharacter::SwitchThree()
{
	SwitchWeapon(2);
	OnSwitchWeapon();
}

void ASCharacter::SwitchFour()
{
	SwitchWeapon(3);
	OnSwitchWeapon();
}

void ASCharacter::BeginZoom()
{
	bAimDownSights = true;
}

void ASCharacter::EndZoom()
{
	bAimDownSights = false;
}

void ASCharacter::ServerReload_Implementation()
{
	Reload();
}

bool ASCharacter::ServerReload_Validate()
{
	return true;
}

void ASCharacter::SwitchCamera()
{
	if (ThirdPersonCamera->IsActive())
	{
		ActiveCamera = FirstPersonCamera;
		FirstPersonCamera->SetActive(true, false);
		ThirdPersonCamera->SetActive(false, false);
		FirstPersonCamera->bUsePawnControlRotation = true;
	}
	else
	{
		ActiveCamera = ThirdPersonCamera;
		FirstPersonCamera->SetActive(false, false);
		ThirdPersonCamera->SetActive(true, false);
	}
}

void ASCharacter::StartReload()
{
	if (CurrentWeapon && CurrentAmmo < DefaultAmmo)
	{
		bIsReloading = true;
		float ReloadTime = CurrentWeapon->GetReloadTime();
		GetWorld()->GetTimerManager().SetTimer(ReloadTimer, this, &ASCharacter::Reload, ReloadTime);
	}
	UE_LOG(LogTemp, Log, TEXT("Attempting Reload, Current Ammo is %d, Default Ammo is %d"), CurrentAmmo, DefaultAmmo);
}

void ASCharacter::Reload()
{

	if (CurrentWeapon)
	{
		UE_LOG(LogTemp, Log, TEXT("Reload successfully called in Character"));
		bIsReloading = false;
		if (HasAuthority())
		{
			if (TotalAmmo > DefaultAmmo)
			{
				CurrentAmmo = DefaultAmmo;
			}
			else
			{
				CurrentAmmo = TotalAmmo;
			}
			CurrentWeapon->Reload(TotalAmmo);
		}
		else
		{
			ServerReload();
		}
		
	}
}

void ASCharacter::OnHealthChanged(USHealthComponent* HealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0.0f && !bDied)
	{
		bDied = true;
		//Kill Pawn
		GetMovementComponent()->StopMovementImmediately();

		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		DetachFromControllerPendingDestroy();

		SetLifeSpan(10.0f);
	}
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	float TargetFOV = bAimDownSights ? ZoomedFOV : DefaultFOV;
	float InterpFOV = FMath::FInterpTo(ActiveCamera->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);
	ActiveCamera->SetFieldOfView(InterpFOV);
	if (HasAuthority())
	{
		//CurrentAmmo = CurrentWeapon->GetCurrentAmmo();
	}



}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &ASCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ASCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ASCharacter::EndCrouch);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::BeginJump);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ASCharacter::Sprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ASCharacter::StopSprint);
	PlayerInputComponent->BindAction("SwitchCamera", IE_Pressed, this, &ASCharacter::SwitchCamera);
	//PlayerInputComponent->BindAction("SwitchWeapon", IE_Pressed, this, &ASCharacter::SwitchWeapon);
	PlayerInputComponent->BindAction("AimDownSights", IE_Pressed, this, &ASCharacter::BeginZoom);
	PlayerInputComponent->BindAction("AimDownSights", IE_Released, this, &ASCharacter::EndZoom);
	PlayerInputComponent->BindAction("AimDownSights", IE_Released, this, &ASCharacter::EndZoom);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ASCharacter::StopFire);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ASCharacter::StartReload);
	PlayerInputComponent->BindAction("One", IE_Pressed, this, &ASCharacter::SwitchOne);
	PlayerInputComponent->BindAction("Two", IE_Pressed, this, &ASCharacter::SwitchTwo);
	PlayerInputComponent->BindAction("Three", IE_Pressed, this, &ASCharacter::SwitchThree);
	PlayerInputComponent->BindAction("Four", IE_Pressed, this, &ASCharacter::SwitchFour);
}

FVector ASCharacter::GetPawnViewLocation() const
{
	if (ActiveCamera)
	{
		//UE_LOG(LogTemp, Log, TEXT("Getting Camera Location"));
		return ActiveCamera->GetComponentLocation();
	}

	return Super::GetPawnViewLocation();
}

ASRangedWeapon* ASCharacter::GetCurrentWeapon()
{
	return CurrentWeapon;
}

void ASCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASCharacter, CurrentWeapon);
	DOREPLIFETIME(ASCharacter, CurrentAmmo);
	DOREPLIFETIME(ASCharacter, DefaultAmmo);
	DOREPLIFETIME(ASCharacter, bHasInfiniteAmmo);
	DOREPLIFETIME(ASCharacter, TotalAmmo);
	DOREPLIFETIME(ASCharacter, bIsReloading);
	DOREPLIFETIME(ASCharacter, bDied);
}

