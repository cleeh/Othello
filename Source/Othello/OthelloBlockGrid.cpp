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
	TurnText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("ScoreText0"));
	TurnText->SetRelativeLocation(FVector(200.f, 0.f, 0.f));
	TurnText->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
	TurnText->SetText(FText::Format(LOCTEXT("ScoreFmt", "Score: {0}"), FText::AsNumber(0)));
	TurnText->SetupAttachment(DummyRoot);

	// Set defaults
	Size = 8;
	BlockSpacing = 300.f;
}

void AOthelloBlockGrid::BeginPlay()
{
	// Number of blocks
	const int32 NumBlocks = Size * Size;

	BlockArray.SetNum(NumBlocks);
	IsPutable.SetNum(NumBlocks);

	// Loop to spawn each block
	for (int32 BlockIndex = 0; BlockIndex<NumBlocks; BlockIndex++)
	{
		const float XOffset = (BlockIndex % Size) * BlockSpacing;
		const float YOffset = (BlockIndex / Size) * BlockSpacing;

		// Make position vector, offset from Grid location
		const FVector BlockLocation = FVector(XOffset, YOffset, 0.f) + GetActorLocation();

		// Spawn a block
		AOthelloBlock* NewBlock = GetWorld()->SpawnActor<AOthelloBlock>(BlockLocation, FRotator(0, 0, 0));

		// Tell the block about its owner
		if (NewBlock != nullptr)
		{
			NewBlock->OwningGrid = this;
			NewBlock->X = BlockIndex % Size;
			NewBlock->Y = BlockIndex / Size;
			BlockArray[BlockIndex] = NewBlock;
		}

		// Put stones on board depending on the rule for othello
		if (BlockIndex / Size == 3 && BlockIndex % Size == 3 || BlockIndex / Size == 4 && BlockIndex % Size == 4)
			NewBlock->PutStoneF(EStoneColor::White);
		else if (BlockIndex / Size == 3 && BlockIndex % Size == 4 || BlockIndex / Size == 4 && BlockIndex % Size == 3)
			NewBlock->PutStoneF(EStoneColor::Black);
	}

	UpdatePutablePosition();

	Super::BeginPlay();
}

