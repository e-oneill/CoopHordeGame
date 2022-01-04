// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SGameMode.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnActorKilled, AActor*, VictimActor, AActor*, KillerActor); //What we want to send: Killed Actor, Killer, Place where shot hit killed actor?

enum class EWaveState : uint8;

UCLASS()
class COOPHORDEGAME_API ASGameMode : public AGameModeBase
{
	GENERATED_BODY()
	

protected:
	//Hook for BP to spawn a single bot
	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
	void SpawnNewBot();

	void SpawnBotTimerElapsed();

	FTimerHandle TimerHandle_BotSpawner;
	FTimerHandle TimerHandle_NextWaveStart;

	UPROPERTY(EditDefaultsOnly, Category = "GameMode")
	float TimeBetweenWaves;

	//Number of bots to spawn in current wave
	int32 NrOfBotsToSpawn;
	int32 WaveCount;

	UPROPERTY(EditDefaultsOnly, Category = "GameMode")
	int32 NrOfWaves;
	void StartWave();

	void EndWave();

	//Set timer for next start wave
	void PrepareForNextWave();

	void CheckWaveState();

	void CheckAnyPlayerAlive();

	void GameOver();

	void SetWaveState(EWaveState NewState);

	void RespawnDeadPlayers();
	UPROPERTY(BlueprintReadOnly, meta = (ClampMin = 0.f, ClampMax = 1.0f))
	float FriendlyFirePercentage;

public:
	ASGameMode();

	virtual void StartPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable)
	int32 GetBotsLeftToSpawn() const;

	UFUNCTION(BlueprintCallable)
		int32 GetWaveCount() const;

	UPROPERTY(BlueprintAssignable, Category = "GameMode")
	FOnActorKilled OnActorKilled;

	UFUNCTION()
	float GetFriendlyFirePercentage();

	UFUNCTION(BlueprintCallable)
	float GetTimeBetweenWaves() const;
};
