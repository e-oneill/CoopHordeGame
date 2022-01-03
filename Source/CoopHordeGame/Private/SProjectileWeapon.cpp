// Fill out your copyright notice in the Description page of Project Settings.


#include "SProjectileWeapon.h"
//#include "SProjectile.h"
#include "SCharacter.h"
void ASProjectileWeapon::Fire()
{

	if (!HasAuthority())
	{
		ServerFire();
		return;
	}

	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		ASCharacter* MyChar = Cast<ASCharacter>(MyOwner);
		if (MyChar)
		{
			if (MyChar->CurrentAmmo > 0)
			{
				FVector EyeLocation;
				FRotator EyeRotation;
				MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);
				if (Projectile)
				{
					FVector MuzzleLocation = MeshComponent->GetSocketLocation(MuzzleSocket);
					//FRotator MuzzleRotation = MeshComponent->GetSocketRotation(MuzzleSocket);

					FActorSpawnParameters ActorSpawnParams;
					ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
					ActorSpawnParams.Owner = MyOwner;
					ActorSpawnParams.Instigator = MyOwner->GetInstigator();

					GetWorld()->SpawnActor<AActor>(Projectile, MuzzleLocation, EyeRotation, ActorSpawnParams);
					CurrentAmmo--;
					MyChar->DeductAmmo();

				}
			}
		}
	}


}
