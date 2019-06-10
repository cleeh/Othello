// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "OthelloGameMode.h"
#include "OthelloPlayerController.h"
#include "OthelloPawn.h"

AOthelloGameMode::AOthelloGameMode()
{
	// no pawn by default
	DefaultPawnClass = AOthelloPawn::StaticClass();
	// use our own player controller class
	PlayerControllerClass = AOthelloPlayerController::StaticClass();

	Reset();
}

int AOthelloGameMode::OmitTurn()
{
	// game is over if previous turn is omitted for reason that previous player can't put stone on any location
	// if (IsTurnOmitted) GameOver();
	// checksum reset if not
	// else IsTurnOmitted = true;
	IsTurnOmitted = true;
	if (GameTurn == ETurn::White) GameTurn = ETurn::Black;
	else if (GameTurn == ETurn::Black) GameTurn = ETurn::White;

	return GameTurnCount;
}

int AOthelloGameMode::NextTurn()
{
	IsTurnOmitted = false;

	if (GameTurn == ETurn::White) GameTurn = ETurn::Black;
	else if (GameTurn == ETurn::Black) GameTurn = ETurn::White;

	return ++GameTurnCount;
}

void AOthelloGameMode::GameOver()
{
	GameStatus = EGameStatus::GameOver;
	UE_LOG(LogTemp, Error, TEXT("Game Over"), TEXT("GameMode"));
}

bool AOthelloGameMode::IsGameOver()
{
	if(GameStatus == EGameStatus::GameOver)
		return true;
	return false;
}

void AOthelloGameMode::Reset()
{
	GameStatus = EGameStatus::Play;
	GameTurn = ETurn::Black;
	GameTurnCount = 1;

	IsTurnOmitted = false;
}