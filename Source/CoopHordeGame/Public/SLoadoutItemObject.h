// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SLoadoutItemObject.generated.h"

/**
 * 
 */

class ASRangedWeapon;

UCLASS(Abstract, BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class COOPHORDEGAME_API USLoadoutItemObject : public UObject
{
	GENERATED_BODY()

public:
	USLoadoutItemObject();

	virtual class UWorld* GetWorld() const { return World; };

	UPROPERTY(Transient)
		class UWorld* World;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
		FText ItemName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
		class UTexture2D* Thumbnail;

	virtual bool IsSupportedForNetworking() const { return true; };

	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags); // note no override because this is the FIRST declaration of this function

	UPROPERTY()
	class ULoadoutComponent* OwningLoadout;
};
