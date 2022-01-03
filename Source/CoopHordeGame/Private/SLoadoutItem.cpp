// Fill out your copyright notice in the Description page of Project Settings.


#include "SLoadoutItem.h"
#include "SRangedWeapon.h"
// Sets default values for this component's properties
USLoadoutItem::USLoadoutItem()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	IsWeapon = true;
	Ammo = 0;
}


// Called when the game starts
void USLoadoutItem::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

TSubclassOf<ASRangedWeapon> USLoadoutItem::GetWeapon()
{
	if (Weapon)
	{
		return Weapon;
	}
	return nullptr;
}

int32 USLoadoutItem::GetAmmo()
{
	if (Ammo)
	{
		return Ammo;
	}
	return 0;
}

