// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SGameState.generated.h"

/**
 * 
 */
//Enum to track the state of the game, blueprinttype so we can use it do drive UIs
UENUM(BlueprintType)
enum class EWaveState : uint8
{
	WaitingToStart,
	WaveInProgress,
	//No longer spawning new Bots, waiting for players to kill all bots
	WaitingToComplete,
	WaveComplete,
	GameOver,
};

class ASPlayerState;

UCLASS()
class COOPHORDEGAME_API ASGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_WaveState, Category = "GameState")
	EWaveState WaveState;

protected:
	//Passing in the previous value to OnRep allows us to use it
	UFUNCTION()
	void OnRep_WaveState(EWaveState OldState);
	

public:
	void SetWaveState(EWaveState NewState);

	UFUNCTION(BlueprintImplementableEvent, Category = "GameState")
		void WaveStateChanged(EWaveState NewState, EWaveState OldState);

};
