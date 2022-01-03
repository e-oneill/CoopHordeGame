// Fill out your copyright notice in the Description page of Project Settings.


#include "LoadoutComponent.h"
#include "SLoadoutItemObject.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"
// Sets default values for this component's properties
ULoadoutComponent::ULoadoutComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	Capacity = 4;
	// ...
	
}




// Called when the game starts
void ULoadoutComponent::BeginPlay()
{
	Super::BeginPlay();
	for (auto& Item : DefaultItems)
	{
		AddItem(Item);
	}
	SetIsReplicated(true);
	// ...
	
}

//bool ULoadoutComponent::ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
//{
//	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
//	if (Items) WroteSomething |= Channel->ReplicateSubobject(Items, *Bunch, *RepFlags);
//
//	return WroteSomething;
//}

bool ULoadoutComponent::AddItem(class USLoadoutItemObject* Item)
{
	if (!Item)
	{
		return false;
	}
	if (Items.Num() >= Capacity)
	{
		//Add Swap functionality soon
		return false;
	}
	Item->OwningLoadout = this;
	Item->World = GetWorld();
	Items.Add(Item);

	OnLoadoutChanged.Broadcast();

	return true;
}

bool ULoadoutComponent::RemoveItem(class USLoadoutItemObject* Item)
{
	if (Item)
	{
		Item->OwningLoadout = nullptr;
		Item->World = nullptr;
		Items.RemoveSingle(Item);
		OnLoadoutChanged.Broadcast();
		return true;
	}
	return false;
}

bool ULoadoutComponent::SwapItems(class USLoadoutItemObject* NewItem, class USLoadoutItemObject* OldItem)
{

	return false;
}


void ULoadoutComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	//Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ULoadoutComponent, Items);

}

bool ULoadoutComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool wroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (USLoadoutItemObject* MyObject : Items)
	{
		if (MyObject)
		{
			Channel->ReplicateSubobject(MyObject, *Bunch, *RepFlags);
		}
	}

	//Channel->ReplicateSubobjectList(Items, Bunch, RepFlags );

	//wroteSomething |= Channel->ReplicateSubobjects()
	return wroteSomething;
}
