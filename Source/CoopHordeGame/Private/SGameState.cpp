// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameState.h"
#include "Net/UnrealNetwork.h"
#include "SPlayerState.h"
void ASGameState::OnRep_WaveState(EWaveState OldState)
{
	WaveStateChanged(WaveState, OldState);
}

void ASGameState::SetWaveState(EWaveState NewState)
{
	//We have to call the on rep to make sure it is called by a server (e.g. a listen server)
	if (HasAuthority())
	{
		EWaveState OldState = WaveState;
		WaveState = NewState;
		OnRep_WaveState(OldState);
	}
	
}


void ASGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASGameState, WaveState);

}
