// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SRangedWeapon.h"
#include "SProjectile.h"
#include "SProjectileWeapon.generated.h"
/**
 * 
 */



UCLASS()
class COOPHORDEGAME_API ASProjectileWeapon : public ASRangedWeapon
{
	GENERATED_BODY()
	

protected:

	virtual void Fire() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		TSubclassOf<AActor> Projectile;

};
