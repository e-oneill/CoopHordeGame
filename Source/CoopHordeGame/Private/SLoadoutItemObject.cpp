// Fill out your copyright notice in the Description page of Project Settings.


#include "SLoadoutItemObject.h"
#include "Net/UnrealNetwork.h"
USLoadoutItemObject::USLoadoutItemObject()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	ItemName = FText::FromString("Item");
	
}

bool USLoadoutItemObject::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	return false;
}
