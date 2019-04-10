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

	GameTurn = ETurn::Black;
}