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
#include "main.h"

#include "g_map.h"

#include "Map.h"

#include "s_alloc.h"

#include "Texture.h"

int DynamicData[32];


namespace
{
    Texture TransitMapTexture;
};


const Texture& transitMapTexture()
{
    return TransitMapTexture;
}


void initMapTextures()
{
    initTexture(TransitMapTexture, SimWidth * 3, SimHeight * 3);
}


void drawRes()
{
    /*
    int lineBytes = view->line_bytes;
    int pixelBytes = view->pixel_bytes;

    int* mp = &Map[0][0];
    unsigned char* imageBase = view->data;

    for (int col = 0; col < SimWidth; col++)
    {
        unsigned char* image = imageBase + (3 * pixelBytes * col);
        for (int row = 0; row < SimHeight; row++)
        {

            unsigned int tile = *(mp++) & LOMASK;
            if (tile >= TILE_COUNT)
            {
                tile -= TILE_COUNT;
            }

            ////////
            if (tile > 422)
            {
                tile = 0;
            }
            ////////

            unsigned int* mem = (unsigned int*)&view->smalltiles[tile * 4 * 4 * pixelBytes];
            for (int i = 0; i < 3; ++i)
            {
                unsigned int l = mem[i];
                image[0] = l >> 24;
                image[1] = l >> 16;
                image[2] = l >> 8;
                image += lineBytes;
            }
        }
    }
    */
}


void drawCom()
{
    /*
    int lineBytes = view->line_bytes;
    int pixelBytes = view->pixel_bytes;

    int* mp = &Map[0][0];
    unsigned char* imageBase = view->data;

    for (int col = 0; col < SimWidth; col++)
    {
        unsigned char* image = imageBase + (3 * pixelBytes * col);
        for (int row = 0; row < SimHeight; row++)
        {

            unsigned int tile = *(mp++) & LOMASK;
            if (tile >= TILE_COUNT)
            {
                tile -= TILE_COUNT;
            }

            ////////
            if ((tile > 609) || ((tile >= 232) && (tile < 423)))
            {
                tile = 0;
            }
            ////////

            unsigned int* mem = (unsigned int*)&view->smalltiles[tile * 4 * 4 * pixelBytes];
            for (int i = 0; i < 3; ++i)
            {
                unsigned int l = mem[i];
                image[0] = l >> 24;
                image[1] = l >> 16;
                image[2] = l >> 8;
                image += lineBytes;
            }
        }
    }
    */
}


void drawInd()
{
    /*
    int lineBytes = view->line_bytes;
    int pixelBytes = view->pixel_bytes;

    int* mp = &Map[0][0];
    unsigned char* imageBase = view->data;

    for (int col = 0; col < SimWidth; col++)
    {
        unsigned char* image = imageBase + (3 * pixelBytes * col);
        for (int row = 0; row < SimHeight; row++)
        {

            unsigned int tile = *(mp++) & LOMASK;
            if (tile >= TILE_COUNT)
            {
                tile -= TILE_COUNT;
            }

            ////////
            if (((tile >= 240) && (tile <= 611)) ||
                ((tile >= 693) && (tile <= 851)) ||
                ((tile >= 860) && (tile <= 883)) ||
                (tile >= 932))
            {
                tile = 0;
            }
            ////////

            unsigned int* mem = (unsigned int*)&view->smalltiles[tile * 4 * 4 * pixelBytes];
            for (int i = 0; i < 3; ++i)
            {
                unsigned int l = mem[i];
                image[0] = l >> 24;
                image[1] = l >> 16;
                image[2] = l >> 8;
                image += lineBytes;
            }
        }
    }
    */
}


void drawLilTransMap()
{
    SDL_Rect miniMapDrawRect{ 0, 0, 3, 3 };

    SDL_SetRenderTarget(MainWindowRenderer, TransitMapTexture.texture);
    for (int row = 0; row < SimWidth; row++)
    {
        for (int col = 0; col < SimHeight; col++)
        {
            miniMapDrawRect = { row * 3, col * 3, miniMapDrawRect.w, miniMapDrawRect.h };

            unsigned int tile = maskedTileValue(row, col);

            if ((tile >= RESBASE) ||
                ((tile >= BRWXXX7) && tile <= 220) ||
                (tile == UNUSED_TRASH6))
            {
                tile = 0;
            }


            miniMapTileRect().y = tile * 3;
            SDL_RenderCopy(MainWindowRenderer, SmallTileset.texture, &miniMapTileRect(), &miniMapDrawRect);
        }
    }
    SDL_RenderPresent(MainWindowRenderer);

    SDL_SetRenderTarget(MainWindowRenderer, nullptr);
}


/* color pixel values */
#define UNPOWERED	COLOR_LIGHTBLUE
#define POWERED		COLOR_RED
#define CONDUCTIVE	COLOR_LIGHTGRAY


