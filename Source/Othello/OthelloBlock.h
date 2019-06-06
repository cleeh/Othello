// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OthelloBlock.generated.h"

UENUM(BlueprintType)
enum class EStoneColor : uint8
{
	None = 0,
	Black = 127,
	White = 254
};

/** A block that can be clicked */
UCLASS(minimalapi)
class AOthelloBlock : public AActor
{
	GENERATED_BODY()

	/** Dummy root component */
	UPROPERTY(Category = Block, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* DummyRoot;

	/** StaticMesh component for the clickable block */
	UPROPERTY(Category = Block, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* BlockMesh;

	/** StaticMesh component for the stone put on block */
	UPROPERTY(Category = Block, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* StoneMesh;

private:
	/** What stone is put on block? */
	EStoneColor WhatStoneColor;

public:
	AOthelloBlock();

	UPROPERTY(BlueprintReadOnly, Category = "Grid")
	int X;

	UPROPERTY(BlueprintReadOnly, Category = "Grid")
	int Y;

	/** Are we currently active? */
	bool bIsActive;

	/** Pointer to white material used on the focused block */
	UPROPERTY()
	class UMaterial* BaseMaterial;

	/** Pointer to blue material used on inactive blocks */
	UPROPERTY()
	class UMaterialInstance* BlueMaterial;

	/** Pointer to orange material used on active blocks */
	UPROPERTY()
	class UMaterialInstance* OrangeMaterial;

	/** Pointer to white material used on white stone */
	UPROPERTY()
	class UMaterialInstance* WhiteMaterial;

	/** Pointer to black material used on black stone */
	UPROPERTY()
	class UMaterialInstance* BlackMaterial;

	/** Grid that owns us */
	UPROPERTY()
	class AOthelloBlockGrid* OwningGrid;

	/** Handle the block being clicked */
	UFUNCTION()
	void BlockClicked(UPrimitiveComponent* ClickedComp, FKey ButtonClicked);

	/** Handle the block being touched  */
	UFUNCTION()
	void OnFingerPressedBlock(ETouchIndex::Type FingerIndex, UPrimitiveComponent* TouchedComponent);

	/** Activated when instances of this class are clicked */
	void HandleClicked();

	void Highlight(bool bOn);

	/** Put a stone on block & update variables according to game rule */
	UFUNCTION(BlueprintCallable, Category = "Block")
	bool PutStone();

	/** Put a stone having specific color on block forcefully ignoring game rule */
	UFUNCTION(BlueprintCallable, Category = "Block")
	void PutStoneF(EStoneColor color);

	/** Clear stone on block & update variables */
	void ClearStone();

	/** Change color of stone on block according to present status */
	void ChangeStone();

public:
	/** Returns DummyRoot subobject **/
	FORCEINLINE class USceneComponent* GetDummyRoot() const { return DummyRoot; }
	/** Returns BlockMesh subobject **/
	FORCEINLINE class UStaticMeshComponent* GetBlockMesh() const { return BlockMesh; }
	/** Returns 'WhatStoneColor' */
	UFUNCTION(BlueprintCallable, Category = "Block")
	FORCEINLINE EStoneColor GetStoneColor() const { return WhatStoneColor; }
};