bool AOthelloBlockGrid::CheckPossibility(int start_x, int start_y)
{
	if (start_x < 0 || start_y < 0) return false;

	AOthelloGameMode* GameMode = Cast<AOthelloGameMode>(GetWorld()->GetAuthGameMode());
	EStoneColor target_color;
	EStoneColor other_color;
	if (GameMode->GetTurn() == ETurn::Black)	target_color = EStoneColor::Black;
	else if (GameMode->GetTurn() == ETurn::White) target_color = EStoneColor::White;
	if (target_color == EStoneColor::Black) other_color = EStoneColor::White;
	else if (target_color == EStoneColor::White) other_color = EStoneColor::Black;

	if (GetBlock(start_x, start_y)->GetStoneColor() != EStoneColor::None)
	{
		return false;
	}
	else
	{
		// West
		bool Flag = false;
		for (int x = start_x - 1; x >= 0; x--)
		{
			AOthelloBlock* Target = GetBlock(x, start_y);

			// same color
			if (Target->GetStoneColor() == target_color)
			{
				if (Flag) return true;
				else break;
			}
			// other color
			else if (Target->GetStoneColor() == other_color)
			{
				Flag = true;
			}
			// no color
			else if (Target->GetStoneColor() == EStoneColor::None)
			{
				break;
			}
		}

		// East
		Flag = false;
		for (int x = start_x + 1; x < Size; x++)
		{
			AOthelloBlock* Target = GetBlock(x, start_y);

			// same color
			if (Target->GetStoneColor() == target_color)
			{
				if (Flag) return true;
				else break;
			}
			// other color
			else if (Target->GetStoneColor() == other_color)
			{
				Flag = true;
			}
			// no color
			else if (Target->GetStoneColor() == EStoneColor::None)
			{
				break;
			}
		}

		// South
		Flag = false;
		for (int y = start_y - 1; y >= 0; y--)
		{
			AOthelloBlock* Target = GetBlock(start_x, y);

			// same color
			if (Target->GetStoneColor() == target_color)
			{
				if (Flag) return true;
				else break;
			}
			// other color
			else if (Target->GetStoneColor() == other_color)
			{
				Flag = true;
			}
			// no color
			else if (Target->GetStoneColor() == EStoneColor::None)
			{
				break;
			}
		}

		// North
		Flag = false;
		for (int y = start_y + 1; y < Size; y++)
		{
			AOthelloBlock* Target = GetBlock(start_x, y);

			// same color
			if (Target->GetStoneColor() == target_color)
			{
				if (Flag) return true;
				else break;
			}
			// other color
			else if (Target->GetStoneColor() == other_color)
			{
				Flag = true;
			}
			// no color
			else if (Target->GetStoneColor() == EStoneColor::None)
			{
				break;
			}
		}

		// Northern-West
		Flag = false;
		for (int x = start_x - 1, y = start_y + 1; x >= 0 && y < Size; x--, y++)
		{
			AOthelloBlock* Target = GetBlock(x, y);

			// same color
			if (Target->GetStoneColor() == target_color)
			{
				if (Flag) return true;
				else break;
			}
			// other color
			else if (Target->GetStoneColor() == other_color)
			{
				Flag = true;
			}
			// no color
			else if (Target->GetStoneColor() == EStoneColor::None)
			{
				break;
			}
		}

		// Northern-East
		Flag = false;
		for (int x = start_x + 1, y = start_y + 1; x < Size && y < Size; x++, y++)
		{
			AOthelloBlock* Target = GetBlock(x, y);

			// same color
			if (Target->GetStoneColor() == target_color)
			{
				if (Flag) return true;
				else break;
			}
			// other color
			else if (Target->GetStoneColor() == other_color)
			{
				Flag = true;
			}
			// no color
			else if (Target->GetStoneColor() == EStoneColor::None)
			{
				break;
			}
		}

		// Southern-West
		Flag = false;
		for (int x = start_x - 1, y = start_y - 1; x >= 0 && y >= 0; x--, y--)
		{
			AOthelloBlock* Target = GetBlock(x, y);

			// same color
			if (Target->GetStoneColor() == target_color)
			{
				if (Flag) return true;
				else break;
			}
			// other color
			else if (Target->GetStoneColor() == other_color)
			{
				Flag = true;
			}
			// no color
			else if (Target->GetStoneColor() == EStoneColor::None)
			{
				break;
			}
		}

		// Southern-East
		Flag = false;
		for (int x = start_x + 1, y = start_y - 1; x < Size && y >= 0; x++, y--)
		{
			AOthelloBlock* Target = GetBlock(x, y);

			// same color
			if (Target->GetStoneColor() == target_color)
			{
				if (Flag) return true;
				else break;
			}
			// other color
			else if (Target->GetStoneColor() == other_color)
			{
				Flag = true;
			}
			// no color
			else if (Target->GetStoneColor() == EStoneColor::None)
			{
				break;
			}
		}
	}

	return false;
}

