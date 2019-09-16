// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "OthelloBlock.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OthelloBlockGrid.generated.h"

/** Class used to spawn blocks and manage score */
UCLASS(minimalapi)
class AOthelloBlockGrid : public AActor
{
	GENERATED_BODY()

	/** Dummy root component */
	UPROPERTY(Category = Grid, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* DummyRoot;

	/** Text component for the score */
	UPROPERTY(Category = Grid, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UTextRenderComponent* TurnText;

public:
	AOthelloBlockGrid();

	/** How many blocks have been clicked */
	int32 Score;

	/** Number of blocks along each side of grid */
	UPROPERTY(Category = Grid, EditAnywhere, BlueprintReadOnly)
	static int32 Size;

	/** Spacing of blocks */
	UPROPERTY(Category = Grid, EditAnywhere, BlueprintReadOnly)
	float BlockSpacing;

	/** Activated after 'AOthelloBlock::PutStone()' activates */
	UFUNCTION(BlueprintImplementableEvent, Category = "Tensorflow")
	void AfterPutStone_Black();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void UpdatePutablePosition();

	AOthelloBlock* GetBlockS(int index);

	UFUNCTION(BlueprintCallable, Category = "Game")
	void ResetGrid();

	UFUNCTION(BlueprintCallable, Category = "Game")
	EStoneColor WhatColorIsMore();

protected:
	// Begin AActor interface
	virtual void BeginPlay() override;
	// End AActor interface

	UPROPERTY(BlueprintReadOnly)
	TArray<AOthelloBlock*> BlockArray;

	UPROPERTY(BlueprintReadOnly)
	TArray<bool> IsPutable;

public:
	/** Check stone can be put on (start_x, start_y)
	* @return true stone can be put on this position
	* @return false stone can't be put on this position
	*/
	UFUNCTION(BlueprintCallable, Category = "Check")
	bool CheckPossibility(int start_x, int start_y);

	/** Change color of stones which lie on 8-directions */
	UFUNCTION(BlueprintCallable, Category = "Game")
	int ChangeStonesColor(uint8 stone_x, uint8 stone_y);

	/** Check whether game is over or not
	* @return true game is over
	* @return false game is not over
	*/
	UFUNCTION(BlueprintCallable, Category="Game")
	bool CheckGameOver();

	UPROPERTY(BlueprintReadOnly)
	int Reward = 0;

	/** Get block on (x, y) */
	UFUNCTION(BlueprintCallable, Category = "Block")
	AOthelloBlock* GetBlock(int x, int y);

	/** Returns DummyRoot subobject **/
	FORCEINLINE class USceneComponent* GetDummyRoot() const { return DummyRoot; }
	/** Returns ScoreText subobject **/
	FORCEINLINE class UTextRenderComponent* GetTurnText() const { return TurnText; }

public:
	UFUNCTION(BlueprintPure, Category = "Tensorflow")
	int GetRoundNumber();

	UFUNCTION(BlueprintPure, Category = "Tensorflow")
	TArray<float> GetState();

	UFUNCTION(BlueprintPure, Category = "Tensorflow")
	bool IsGameOver();

#pragma region Minimax Algorithm
	TArray<EStoneColor> AOthelloBlockGrid::GetTreeNodeScore(TArray<EStoneColor> board, uint8 stone_x, uint8 stone_y, int& changed_number);
	void AOthelloBlockGrid::GetTreeNodeScore()
#pragma endregion
};