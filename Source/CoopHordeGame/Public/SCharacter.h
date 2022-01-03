// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class USkeletalMeshComponent;
class ASRangedWeapon;
class USHealthComponent;
class USLoadoutItem;

UCLASS()
class COOPHORDEGAME_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

#pragma region Locomotion
	void MoveForward(float Value);

	void MoveRight(float Value);

	void BeginCrouch();

	void EndCrouch();

	void BeginJump();

	void EndJump();

	void Sprint();
	void StopSprint();

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Character Movement")
		float SprintSpeed;
#pragma endregion
#pragma region Cameras
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UCameraComponent* ThirdPersonCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UCameraComponent* FirstPersonCamera;

	UCameraComponent* ActiveCamera;
#pragma endregion
#pragma region Actor Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USHealthComponent* HealthComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
		class ULoadoutComponent* Loadout;

	UPROPERTY(BlueprintReadOnly, Category = "Loadout")
		class USLoadoutItemObjectWeapon* EquipppedWeapon;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	//	TSubclassOf<ASRangedWeapon> firstWeapon;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	//	TSubclassOf<ASRangedWeapon> secondWeapon;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout", Replicated)
	ASRangedWeapon* CurrentWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player")
	FName WeaponAttachSocketName;
#pragma endregion
#pragma region Weapon Switching
	UFUNCTION(BlueprintImplementableEvent)
	void OnSwitchWeapon();

	UFUNCTION(Server,Reliable, WithValidation)
		void ServerSwitchWeapon(int32 LoadoutIndex);

	void SwitchWeapon(int32 LoadoutIndex);
	void SwitchOne();
	void SwitchTwo();
	void SwitchThree();
	void SwitchFour();
#pragma endregion
#pragma region Animation Driving Bools and Helper Functions
	UPROPERTY(BlueprintReadWrite, Category = "Player")
	bool bAimDownSights;
	float DefaultFOV;
	UPROPERTY(EditDefaultsOnly, Category = "Player Settings")
		float ZoomedFOV;
	FTimerHandle ReloadTimer;

	UPROPERTY(BlueprintReadOnly, Category = "Player", Replicated)
		bool bIsReloading;
	UPROPERTY(EditDefaultsOnly, Category = "Player Settings", meta = (ClampMin = 0.1, ClampMax = 100))
		float ZoomInterpSpeed;
	UFUNCTION(BlueprintCallable)
	void BeginZoom();
	UFUNCTION(BlueprintCallable)
	void EndZoom();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerReload();

	UPROPERTY(BlueprintReadOnly, Category = "Player", Replicated)
		bool bDied;


	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	//USkeletalMeshComponent* MeshComponent;

	UFUNCTION()
		void SwitchCamera();
	UFUNCTION(BlueprintCallable)
	void StartReload();
	void Reload();
#pragma endregion
	UFUNCTION()
	void OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
	
	
	
	

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void DeductAmmo();

	UPROPERTY(BlueprintReadOnly, Category = "Weapon", Replicated)
		int32 CurrentAmmo;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon", Replicated)
		int32 DefaultAmmo;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon", Replicated)
		int32 TotalAmmo;
	
	//Infinite Ammo bool to allow infinite ammo power up
	UPROPERTY(BlueprintReadWrite, Category = "PowerUp", Replicated)
		bool bHasInfiniteAmmo;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual FVector GetPawnViewLocation() const override;

	UFUNCTION(BlueprintCallable)
		ASRangedWeapon* GetCurrentWeapon();

	UFUNCTION(BlueprintCallable, Category = "Player")
		void StartFire();

	UFUNCTION(BlueprintCallable, Category = "Player")
		void StopFire();
};
