// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SPlayerState.generated.h"

//enum class EWaveState : uint8;

UCLASS()
class COOPHORDEGAME_API ASPlayerState : public APlayerState
{
	GENERATED_BODY()
	

public:
	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	void AddScore(float ScoreDelta);

	

};
