// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LoadoutComponent.generated.h"

//Can be used if we want to update the UI via blueprint
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoadoutChanged);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COOPHORDEGAME_API ULoadoutComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULoadoutComponent();

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Inventory")
	TArray<class USLoadoutItemObject*> DefaultItems;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	int32 Capacity;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
		FOnLoadoutChanged OnLoadoutChanged;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Items")
		TArray<class USLoadoutItemObject*> Items;

	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags);

	bool AddItem(class USLoadoutItemObject* Item);
	bool RemoveItem(class USLoadoutItemObject* Item);
	bool SwapItems(class USLoadoutItemObject* NewItem, class USLoadoutItemObject* OldItem);
		
};
