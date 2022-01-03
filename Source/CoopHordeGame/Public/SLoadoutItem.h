// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SLoadoutItem.generated.h"

class ASRangedWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COOPHORDEGAME_API USLoadoutItem : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USLoadoutItem();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	//Adding bool in case we later decide to add other types of items. For now all loadout items are weapons
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	bool IsWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout", meta = (EditCondition = "isWeapon"))
		TSubclassOf<ASRangedWeapon> Weapon;

	//The total amount of ammo the player is carrying for this weapon
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout", meta = (EditCondition = "isWeapon"))
		int32 Ammo;

public:	

	UFUNCTION()
		TSubclassOf<ASRangedWeapon> GetWeapon();

	UFUNCTION()
		int32 GetAmmo();
		
};
