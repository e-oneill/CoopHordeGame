// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SHealthComponent.generated.h"

//OnHealth Changed Event
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedSignature, USHealthComponent*, HealthComp, float, Health, float, HealthDelta, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);
//OnShield Changed Event
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnShieldChangedSignature, USHealthComponent*, HealthComp, float, Shield, float, ShieldDelta, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);
UCLASS( ClassGroup=(COOP), meta=(BlueprintSpawnableComponent) )
class COOPHORDEGAME_API USHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USHealthComponent();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health Component")
	uint8 TeamNum;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, Category = "Stats", ReplicatedUsing=OnRep_Health)
	float Health;

	UFUNCTION()
	void OnRep_Health(float OldHealth);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHealth;

	UPROPERTY(BlueprintReadWrite, Category = "Stats", ReplicatedUsing = OnRep_Shield)
		float Shield;

	//Seconds to wait before starting shield regen. Damage will reset this timer;
	UPROPERTY(EditAnywhere, Category = "Stats")
	float ShieldRegenDelay;

	//Seconds it takes for Shield to fully recharge
	UPROPERTY(EditAnywhere, Category = "Stats")
	float ShieldRegenDuration;

	FTimerHandle TimerHandle_StartShieldRegen;

	void StartShieldRegen();
	//Bool that drives whether shield should regenerate
	bool bShieldRegen;
	//Float used to track shield regen in C++
	float ShieldSecondRegen;

	UFUNCTION()
		void OnRep_Shield(float OldHealth);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		float MaxShield;

	UFUNCTION()
	void handleTakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	bool bIsDead;
public:	
	// Called every frame
	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnHealthChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnShieldChangedSignature OnShieldChanged;

	UFUNCTION(BlueprintCallable, Category = "Health Component")
	void Heal(float HealAmount);
	UFUNCTION(BlueprintCallable)
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Health Component")
	static bool IsFriendly(AActor* ActorA, AActor* ActorB);
};