void drawPower()
{
    /*
    int lineBytes = view->line_bytes;
    int pixelBytes = view->pixel_bytes;

    int pix = 0;

    int powered = view->pixels[POWERED];
    int unpowered = view->pixels[UNPOWERED];
    int conductive = view->pixels[CONDUCTIVE];

    int* mp = &Map[0][0];
    unsigned char* imageBase = view->data;

    for (int col = 0; col < SimWidth; col++)
    {
        unsigned char* image = imageBase + (3 * pixelBytes * col);
        for (int row = 0; row < SimHeight; row++)
        {
            unsigned int tile = *(mp++);

            if ((tile & LOMASK) >= TILE_COUNT)
            {
                tile -= TILE_COUNT;
            }

            if ((unsigned int)(tile & LOMASK) <= (unsigned int)63)
            {
                tile &= LOMASK;
                pix = -1;
            }
            else if (tile & ZONEBIT)
            {
                pix = (tile & PWRBIT) ? powered : unpowered;
            }
            else
            {
                if (tile & CONDBIT)
                {
                    pix = conductive;
                }
                else
                {
                    tile = 0;
                    pix = -1;
                }
            }

            if (pix < 0)
            {
                unsigned int* mem = (unsigned int*)&view->smalltiles[tile * 4 * 4 * pixelBytes];
                for (int i = 0; i < 3; ++i)
                {
                    unsigned int l = mem[i];
                    image[0] = l >> 24;
                    image[1] = l >> 16;
                    image[2] = l >> 8;
                    image += lineBytes;
                }
            }
            else
            {
                for (int y = 0; y < 3; y++)
                {
                    unsigned char* img = image;
                    for (int x = 0; x < 4; x++)
                    {
                        *(img++) = (pix >> 0) & 0xff;
                        *(img++) = (pix >> 8) & 0xff;
                        *(img++) = (pix >> 16) & 0xff;
                        if (pixelBytes == 4)
                        {
                            img++;
                        }
                    }
                    image += lineBytes;
                }
            }
        }
    }
    */
}


int dynamicFilter(int col, int row)
{
    int x;

    int r = row >> 1;
    int c = col >> 1;

    if (((DynamicData[0] > DynamicData[1]) ||
        ((x = PopDensity[c][r]) >= DynamicData[0]) &&
        (x <= DynamicData[1])) &&
        ((DynamicData[2] > DynamicData[3]) ||
            ((x = RateOGMem[c >> 2][r >> 2]) >= ((2 * DynamicData[2]) - 256)) &&
            (x <= ((2 * DynamicData[3]) - 256))) &&
        ((DynamicData[4] > DynamicData[5]) ||
            ((x = TrfDensity[c][r]) >= DynamicData[4]) &&
            (x <= DynamicData[5])) &&
        ((DynamicData[6] > DynamicData[7]) ||
            ((x = PollutionMem[c][r]) >= DynamicData[6]) &&
            (x <= DynamicData[7])) &&
        ((DynamicData[8] > DynamicData[9]) ||
            ((x = CrimeMem[c][r]) >= DynamicData[8]) &&
            (x <= DynamicData[9])) &&
        ((DynamicData[10] > DynamicData[11]) ||
            ((x = LandValueMem[c][r]) >= DynamicData[10]) &&
            (x <= DynamicData[11])) &&
        ((DynamicData[12] > DynamicData[13]) ||
            ((x = PoliceMapEffect[c >> 2][r >> 2]) >= DynamicData[12]) &&
            (x <= DynamicData[13])) &&
        ((DynamicData[14] > DynamicData[15]) ||
            ((x = FireRate[c >> 2][r >> 2]) >= DynamicData[14]) &&
            (x <= DynamicData[15]))) {
        return 1;
    }
    else
    {
        return 0;
    }
}


void drawDynamic()
{
    /*
    int lineBytes = view->line_bytes;
    int pixelBytes = view->pixel_bytes;

    int* mp = &Map[0][0];
    unsigned char* imageBase = view->data;

    for (int col = 0; col < SimWidth; col++)
    {
        unsigned char* image = imageBase + (3 * pixelBytes * col);
        for (int row = 0; row < SimHeight; row++)
        {

            unsigned int tile = *(mp++) & LOMASK;
            if (tile >= TILE_COUNT)
            {
                tile -= TILE_COUNT;
            }

            ////////
            if (tile > 63)
            {
                if (!dynamicFilter(col, row))
                {
                    tile = 0;
                }
            }
            ////////

            unsigned int* mem = (unsigned int*)&view->smalltiles[tile * 4 * 4 * pixelBytes];
            for (int i = 0; i < 3; ++i)
            {
                unsigned int l = mem[i];
                image[0] = l >> 24;
                image[1] = l >> 16;
                image[2] = l >> 8;
                image += lineBytes;
            }
        }
    }
    */
}