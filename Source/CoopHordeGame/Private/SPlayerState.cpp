// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"
//#include "SGameMode.h"
//#include "SGameState.h"

void ASPlayerState::AddScore(float ScoreDelta)
{
	SetScore(Score + ScoreDelta);
}
