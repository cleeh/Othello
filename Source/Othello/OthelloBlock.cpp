// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "OthelloBlock.h"
#include "OthelloBlockGrid.h"
#include "OthelloGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstance.h"

AOthelloBlock::AOthelloBlock()
{
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh;
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> CylinderMesh;
		ConstructorHelpers::FObjectFinderOptional<UMaterial> BaseMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> BlueMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> OrangeMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> WhiteMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> BlackMaterial;
		FConstructorStatics()
			: PlaneMesh(TEXT("/Game/Puzzle/Meshes/PuzzleCube.PuzzleCube"))
			, CylinderMesh(TEXT("/Game/Puzzle/Meshes/CylinderMesh.CylinderMesh"))
			, BaseMaterial(TEXT("/Game/Puzzle/Meshes/BaseMaterial.BaseMaterial"))
			, BlueMaterial(TEXT("/Game/Puzzle/Meshes/BlueMaterial.BlueMaterial"))
			, OrangeMaterial(TEXT("/Game/Puzzle/Meshes/OrangeMaterial.OrangeMaterial"))
			, WhiteMaterial(TEXT("/Game/Puzzle/Meshes/WhiteMaterial.WhiteMaterial"))
			, BlackMaterial(TEXT("/Game/Puzzle/Meshes/BlackMaterial.BlackMaterial"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	// Create dummy root scene component
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	RootComponent = DummyRoot;

	// Create static mesh componentfor block
	BlockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
	BlockMesh->SetStaticMesh(ConstructorStatics.PlaneMesh.Get());
	BlockMesh->SetRelativeScale3D(FVector(1.f,1.f,0.25f));
	BlockMesh->SetRelativeLocation(FVector(0.f,0.f,25.f));
	BlockMesh->SetMaterial(0, ConstructorStatics.BlueMaterial.Get());
	BlockMesh->SetupAttachment(DummyRoot);
	BlockMesh->OnClicked.AddDynamic(this, &AOthelloBlock::BlockClicked);
	BlockMesh->OnInputTouchBegin.AddDynamic(this, &AOthelloBlock::OnFingerPressedBlock);

	// Create static mesh component for stone
	StoneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StoneMesh"));
	StoneMesh->SetStaticMesh(ConstructorStatics.CylinderMesh.Get());
	StoneMesh->SetRelativeScale3D(FVector(2.f, 2.f, 0.25f));
	StoneMesh->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
	StoneMesh->SetMaterial(0, ConstructorStatics.OrangeMaterial.Get());
	StoneMesh->SetupAttachment(DummyRoot);
	StoneMesh->OnClicked.AddDynamic(this, &AOthelloBlock::BlockClicked);
	StoneMesh->OnInputTouchBegin.AddDynamic(this, &AOthelloBlock::OnFingerPressedBlock);

	// Save a pointer to the orange material
	BaseMaterial = ConstructorStatics.BaseMaterial.Get();
	BlueMaterial = ConstructorStatics.BlueMaterial.Get();
	OrangeMaterial = ConstructorStatics.OrangeMaterial.Get();
	WhiteMaterial = ConstructorStatics.WhiteMaterial.Get();
	BlackMaterial = ConstructorStatics.BlackMaterial.Get();

	X = -1;
	Y = -1;

	ClearStone();
}

void AOthelloBlock::BlockClicked(UPrimitiveComponent* ClickedComp, FKey ButtonClicked)
{
	HandleClicked();
}


void AOthelloBlock::OnFingerPressedBlock(ETouchIndex::Type FingerIndex, UPrimitiveComponent* TouchedComponent)
{
	HandleClicked();
}

void AOthelloBlock::HandleClicked()
{
	if (WhatStoneColor != EStoneColor::None) ClearStone();
	else
	{
		PutStone();
		OwningGrid->ChangeStonesColor(X, Y);
	}
}

void AOthelloBlock::Highlight(bool bOn)
{
	// Do not highlight if the block has already been activated.
	if (bIsActive)
	{
		return;
	}

	if (bOn)
	{
		BlockMesh->SetMaterial(0, OrangeMaterial);
	}
	else
	{
		BlockMesh->SetMaterial(0, BlueMaterial);
	}
}

void AOthelloBlock::PutStone()
{
	AOthelloGameMode* GameMode = Cast<AOthelloGameMode>(GetWorld()->GetAuthGameMode());

	switch (GameMode->GameTurn)
	{
	case ETurn::Black:
		StoneMesh->SetMaterial(0, BlackMaterial);
		GameMode->GameTurn = ETurn::White;
		WhatStoneColor = EStoneColor::Black;
		break;
	case ETurn::White:
		StoneMesh->SetMaterial(0, WhiteMaterial);
		GameMode->GameTurn = ETurn::Black;
		WhatStoneColor = EStoneColor::White;
		break;
	}
	StoneMesh->SetVisibility(true);
}

void AOthelloBlock::PutStone(EStoneColor stone)
{
	switch (stone)
	{
	case EStoneColor::Black:
		StoneMesh->SetMaterial(0, BlackMaterial);
		WhatStoneColor = EStoneColor::Black;
		break;
	case EStoneColor::White:
		StoneMesh->SetMaterial(0, WhiteMaterial);
		WhatStoneColor = EStoneColor::White;
		break;
	}
	StoneMesh->SetVisibility(true);
}

void AOthelloBlock::ClearStone()
{
	WhatStoneColor = EStoneColor::None;
	StoneMesh->SetVisibility(false);
}

void AOthelloBlock::ChangeStone()
{
	if (WhatStoneColor == EStoneColor::White) PutStone(EStoneColor::Black);
	else if (WhatStoneColor == EStoneColor::Black) PutStone(EStoneColor::White);
}