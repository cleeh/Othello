// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "OthelloBlockGrid.h"
#include "OthelloBlock.h"
#include "Components/TextRenderComponent.h"
#include "Engine/World.h"
#include "OthelloGameMode.h"

#define LOCTEXT_NAMESPACE "PuzzleBlockGrid"

AOthelloBlockGrid::AOthelloBlockGrid()
{
	// Create dummy root scene component
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	RootComponent = DummyRoot;

	// Create static mesh component
	ScoreText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("ScoreText0"));
	ScoreText->SetRelativeLocation(FVector(200.f,0.f,0.f));
	ScoreText->SetRelativeRotation(FRotator(90.f,0.f,0.f));
	ScoreText->SetText(FText::Format(LOCTEXT("ScoreFmt", "Score: {0}"), FText::AsNumber(0)));
	ScoreText->SetupAttachment(DummyRoot);

	// Set defaults
	Size = 8;
	BlockSpacing = 300.f;
}


void AOthelloBlockGrid::BeginPlay()
{
	Super::BeginPlay();

	// Number of blocks
	const int32 NumBlocks = Size * Size;

	BlockArray.SetNum(NumBlocks);

	// Loop to spawn each block
	for(int32 BlockIndex=0; BlockIndex<NumBlocks; BlockIndex++)
	{
		const float XOffset = (BlockIndex/Size) * BlockSpacing; // Divide by dimension
		const float YOffset = (BlockIndex%Size) * BlockSpacing; // Modulo gives remainder

		// Make position vector, offset from Grid location
		const FVector BlockLocation = FVector(XOffset, YOffset, 0.f) + GetActorLocation();

		// Spawn a block
		AOthelloBlock* NewBlock = GetWorld()->SpawnActor<AOthelloBlock>(BlockLocation, FRotator(0,0,0));

		// Tell the block about its owner
		if (NewBlock != nullptr)
		{
			NewBlock->OwningGrid = this;
			NewBlock->X = BlockIndex / Size;
			NewBlock->Y = BlockIndex % Size;
			BlockArray[BlockIndex] = NewBlock;
		}

		// Put stones on board depending on the rule for othello
		if (BlockIndex / Size == 3 && BlockIndex % Size == 3 || BlockIndex / Size == 4 && BlockIndex % Size == 4)
			NewBlock->PutStone(EStoneColor::White);
		else if (BlockIndex / Size == 3 && BlockIndex % Size == 4 || BlockIndex / Size == 4 && BlockIndex % Size == 3)
			NewBlock->PutStone(EStoneColor::Black);
	}
}


void AOthelloBlockGrid::AddScore()
{
	// Increment score
	Score++;

	// Update text
	ScoreText->SetText(FText::Format(LOCTEXT("ScoreFmt", "Score: {0}"), FText::AsNumber(Score)));
}

void AOthelloBlockGrid::ChangeStonesColor(uint8 stone_x, uint8 stone_y)
{
	EStoneColor target_color = GetBlock(stone_x, stone_y)->WhatStoneColor;
	EStoneColor other_color;
	if (target_color == EStoneColor::Black) other_color = EStoneColor::White;
	else if (target_color == EStoneColor::White) other_color = EStoneColor::Black;

	TArray<bool> StoneToChange; // 개선이 필요하다
	StoneToChange.Empty();
	StoneToChange.SetNum(Size*Size);

	// Western
	for (int x = stone_x - 1; x >= 0; x--)
	{
		// same stone color
		if (GetBlock(x, stone_y)->WhatStoneColor == target_color)
		{
			break;
		}
		// other stone color
		else if (GetBlock(x, stone_y)->WhatStoneColor == other_color)
		{
			if (x > 0) StoneToChange[stone_y*Size + x] = true;
			else
			{
				for (int i = 0; i < stone_x; i++) StoneToChange[stone_y*Size + i] = false;
				break;
			}
		}
		// no stone
		else if (GetBlock(x, stone_y)->WhatStoneColor == EStoneColor::None)
		{
			for (int i = x; i < stone_x; i++) StoneToChange[stone_y*Size + i] = false;
			break;
		}
	}

	// Eastern
	for (int x = stone_x +1; x < Size; x++)
	{
		// same stone color
		if (GetBlock(x, stone_y)->WhatStoneColor == target_color)
		{
			break;
		}
		// other stone color
		else if (GetBlock(x, stone_y)->WhatStoneColor == other_color)
		{
			if (x < Size - 1) StoneToChange[stone_y*Size + x] = true;
			else
			{
				for (int i = Size - 1; i > stone_x; i--) StoneToChange[stone_y*Size + i] = false;
				break;
			}
		}
		// no stone
		else if (GetBlock(x, stone_y)->WhatStoneColor == EStoneColor::None)
		{
			for (int i = x; i > stone_x; i--) StoneToChange[stone_y*Size + i] = false;
			break;
		}
	}

	// Northern
	for (int y = stone_y + 1; y < Size; y++)
	{
		// same stone color
		if (GetBlock(stone_x, y)->WhatStoneColor == target_color)
		{
			break;
		}
		// other stone color
		else if (GetBlock(stone_x, y)->WhatStoneColor == other_color)
		{
			if (y < Size - 1) StoneToChange[y*Size + stone_x] = true;
			else
			{
				for (int i = Size - 1; i > stone_y; i--) StoneToChange[i*Size + stone_x] = false;
				break;
			}
		}
		// no stone
		else if (GetBlock(stone_x, y)->WhatStoneColor == EStoneColor::None)
		{
			for (int i = y; i > stone_y; i--) StoneToChange[i*Size + stone_x] = false;
		}
	}

	// Southern
	for (int y = stone_y - 1; y >= 0; y--)
	{
		if (GetBlock(stone_x, y))
		{
			// same stone color
			if (GetBlock(stone_x, y)->WhatStoneColor == target_color)
			{
				break;
			}
			// other stone color
			else if (GetBlock(stone_x, y)->WhatStoneColor == other_color)
			{
				if (y > 0) StoneToChange[y*Size + stone_x] = true;
				else
				{
					for (int i = 0; i < stone_y; i++) StoneToChange[i*Size + stone_x] = false;
					break;
				}
			}
			// no stone
			else if (GetBlock(stone_x, y)->WhatStoneColor == EStoneColor::None)
			{
				for (int i = y; i < stone_y; i++) StoneToChange[i*Size + stone_x] = false;
			}
		}
	}

	for (int y = 0; y < Size; y++)
		for (int x = 0; x < Size; x++)
			if (StoneToChange[y*Size + x]) GetBlock(x, y)->ChangeStone();
}

AOthelloBlock* AOthelloBlockGrid::GetBlock(uint8 x, uint8 y)
{
	const int target = x * Size + y;
	if (target < BlockArray.Num() && target >= 0)
		return BlockArray[target];

	return nullptr;
}

#undef LOCTEXT_NAMESPACE
