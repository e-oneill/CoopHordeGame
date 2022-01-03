// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameMode.h"
#include "SCharacter.h"
#include "TimerManager.h"
#include "SHealthComponent.h"
#include "EngineUtils.h"
#include "SGameState.h"
#include "SPlayerState.h"
ASGameMode::ASGameMode()
{
	TimeBetweenWaves = 2.0f;
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;

	GameStateClass = ASGameState::StaticClass();
	PlayerStateClass = ASPlayerState::StaticClass();
	FriendlyFirePercentage = 0.f;
}



void ASGameMode::StartWave()
{
	if (WaveCount < NrOfWaves)
	{
		WaveCount++;
		//UE_LOG(LogTemp, Log, TEXT("Starting Wave %d"), WaveCount);
		NrOfBotsToSpawn = 2 * WaveCount;
		GetWorldTimerManager().SetTimer(TimerHandle_BotSpawner, this, &ASGameMode::SpawnBotTimerElapsed, 1.0f, true, 0.0f);
	}
	SetWaveState(EWaveState::WaveInProgress);
}

void ASGameMode::EndWave()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_BotSpawner);
	SetWaveState(EWaveState::WaitingToComplete);

	//PrepareForNextWave();
}

void ASGameMode::PrepareForNextWave()
{
	SetWaveState(EWaveState::WaitingToStart);
	RespawnDeadPlayers();
	GetWorldTimerManager().SetTimer(TimerHandle_NextWaveStart, this, &ASGameMode::StartWave, TimeBetweenWaves, false);
	
}

void ASGameMode::CheckWaveState()
{
	bool bIsPreparingWave = GetWorldTimerManager().IsTimerActive(TimerHandle_NextWaveStart);

	if (NrOfBotsToSpawn > 0 || bIsPreparingWave)
	{
		return;
	}
	bool bIsBotAlive = false;
	//Create Iterator of all pawns in game world.
		//for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; It++)
	//
	for (TActorIterator<APawn> It(GetWorld()); It; ++It)
	{
		APawn* TestPawn = *It;
		if (TestPawn == nullptr || TestPawn->IsPlayerControlled())
		{
			continue;
		}

		USHealthComponent* HC = Cast<USHealthComponent>(TestPawn->GetComponentByClass(USHealthComponent::StaticClass()));
		if (HC && HC->GetHealth() > 0)
		{
			bIsBotAlive = true;
			break;
		}
	}

	if (!bIsBotAlive && !bIsPreparingWave)
	{
		
		SetWaveState(EWaveState::WaveComplete);
		PrepareForNextWave();
	}
}

void ASGameMode::CheckAnyPlayerAlive()
{
	bool bIsPlayerAlive;
	for (TActorIterator<ASCharacter> It(GetWorld()); It; ++It)
	{
		ASCharacter* TestCharacter = *It;
		if (TestCharacter == nullptr || !TestCharacter->IsPlayerControlled())
		{
			continue;
		}

		USHealthComponent* HC = Cast<USHealthComponent>(TestCharacter->GetComponentByClass(USHealthComponent::StaticClass()));
		if (HC && HC->GetHealth() > 0)
		{
			bIsPlayerAlive = true;
			return;
		}
	}

	GameOver();
}

void ASGameMode::GameOver()
{
	EndWave();
	UE_LOG(LogTemp, Log, TEXT("Game Over"));
	SetWaveState(EWaveState::GameOver);
}

void ASGameMode::SetWaveState(EWaveState NewState)
{
	ASGameState* GS = GetGameState<ASGameState>();
	if (ensureAlways(GS))
	{
		GS->SetWaveState(NewState);
	}
}

void ASGameMode::RespawnDeadPlayers()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC && PC->GetPawn() == nullptr)
		{
			RestartPlayer(PC);
		}

		//USHealthComponent* HC = Cast<USHealthComponent>(TestCharacter->GetComponentByClass(USHealthComponent::StaticClass()));
		//if (HC && HC->GetHealth() > 0)
		//{
		//	bIsPlayerAlive = true;
		//	return;
		//}
	}
}

void ASGameMode::StartPlay()
{
	Super::StartPlay();
	SetWaveState(EWaveState::WaitingToStart);
	PrepareForNextWave();
}

void ASGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CheckWaveState();
	CheckAnyPlayerAlive();
}

int32 ASGameMode::GetBotsLeftToSpawn() const
{
	return NrOfBotsToSpawn;
}

int32 ASGameMode::GetWaveCount() const
{
	return WaveCount;
}

float ASGameMode::GetFriendlyFirePercentage()
{
	return FriendlyFirePercentage;
}

void ASGameMode::SpawnBotTimerElapsed()
{
	NrOfBotsToSpawn--;
	if (NrOfBotsToSpawn <= 0)
	{
		EndWave();
	}
	SpawnNewBot();
	//UE_LOG(LogTemp, Log, TEXT("Spawning Bot for Wave %d, there are %d Bots left to spawn"), WaveCount, NrOfBotsToSpawn);
}