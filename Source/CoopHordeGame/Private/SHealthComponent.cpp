// Fill out your copyright notice in the Description page of Project Settings.


#include "SHealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "SGameMode.h"
// Sets default values for this component's properties
USHealthComponent::USHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	MaxHealth = 100;
	Health = MaxHealth;
	// ...

	bIsDead = false;
	TeamNum = 255;
}


// Called when the game starts
void USHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	SetIsReplicated(true);
	// We don't need to protect the fire add damage because we are limiting the health component to the server
	if (GetOwnerRole() == ROLE_Authority)
	{
		AActor* MyOwner = GetOwner();
		if (MyOwner)
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &USHealthComponent::handleTakeDamage);
		}
	}
	
}

void USHealthComponent::OnRep_Health(float OldHealth)
{
	float Damage = Health - OldHealth;
	OnHealthChanged.Broadcast(this, Health, Damage, nullptr, nullptr, nullptr);
}

void USHealthComponent::handleTakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (DamageCauser != DamagedActor && IsFriendly(DamageCauser, DamagedActor))
	{
		ASGameMode* GM = Cast<ASGameMode>(GetWorld()->GetAuthGameMode());
		if (GM)
		{
			Damage = Damage * GM->GetFriendlyFirePercentage();
		}

	}
	if (Damage <= 0.0f||bIsDead)
	{

		return;
	}
	Health = FMath::Clamp(Health - Damage, 0.0f, MaxHealth);

	UE_LOG(LogTemp, Log, TEXT("%s Damage Taken, new Health is %s"), *FString::SanitizeFloat(Damage), *FString::SanitizeFloat(Health));

	OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);
	if (Health <= 0.0f && !bIsDead)
	{
		ASGameMode* GM = Cast<ASGameMode>(GetWorld()->GetAuthGameMode());
		if (GM)
		{
			GM->OnActorKilled.Broadcast(DamagedActor, DamageCauser);
		}
		bIsDead = true;
	}
}

void USHealthComponent::Heal(float HealAmount)
{
	if (HealAmount <= 0.0f || Health <= 0.0f)
	{
		return;
	}

	Health = FMath::Clamp(Health + HealAmount, 0.0f, MaxHealth);

	OnHealthChanged.Broadcast(this, Health, 0.0f, nullptr, nullptr, nullptr);
}


float USHealthComponent::GetHealth() const
{
	return Health;
}

bool USHealthComponent::IsFriendly(AActor* ActorA, AActor* ActorB)
{
	if (ActorA == nullptr || ActorB == nullptr)
	{
		return true;
	}
	USHealthComponent* ActorAHC = Cast<USHealthComponent>(ActorA->GetComponentByClass(USHealthComponent::StaticClass()));
	USHealthComponent* ActorBHC = Cast<USHealthComponent>(ActorB->GetComponentByClass(USHealthComponent::StaticClass()));
	if (ActorAHC == nullptr || ActorBHC == nullptr)
	{
		return true;
	}

	if (ActorAHC->TeamNum == ActorBHC->TeamNum)
	{
		return true;
	}
	return false;
}

//// Called every frame
//void USHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//	// ...
//}



void USHealthComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USHealthComponent, Health);

}