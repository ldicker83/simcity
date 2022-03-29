// This file is part of Micropolis-SDL2PP
// Micropolis-SDL2PP is based on Micropolis
//
// Copyright � 2022 Leeor Dicker
//
// Portions Copyright � 1989-2007 Electronic Arts Inc.
//
// Micropolis-SDL2PP is free software; you can redistribute it and/or modify
// it under the terms of the GNU GPLv3, with additional terms. See the README
// file, included in this distribution, for details.
#pragma once

#include "Tool.h"

class Budget;

enum class ToolOperation
{
	Road,
	Rail,
	Wire
};

ToolResult ConnectTile(int x, int y, int* TileAdrPtr, int Command, Budget&);
ToolResult CanConnectTile(int x, int y, Tool, Budget&);