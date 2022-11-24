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
#include "MiniMapWindow.h"

#include "Map.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <stdexcept>
#include <iostream>


/**
 * Constructs a minimap window
 * 
 * \param position  Position to open the window at
 * \param size      Width/Height of the map in tiles
 * \param tileSize  Size of large and mini tiles in pixels. X == Full Size, Y == Mini.
 */
MiniMapWindow::MiniMapWindow(const Point<int>& position, const Vector<int>& size, const Vector<int>& tileSize):
    MiniTileSize{ tileSize.y },
    TileSize{ tileSize.x },
    mMapSize{ size }
{
    if (!SDL_WasInit(SDL_INIT_VIDEO))
    {
        std::cout << "MiniMapWindow::c'tor: SDL Video subsystem was not initialized. Initializing now." << std::endl;
        SDL_Init(SDL_INIT_VIDEO);
    }

    mWindow = SDL_CreateWindow("Mini Map",
        position.x, position.y,
        size.x * MiniTileSize, size.y * MiniTileSize,
        SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_SKIP_TASKBAR | SDL_WINDOW_UTILITY);

    if (!mWindow)
    {
        throw std::runtime_error("MiniMapWindow::MiniMapWindow(): Unable to create primary window: " + std::string(SDL_GetError()));
    }

    mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED);
    if (!mRenderer)
    {
        throw std::runtime_error("MiniMapWindow::MiniMapWindow(): Unable to create renderer: " + std::string(SDL_GetError()));
    }

    mWindowID = SDL_GetWindowID(mWindow);

    mTiles = loadTexture(mRenderer, "images/tilessm.xpm");
    mTexture.texture = SDL_CreateTexture(mRenderer, SDL_PIXELFORMAT_ARGB32, SDL_TEXTUREACCESS_TARGET, size.x * MiniTileSize, size.y * MiniTileSize);
    //SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 150);
}


MiniMapWindow::~MiniMapWindow()
{
    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
}


Uint32 MiniMapWindow::id() const
{
    return mWindowID;
}


void MiniMapWindow::updateMapViewPosition(const Point<int>& position)
{
    
    mSelector.x = (position.x / TileSize) * MiniTileSize;
    mSelector.y = (position.y / TileSize) * MiniTileSize;
}


void MiniMapWindow::updateViewportSize(const Vector<int>& viewportSize)
{
    mSelector.w = static_cast<int>(std::ceil(viewportSize.x / static_cast<float>(TileSize)) * MiniTileSize);
    mSelector.h = static_cast<int>(std::ceil(viewportSize.y / static_cast<float>(TileSize)) * MiniTileSize);
}


void MiniMapWindow::updateTilePointedAt(const Point<int>& tilePointedAt)
{
    mTileHighlight.x = tilePointedAt.x * MiniTileSize;
    mTileHighlight.y = tilePointedAt.y * MiniTileSize;
}


void MiniMapWindow::hide()
{
    SDL_HideWindow(mWindow);
}


void MiniMapWindow::show()
{
    SDL_ShowWindow(mWindow);
}


void MiniMapWindow::draw()
{
    SDL_Rect miniMapDrawRect{ 0, 0, MiniTileSize, MiniTileSize };

    SDL_SetRenderTarget(mRenderer, mTexture.texture);
    for (int row = 0; row < mMapSize.x; row++)
    {
        for (int col = 0; col < mMapSize.y; col++)
        {
            miniMapDrawRect = { row * MiniTileSize, col * MiniTileSize, miniMapDrawRect.w, miniMapDrawRect.h };
            mTileRect.y = maskedTileValue(tileValue(row, col)) * MiniTileSize;
            SDL_RenderCopy(mRenderer, mTiles.texture, &mTileRect, &miniMapDrawRect);
        }
    }

    SDL_RenderPresent(mRenderer);
    SDL_SetRenderTarget(mRenderer, nullptr);
}


void MiniMapWindow::drawUI()
{
    SDL_RenderCopy(mRenderer, mTexture.texture, nullptr, nullptr);

    //SDL_RenderCopy(MainWindowRenderer, powerMapTexture().texture, nullptr, &MiniMapDestination);

    // \todo Make this only draw when an overlay flag is set
    //SDL_RenderCopy(MainWindowRenderer, crimeOverlayTexture().texture, nullptr, &MiniMapDestination);
    //SDL_RenderCopy(MainWindowRenderer, populationDensityTexture().texture, nullptr, &MiniMapDestination);
    //SDL_RenderCopy(MainWindowRenderer, pollutionTexture().texture, nullptr, &MiniMapDestination);
    //SDL_RenderCopy(MainWindowRenderer, landValueTexture().texture, nullptr, &MiniMapDestination);
    //SDL_RenderCopy(MainWindowRenderer, policeRadiusTexture().texture, nullptr, &MiniMapDestination);
    //SDL_RenderCopy(MainWindowRenderer, fireRadiusTexture().texture, nullptr, &MiniMapDestination);
    //SDL_RenderCopy(MainWindowRenderer, rateOfGrowthTexture().texture, nullptr, &MiniMapDestination);

    // traffic map
    //SDL_RenderCopy(MainWindowRenderer, transitMapTexture().texture, nullptr, &MiniMapDestination);
    //SDL_RenderCopy(MainWindowRenderer, trafficDensityTexture().texture, nullptr, &MiniMapDestination);

    SDL_RenderDrawRect(mRenderer, &mSelector);
    SDL_RenderDrawRect(mRenderer, &mTileHighlight);

    SDL_RenderPresent(mRenderer);
}
