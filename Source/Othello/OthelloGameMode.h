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
	int GameTurnCount;

	bool IsTurnOmitted;

public:
	AOthelloGameMode();

	/** Omit turn */
	UFUNCTION(BlueprintCallable, Category="Mode")
	int OmitTurn();

	/** Turn over & change turn to opponent player */
	int NextTurn();

	/** Declare game over & update game status */
	void GameOver();

	bool IsGameOver();

	void Reset();

	/** Returns 'GameTurn' */
	FORCEINLINE ETurn GetTurn() const { return GameTurn; }
	/** Returns 'GameTurnCount' */
	FORCEINLINE uint16 GetTurnCount() const { return GameTurnCount; }
	/** Returns 'IsTurnOmitted' */
	FORCEINLINE bool GetTurnOmit() const { return IsTurnOmitted; }
	/** Returns 'GameStatus' */
	FORCEINLINE EGameStatus GetGameStatus() const{ return GameStatus; }
};