int AOthelloBlockGrid::ChangeStonesColor(uint8 stone_x, uint8 stone_y)
{
	EStoneColor target_color = GetBlock(stone_x, stone_y)->GetStoneColor();
	EStoneColor other_color;
	if (target_color == EStoneColor::Black) other_color = EStoneColor::White;
	else if (target_color == EStoneColor::White) other_color = EStoneColor::Black;

	TArray<bool> StoneToChange;
	StoneToChange.Empty();
	StoneToChange.SetNum(Size*Size);

	// West
	for (int x = stone_x - 1; x >= 0; x--)
	{
		AOthelloBlock* Target = GetBlock(x, stone_y);

		if (Target)
		{
			// same stone color
			if (Target->GetStoneColor() == target_color)
			{
				break;
			}
			// other stone color
			else if (Target->GetStoneColor() == other_color)
			{
				if (x > 0) StoneToChange[stone_y*Size + x] = true;
				else
				{
					for (int i = 0; i < stone_x; i++) StoneToChange[stone_y*Size + i] = false;
					break;
				}
			}
			// no stone
			else if (Target->GetStoneColor() == EStoneColor::None)
			{
				for (int i = x; i < stone_x; i++) StoneToChange[stone_y*Size + i] = false;
				break;
			}
		}
	}

	// East
	for (int x = stone_x + 1; x < Size; x++)
	{
		AOthelloBlock* Target = GetBlock(x, stone_y);

		if (Target)
		{
			// same stone color
			if (Target->GetStoneColor() == target_color)
			{
				break;
			}
			// other stone color
			else if (Target->GetStoneColor() == other_color)
			{
				if (x < Size - 1) StoneToChange[stone_y*Size + x] = true;
				else
				{
					for (int i = Size - 1; i > stone_x; i--) StoneToChange[stone_y*Size + i] = false;
					break;
				}
			}
			// no stone
			else if (Target->GetStoneColor() == EStoneColor::None)
			{
				for (int i = x; i > stone_x; i--) StoneToChange[stone_y*Size + i] = false;
				break;
			}
		}
	}

	// North
	for (int y = stone_y + 1; y < Size; y++)
	{
		AOthelloBlock* Target = GetBlock(stone_x, y);

		if (Target)
		{
			// same stone color
			if (Target->GetStoneColor() == target_color)
			{
				break;
			}
			// other stone color
			else if (Target->GetStoneColor() == other_color)
			{
				if (y < Size - 1) StoneToChange[y*Size + stone_x] = true;
				else
				{
					for (int j = Size - 1; j > stone_y; j--) StoneToChange[j*Size + stone_x] = false;
					break;
				}
			}
			// no stone
			else if (Target->GetStoneColor() == EStoneColor::None)
			{
				for (int j = y; j > stone_y; j--) StoneToChange[j*Size + stone_x] = false;
				break;
			}
		}
	}

	// South
	for (int y = stone_y - 1; y >= 0; y--)
	{
		AOthelloBlock* Target = GetBlock(stone_x, y);

		if (Target)
		{
			// same stone color
			if (Target->GetStoneColor() == target_color)
			{
				break;
			}
			// other stone color
			else if (Target->GetStoneColor() == other_color)
			{
				if (y > 0) StoneToChange[y*Size + stone_x] = true;
				else
				{
					for (int j = 0; j < stone_y; j++) StoneToChange[j*Size + stone_x] = false;
					break;
				}
			}
			// no stone
			else if (Target->GetStoneColor() == EStoneColor::None)
			{
				for (int j = y; j < stone_y; j++) StoneToChange[j*Size + stone_x] = false;
				break;
			}
		}
	}

	// Northern-West
	for (int x = stone_x - 1, y = stone_y + 1; x >= 0 && y < Size; x--, y++)
	{
		AOthelloBlock* Target = GetBlock(x, y);

		if (Target)
		{
			// same stone color
			if (Target->GetStoneColor() == target_color)
			{
				break;
			}
			// other stone color
			else if (Target->GetStoneColor() == other_color)
			{
				if (y < Size - 1 && x > 0) StoneToChange[y*Size + x] = true;
				else
				{
					for (int i = stone_x - 1, j = stone_y + 1; i > 0 && j < Size - 1; i--, j++)
						StoneToChange[j*Size + i] = false;
					break;
				}
			}
			// no stone
			else if (Target->GetStoneColor() == EStoneColor::None)
			{
				for (int i = stone_x - 1, j = stone_y + 1; i > 0 && j < Size - 1; i--, j++)
					StoneToChange[j*Size + i] = false;
				break;
			}
		}
	}

	// Northern-East
	for (int x = stone_x + 1, y = stone_y + 1; x < Size && y < Size; x++, y++)
	{
		AOthelloBlock* Target = GetBlock(x, y);

		if (Target)
		{
			// same stone color
			if (Target->GetStoneColor() == target_color)
			{
				break;
			}
			// other stone color
			else if (Target->GetStoneColor() == other_color)
			{
				if (y < Size - 1 && x < Size - 1) StoneToChange[y*Size + x] = true;
				else
				{
					for (int i = stone_x + 1, j = stone_y + 1; i < Size - 1 && j < Size - 1; i++, j++)
						StoneToChange[j*Size + i] = false;
					break;
				}
			}
			// no stone
			else if (Target->GetStoneColor() == EStoneColor::None)
			{
				for (int i = stone_x + 1, j = stone_y + 1; i < Size - 1 && j < Size - 1; i++, j++)
					StoneToChange[j*Size + i] = false;
				break;
			}
		}
	}

	// Southern-West
	for (int x = stone_x - 1, y = stone_y - 1; x >= 0 && y >= 0; x--, y--)
	{
		AOthelloBlock* Target = GetBlock(x, y);

		if (Target)
		{
			// same stone color
			if (Target->GetStoneColor() == target_color)
			{
				break;
			}
			// other stone color
			else if (Target->GetStoneColor() == other_color)
			{
				if (y > 0 && x > 0) StoneToChange[y*Size + x] = true;
				else
				{
					for (int i = stone_x - 1, j = stone_y - 1; i > 0 && j > 0; i--, j--)
						StoneToChange[j*Size + i] = false;
					break;
				}
			}
			// no stone
			else if (Target->GetStoneColor() == EStoneColor::None)
			{
				for (int i = stone_x - 1, j = stone_y - 1; i > 0 && j > 0; i--, j--)
					StoneToChange[j*Size + i] = false;
				break;
			}
		}
	}

	// Southern-East
	for (int x = stone_x + 1, y = stone_y - 1; x < Size && y >= 0; x++, y--)
	{
		AOthelloBlock* Target = GetBlock(x, y);

		if (Target)
		{
			// same stone color
			if (Target->GetStoneColor() == target_color)
			{
				break;
			}
			// other stone color
			else if (Target->GetStoneColor() == other_color)
			{
				if (y > 0 && x < Size - 1) StoneToChange[y*Size + x] = true;
				else
				{
					for (int i = stone_x + 1, j = stone_y - 1; i < Size - 1 && j > 0; i++, j--)
						StoneToChange[j*Size + i] = false;
					break;
				}
			}
			// no stone
			else if (Target->GetStoneColor() == EStoneColor::None)
			{
				for (int i = stone_x + 1, j = stone_y - 1; i < Size - 1 && j > 0; i++, j--)
					StoneToChange[j*Size + i] = false;
				break;
			}
		}
	}

	int ChangedStoneNumber = 0;
	for (int y = 0; y < Size; y++)
		for (int x = 0; x < Size; x++)
			if (StoneToChange[y*Size + x])
			{
				GetBlock(x, y)->ChangeStone();
				ChangedStoneNumber++;
			}

	return ChangedStoneNumber;
}

