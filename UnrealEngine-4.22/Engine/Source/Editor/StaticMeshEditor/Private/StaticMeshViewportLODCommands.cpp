﻿// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "StaticMeshViewportLODCommands.h"

#define LOCTEXT_NAMESPACE "StaticMeshViewportLODCommands"

void FStaticMeshViewportLODCommands::RegisterCommands()
{
	UI_COMMAND( LODAuto, "LOD Auto", "Automatically select LOD", EUserInterfaceActionType::RadioButton, FInputChord() );
	UI_COMMAND( LOD0, "LOD 0", "Force select LOD 0", EUserInterfaceActionType::RadioButton, FInputChord() );
}

#undef LOCTEXT_NAMESPACE