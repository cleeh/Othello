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
	UPROPERTY(Category=Grid, EditAnywhere, BlueprintReadOnly)
	int32 Size;

	/** Spacing of blocks */
	UPROPERTY(Category=Grid, EditAnywhere, BlueprintReadOnly)
	float BlockSpacing;

protected:
	// Begin AActor interface
	virtual void BeginPlay() override;
	// End AActor interface

	UPROPERTY()
	TArray<AOthelloBlock*> BlockArray;

public:

	bool CheckPossibility(uint8 start_x, uint8 start_y);

	/** Change color of stones which lie on 8-directions */
	void ChangeStonesColor(uint8 stone_x, uint8 stone_y);

	/** Get block on (x, y) */
	AOthelloBlock* GetBlock(uint8 x, uint8 y);

	/** Returns DummyRoot subobject **/
	FORCEINLINE class USceneComponent* GetDummyRoot() const { return DummyRoot; }
	/** Returns ScoreText subobject **/
	FORCEINLINE class UTextRenderComponent* GetTurnText() const { return TurnText; }
};



