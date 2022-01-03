// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/SLoadoutItemObjectWeapon.h"
#include "Net/UnrealNetwork.h"
void USLoadoutItemObjectWeapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	//Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USLoadoutItemObjectWeapon, Ammo);

}