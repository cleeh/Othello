// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "OthelloGameMode.generated.h"

UENUM()
enum class EGameStatus
{
	Play,
	GameOver
};

UENUM()
enum class ETurn
{
	Black,
	White
};

/** GameMode class to specify pawn and playercontroller */
UCLASS(minimalapi)
class AOthelloGameMode : public AGameModeBase
{
	GENERATED_BODY()

private:
	/** Game Status */
	EGameStatus GameStatus;

	/** Who's turn? */
	ETurn GameTurn;

	/** This increases whenever turn is changed */
	uint16 GameTurnCount;

	bool IsTurnOmitted;
	
public:
	AOthelloGameMode();

	/** Omit turn */
	uint16 OmitTurn();

	/** Turn over & change turn to opponent player */
	uint16 NextTurn();

	/** Declare game over & update game status */
	void GameOver();

	/** Returns 'GameTurn' */
	FORCEINLINE ETurn GetTurn() const { return GameTurn; }
	/** Returns 'GameTurnCount' */
	FORCEINLINE uint16 GetTurnCount() const { return GameTurnCount; }
	/** Returns 'IsTurnOmitted' */
	FORCEINLINE bool GetTurnOmit() const { return IsTurnOmitted; }

	
};



