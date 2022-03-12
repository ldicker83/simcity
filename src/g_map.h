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

#include "Texture.h"


namespace Colors
{
	constexpr SDL_Color Clear{ 0, 0, 0, 0 };
	constexpr SDL_Color DarkBlue{ 0, 0, 139, 255 };
	constexpr SDL_Color DarkGreen{ 0, 100, 0, 255 };
	constexpr SDL_Color Gold{ 255, 215, 0, 255 };
	constexpr SDL_Color Green{ 0, 128, 0, 255 };
	constexpr SDL_Color LightBlue{ 0, 191, 255, 255 };
	constexpr SDL_Color LightGreen{ 50, 205, 50, 255 };
	constexpr SDL_Color LightGrey{ 211, 211, 211, 255 };
	constexpr SDL_Color MediumBlue{ 0, 0, 205, 255 };
	constexpr SDL_Color Olive{ 107, 142, 35 };
	constexpr SDL_Color Orange{ 255, 165, 0, 255 };
	constexpr SDL_Color Red{ 255, 0, 0, 255 };
	constexpr SDL_Color Turquoise{ 64, 224, 208, 255 };
	constexpr SDL_Color Yellow{ 255, 255, 0, 255 };
};


void initTexture(Texture& texture, const int width, const int height);
void initOverlayTexture();

void turnOffBlending(const Texture& texture);
void turnOnBlending(const Texture& texture);

void drawPopDensity();
void drawCrimeMap();
void drawTrafficMap();
void drawPollutionMap();
void drawLandMap();
void drawPoliceRadius();
void drawFireRadius();
void drawRateOfGrowth();

const Texture& crimeOverlayTexture();
const Texture& populationDensityTexture();
const Texture& trafficDensityTexture();
const Texture& pollutionTexture();
const Texture& landValueTexture();
const Texture& policeRadiusTexture();
const Texture& fireRadiusTexture();
const Texture& rateOfGrowthTexture();