bool AOthelloBlockGrid::CheckGameOver()
{
	AOthelloGameMode* GameMode = Cast<AOthelloGameMode>(GetWorld()->GetAuthGameMode());

	// Check whether colors of all stone are same each other
	if (GameMode->GetTurnCount() <= Size * Size - 4)
	{
		uint16 WhiteStoneCount = 0;
		uint16 BlackStoneCount = 0;

		for (int y = 0; y < Size; y++)
		{
			for (int x = 0; x < Size; x++)
			{
				AOthelloBlock* Target = GetBlock(x, y);

				if (Target->GetStoneColor() == EStoneColor::White) WhiteStoneCount++;
				else if (Target->GetStoneColor() == EStoneColor::Black) BlackStoneCount++;
			}
		}
		if ((WhiteStoneCount == 0 && BlackStoneCount > 0) || (WhiteStoneCount > 0 && BlackStoneCount == 0))
		{
			return true;
		}
	}

	// Check whether there is no location stones can be put on
	for (int y = 0, count = 0; y < Size; y++)
	{
		for (int x = 0; x < Size; x++)
		{
			bool IsPossible = CheckPossibility(x, y);

			// found location stone can be put on
			if (IsPossible)
			{
				return false;
			}
			// find more possible location
			else if (!IsPossible && count < Size * Size - 1)
			{
				count++;
			}
			// there is no location stone can be put on
			else
			{
				GameMode->OmitTurn();
				if (GameMode->GetTurnOmit()) return true;
			}
		}
	}

	// Check whether board is full of stones
	UpdatePutablePosition();
	for (int i = 0; i < Size * Size; i++)
		if (IsPutable[i])
			return false;

	return true;
}

