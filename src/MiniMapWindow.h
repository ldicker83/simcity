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

#include "Point.h"
#include "Texture.h"
#include "Vector.h"

#include <cstdint>

#include <SDL2/SDL.h>


struct SDL_Window;
struct SDL_Renderer;


class MiniMapWindow
{
public:
	const int MiniTileSize{ 3 };
	const int TileSize{ 16 };

public:
	MiniMapWindow() = delete;
	MiniMapWindow(const Point<int>& position, const Vector<int>& size, const Vector<int>& tileSize);

	~MiniMapWindow();

	uint32_t id() const;

	void updateMapViewPosition(const Point<int>& position);
	void updateViewportSize(const Vector<int>& viewportSize);
	void updateTilePointedAt(const Point<int>& tilePointedAt);

	void hide();
	void show();

	void draw();
	void drawUI();

private:
	SDL_Window* mWindow{ nullptr };
	SDL_Renderer* mRenderer{ nullptr };

	Texture mTiles{};
	Texture mTexture{};

	SDL_Rect mSelector{};
	SDL_Rect mTileHighlight{ 0, 0, 3, 3 };
	SDL_Rect mTileRect{ 0, 0, 3, 3 };

	uint32_t mWindowID{};

	Vector<int> mMapSize{};
};