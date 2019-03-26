// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "OthelloPlayerController.h"

AOthelloPlayerController::AOthelloPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableTouchEvents = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}
