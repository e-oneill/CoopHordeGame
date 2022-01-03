// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLoadoutItemObject.h"
#include "SLoadoutItemObjectWeapon.generated.h"

/**
 * 
 */

class ASRangedWeapon;

UCLASS()
class COOPHORDEGAME_API USLoadoutItemObjectWeapon : public USLoadoutItemObject
{
	GENERATED_BODY()

public: 
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
		class USkeletalMesh* PickUpMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
		TSubclassOf<ASRangedWeapon> Weapon;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Item", Replicated)
		int32 Ammo;
};
