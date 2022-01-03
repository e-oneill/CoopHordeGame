// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SGunAnimationPack.generated.h"

class UAnimMontage;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COOPHORDEGAME_API USGunAnimationPack : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USGunAnimationPack();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	




public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		UAnimMontage* HipAnimation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		UAnimMontage* Ironsights;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		UAnimMontage* Fire;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		UAnimMontage* IronsightsFire;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		UAnimMontage* Reload;
		
};
