// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "OthelloBlock.h"
#include "OthelloBlockGrid.h"
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
		FConstructorStatics()
			: PlaneMesh(TEXT("/Game/Puzzle/Meshes/PuzzleCube.PuzzleCube"))
			, CylinderMesh(TEXT("/Game/Puzzle/Meshes/CylinderMesh.CylinderMesh"))
			, BaseMaterial(TEXT("/Game/Puzzle/Meshes/BaseMaterial.BaseMaterial"))
			, BlueMaterial(TEXT("/Game/Puzzle/Meshes/BlueMaterial.BlueMaterial"))
			, OrangeMaterial(TEXT("/Game/Puzzle/Meshes/OrangeMaterial.OrangeMaterial"))
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
	// Check we are not already active
	if (!bIsActive)
	{
		bIsActive = true;

		// Change material
		BlockMesh->SetMaterial(0, OrangeMaterial);

		// Tell the Grid
		if (OwningGrid != nullptr)
		{
			OwningGrid->AddScore();
		}
	}
	
	if (Hello) ClearStone();
	else PutStone();
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
		BlockMesh->SetMaterial(0, BaseMaterial);
	}
	else
	{
		BlockMesh->SetMaterial(0, BlueMaterial);
	}
}

void AOthelloBlock::PutStone()
{
	StoneMesh->SetVisibility(true);
	Hello = true;
}

void AOthelloBlock::ClearStone()
{
	StoneMesh->SetVisibility(false);
	Hello = false;
}