EStoneColor AOthelloBlockGrid::WhatColorIsMore()
{
	int WhiteStoneNumber = 0;
	int BlackStoneNumber = 0;
	for (int i = 0; i < Size * Size; i++)
	{
		if (GetBlockS(i)->GetStoneColor() == EStoneColor::White)
			WhiteStoneNumber++;
		else if (GetBlockS(i)->GetStoneColor() == EStoneColor::Black)
			BlackStoneNumber++;
	}

	if (WhiteStoneNumber > BlackStoneNumber)
		return EStoneColor::White;
	else if (WhiteStoneNumber < BlackStoneNumber)
		return EStoneColor::Black;
	else
		return EStoneColor::None;
}

AOthelloBlock* AOthelloBlockGrid::GetBlockS(int index)
{
	return BlockArray[index];
}

void AOthelloBlockGrid::ResetGrid()
{
	AOthelloGameMode* GameMode = Cast<AOthelloGameMode>(GetWorld()->GetAuthGameMode());
	GameMode->Reset();

	for (int i = 0; i < Size * Size; i++)
	{
		AOthelloBlock* Block = GetBlockS(i);

		Block->ClearStone();
		if (i == 27 || i == 36) Block->PutStoneF(EStoneColor::Black);
		else if (i == 28 || i == 35) Block->PutStoneF(EStoneColor::White);
	}

	UpdatePutablePosition();
}

AOthelloBlock* AOthelloBlockGrid::GetBlock(int x, int y)
{
	if (x < 0 || y < 0) return nullptr;

	const int target = y * Size + x;
	if (target < BlockArray.Num() && target >= 0)
		return BlockArray[target];

	return nullptr;
}

int AOthelloBlockGrid::GetRoundNumber()
{
	AOthelloGameMode* GameMode = Cast<AOthelloGameMode>(GetWorld()->GetAuthGameMode());

	return (int)GameMode->GetTurnCount() / 2;
}

TArray<int> AOthelloBlockGrid::GetState()
{
	TArray<int> StateArray;

	for(int y = 0; y < Size; y++)
		for (int x = 0; x < Size; x++)
		{
			StateArray.Add((int)GetBlock(x, y)->GetStoneColor());
		}

	return StateArray;
}

bool AOthelloBlockGrid::IsGameOver()
{
	AOthelloGameMode* GameMode = Cast<AOthelloGameMode>(GetWorld()->GetAuthGameMode());

	return GameMode->IsGameOver();
}

void AOthelloBlockGrid::UpdatePutablePosition()
{
	for (int i = 0; i < Size * Size; i++)
		IsPutable[i] = CheckPossibility(i % Size, i / Size);
}
#undef LOCTEXT_NAMESPACE
