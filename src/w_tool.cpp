/* w_tool.c
 *
 * Micropolis, Unix Version.  This game was released for the Unix platform
 * in or about 1990 and has been modified for inclusion in the One Laptop
 * Per Child program.  Copyright (C) 1989 - 2007 Electronic Arts Inc.  If
 * you need assistance with this program, you may contact:
 *   http://wiki.laptop.org/go/Micropolis  or email  micropolis@laptop.org.
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.  You should have received a
 * copy of the GNU General Public License along with this program.  If
 * not, see <http://www.gnu.org/licenses/>.
 * 
 *             ADDITIONAL TERMS per GNU GPL Section 7
 * 
 * No trademark or publicity rights are granted.  This license does NOT
 * give you any right, title or interest in the trademark SimCity or any
 * other Electronic Arts trademark.  You may not distribute any
 * modification of this program using the trademark SimCity or claim any
 * affliation or association with Electronic Arts Inc. or its employees.
 * 
 * Any propagation or conveyance of this program must include this
 * copyright notice and these terms.
 * 
 * If you convey this program (or any modifications of it) and assume
 * contractual liability for the program to recipients of it, you agree
 * to indemnify Electronic Arts for any liability that those contractual
 * assumptions impose on Electronic Arts.
 * 
 * You may not misrepresent the origins of this program; modified
 * versions of the program must be marked as such and not identified as
 * the original program.
 * 
 * This disclaimer supplements the one included in the General Public
 * License.  TO THE FULLEST EXTENT PERMISSIBLE UNDER APPLICABLE LAW, THIS
 * PROGRAM IS PROVIDED TO YOU "AS IS," WITH ALL FAULTS, WITHOUT WARRANTY
 * OF ANY KIND, AND YOUR USE IS AT YOUR SOLE RISK.  THE ENTIRE RISK OF
 * SATISFACTORY QUALITY AND PERFORMANCE RESIDES WITH YOU.  ELECTRONIC ARTS
 * DISCLAIMS ANY AND ALL EXPRESS, IMPLIED OR STATUTORY WARRANTIES,
 * INCLUDING IMPLIED WARRANTIES OF MERCHANTABILITY, SATISFACTORY QUALITY,
 * FITNESS FOR A PARTICULAR PURPOSE, NONINFRINGEMENT OF THIRD PARTY
 * RIGHTS, AND WARRANTIES (IF ANY) ARISING FROM A COURSE OF DEALING,
 * USAGE, OR TRADE PRACTICE.  ELECTRONIC ARTS DOES NOT WARRANT AGAINST
 * INTERFERENCE WITH YOUR ENJOYMENT OF THE PROGRAM; THAT THE PROGRAM WILL
 * MEET YOUR REQUIREMENTS; THAT OPERATION OF THE PROGRAM WILL BE
 * UNINTERRUPTED OR ERROR-FREE, OR THAT THE PROGRAM WILL BE COMPATIBLE
 * WITH THIRD PARTY SOFTWARE OR THAT ANY ERRORS IN THE PROGRAM WILL BE
 * CORRECTED.  NO ORAL OR WRITTEN ADVICE PROVIDED BY ELECTRONIC ARTS OR
 * ANY AUTHORIZED REPRESENTATIVE SHALL CREATE A WARRANTY.  SOME
 * JURISDICTIONS DO NOT ALLOW THE EXCLUSION OF OR LIMITATIONS ON IMPLIED
 * WARRANTIES OR THE LIMITATIONS ON THE APPLICABLE STATUTORY RIGHTS OF A
 * CONSUMER, SO SOME OR ALL OF THE ABOVE EXCLUSIONS AND LIMITATIONS MAY
 * NOT APPLY TO YOU.
 */
//#include "main.h"

#include "s_alloc.h"
#include "s_msg.h"
#include "s_sim.h"

#include "w_con.h"
#include "w_resrc.h"
#include "w_sound.h"
#include "w_stubs.h"
#include "w_tk.h"
#include "w_tool.h"
#include "w_update.h"
#include "w_util.h"
#include "w_x.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <string>
#include <stdexcept>

int specialBase = CHURCH;
int OverRide = 0;
int Expensive = 1000;
int Players = 1;
int Votes = 0;

Tool PendingTool{ Tool::None };
int PendingX{};
int PendingY{};


std::map<Tool, ToolProperties> Tools =
{
    { Tool::Residential, { 100, 3, 1, "Residential" }},
    { Tool::Commercial, { 100, 3, 1, "Commercial" }},
    { Tool::Industrial, { 100, 3, 1, "Industrial" }},
    { Tool::Fire, { 500, 3, 1, "Fire Department" }},
    { Tool::Query, { 0, 0, 0, "Query" }},
    { Tool::Police, { 500, 3, 1, "Police Department" }},
    { Tool::Wire, { 5, 1, 0, "Power Line" }},
    { Tool::Bulldoze, { 1, 1, 0, "Bulldoze" }},
    { Tool::Rail, { 20, 1, 0, "Rail" }},
    { Tool::Road, { 10, 1, 0, "Roads" }},
    { Tool::Stadium, { 5000, 4, 1, "Stadium" }},
    { Tool::Park, { 10, 1, 0, "Park" }},
    { Tool::Seaport, { 3000, 4, 1, "Seaport" }},
    { Tool::Coal, { 3000, 4, 1, "Coal Power" }},
    { Tool::Nuclear, { 5000, 4, 1, "Nuclear Power" }},
    { Tool::Airport, { 10000, 6, 1, "Airport" }},
    { Tool::Network, { 100, 1, 0, "Network" }}
};


void DoSetWandState(Tool tool)
{
    Eval("UISetToolState " + Tools.at(tool).name);
}


void setWandState(Tool state)
{
    sim_update_editors();
    DoSetWandState(state);
}


int putDownPark(int mapH, int mapV)
{
    int tile{};

    if (TotalFunds() - Tools.at(Tool::Park).cost >= 0)
    {
        int value = RandomRange(0, 4);

        if (value == 4)
        {
            tile = FOUNTAIN | BURNBIT | BULLBIT | ANIMBIT;
        }
        else
        {
            tile = (value + WOODS2) | BURNBIT | BULLBIT;
        }

        if (Map[mapH][mapV] == 0)
        {
            Spend(Tools.at(Tool::Park).cost);
            UpdateFunds();
            Map[mapH][mapV] = tile;
            return 1;
        }
        return -1;
    }
    return -2;
}


// Radar?
int putDownNetwork(int mapH, int mapV)
{
    int tile = Map[mapH][mapV] & LOMASK;

    if ((TotalFunds() > 0) && tally(tile))
    {
        Map[mapH][mapV] = tile = 0;
        Spend(1);
    }

    if (tile == 0)
    {
        if ((TotalFunds() - Tools.at(Tool::Network).cost) >= 0)
        {
            Map[mapH][mapV] = TELEBASE | CONDBIT | BURNBIT | BULLBIT | ANIMBIT;
            Spend(Tools.at(Tool::Network).cost);
            return 1;
        }
        else
        {
            return -2;
        }
    }
    else
    {
        return -1;
    }
}


int checkBigZone(int id, int* deltaHPtr, int* deltaVPtr)
{
    switch (id)
    {
    case POWERPLANT: // check coal plant
    case PORT: // check sea port
    case NUCLEAR: // check nuc plant
    case STADIUM: // check stadium
        *deltaHPtr = 0;
        *deltaVPtr = 0;
        return (4);

    case POWERPLANT + 1: // check coal plant
    case COALSMOKE3: // check coal plant, smoke
    case COALSMOKE3 + 1: // check coal plant, smoke
    case COALSMOKE3 + 2: // check coal plant, smoke
    case PORT + 1: // check sea port
    case NUCLEAR + 1: // check nuc plant
    case STADIUM + 1: // check stadium
        *deltaHPtr = -1;
        *deltaVPtr = 0;
        return (4);

    case POWERPLANT + 4: // check coal plant
    case PORT + 4: // check sea port
    case NUCLEAR + 4: // check nuc plant
    case STADIUM + 4: // check stadium
        *deltaHPtr = 0;
        *deltaVPtr = -1;
        return (4);

    case POWERPLANT + 5: // check coal plant
    case PORT + 5: // check sea port
    case NUCLEAR + 5: // check nuc plant
    case STADIUM + 5: // check stadium
        *deltaHPtr = -1;
        *deltaVPtr = -1;
        return (4);

        // check airport
        //*** first row ***
    case AIRPORT:
        *deltaHPtr = 0;
        *deltaVPtr = 0;
        return (6);

    case AIRPORT + 1:
        *deltaHPtr = -1;
        *deltaVPtr = 0;
        return (6);

    case AIRPORT + 2:
        *deltaHPtr = -2;
        *deltaVPtr = 0;
        return (6);

    case AIRPORT + 3:
        *deltaHPtr = -3;
        *deltaVPtr = 0;
        return (6);

        //*** second row ***
    case AIRPORT + 6:
        *deltaHPtr = 0;
        *deltaVPtr = -1;
        return (6);

    case AIRPORT + 7:
        *deltaHPtr = -1;
        *deltaVPtr = -1;
        return (6);

    case AIRPORT + 8:
        *deltaHPtr = -2;
        *deltaVPtr = -1;
        return (6);

    case AIRPORT + 9:
        *deltaHPtr = -3;
        *deltaVPtr = -1;
        return (6);

        //*** third row ***
    case AIRPORT + 12:
        *deltaHPtr = 0;
        *deltaVPtr = -2;
        return (6);

    case AIRPORT + 13:
        *deltaHPtr = -1;
        *deltaVPtr = -2;
        return (6);

    case AIRPORT + 14:
        *deltaHPtr = -2;
        *deltaVPtr = -2;
        return (6);

    case AIRPORT + 15:
        *deltaHPtr = -3;
        *deltaVPtr = -2;
        return (6);

        //*** fourth row ***
    case AIRPORT + 18:
        *deltaHPtr = 0;
        *deltaVPtr = -3;
        return (6);

    case AIRPORT + 19:
        *deltaHPtr = -1;
        *deltaVPtr = -3;
        return (6);

    case AIRPORT + 20:
        *deltaHPtr = -2;
        *deltaVPtr = -3;
        return (6);

    case AIRPORT + 21:
        *deltaHPtr = -3;
        *deltaVPtr = -3;
        return (6);

    default:
        *deltaHPtr = 0;
        *deltaVPtr = 0;
        return (0);
    }
}


bool tally(int tileValue)
{
    /* can we autobulldoze this tile? */
    return (((tileValue >= FIRSTRIVEDGE) && (tileValue <= LASTRUBBLE)) ||
        ((tileValue >= (POWERBASE + 2)) && (tileValue <= (POWERBASE + 12))) ||
        ((tileValue >= TINYEXP) && (tileValue <= (LASTTINYEXP + 2))));/* ??? */
}


int checkSize(int temp)
{
    /* check for the normal com, resl, ind 3x3 zones & the fireDept & PoliceDept */
    if (((temp >= (RESBASE - 1)) && (temp <= (PORTBASE - 1))) ||
        ((temp >= (LASTPOWERPLANT + 1)) && (temp <= (POLICESTATION + 4))))
    {
        return (3);
    }
    else if (((temp >= PORTBASE) && (temp <= LASTPORT)) ||
        ((temp >= COALBASE) && (temp <= LASTPOWERPLANT)) ||
        ((temp >= STADIUMBASE) && (temp <= LASTZONE)))
    {
        return (4);
    }
    return (0);
}


/* 3x3 */
void check3x3border(int xMap, int yMap)
{
    int xPos, yPos;
    //int cnt;

    xPos = xMap; yPos = yMap - 1;
    for (int cnt = 0; cnt < 3; cnt++)
    {
        /*** this will do the upper bordering row ***/
        ConnecTile(xPos, yPos, &Map[xPos][yPos], 0);
        xPos++;
    }

    xPos = xMap - 1; yPos = yMap;
    for (int cnt = 0; cnt < 3; cnt++)
    {
        /*** this will do the left bordering row ***/
        ConnecTile(xPos, yPos, &Map[xPos][yPos], 0);
        yPos++;
    }

    xPos = xMap; yPos = yMap + 3;
    for (int cnt = 0; cnt < 3; cnt++)
    {
        /*** this will do the bottom bordering row ***/
        ConnecTile(xPos, yPos, &Map[xPos][yPos], 0);
        xPos++;
    }

    xPos = xMap + 3; yPos = yMap;
    for (int cnt = 0; cnt < 3; cnt++)
    {
        /*** this will do the right bordering row ***/
        ConnecTile(xPos, yPos, &Map[xPos][yPos], 0);
        yPos++;
    }
}


int check3x3(int mapH, int mapV, int base, Tool tool)
{
    int rowNum, columnNum;
    int holdMapH, holdMapV;
    int xPos, yPos;
    int cost = 0;
    int tileValue;
    int flag;

    mapH--; mapV--;
    if ((mapH < 0) || (mapH > (SimWidth - 3)) || (mapV < 0) || (mapV > (SimHeight - 3)))
    {
        return -1;
    }

    xPos = holdMapH = mapH;
    yPos = holdMapV = mapV;

    flag = 1;

    for (rowNum = 0; rowNum <= 2; rowNum++)
    {
        mapH = holdMapH;

        for (columnNum = 0; columnNum <= 2; columnNum++)
        {
            tileValue = Map[mapH++][mapV] & LOMASK;

            if (autoBulldoze)
            {
                /* if autoDoze is enabled, add up the cost of bulldozed tiles */
                if (tileValue != 0)
                {
                    if (tally(tileValue))
                    {
                        cost++;
                    }
                    else
                    {
                        flag = 0;
                    }
                }
            }
            else
            {
                /* check and see if the tile is clear or not  */
                if (tileValue != 0)
                {
                    flag = 0;
                }
            }
        }
        mapV++;
    }

    if (flag == 0)
    {
        return -1;
    }

    cost += Tools.at(tool).cost;

    if ((TotalFunds() - cost) < 0)
    {
        return -2;
    }

    if ((Players > 1) &&
        (OverRide == 0) &&
        (cost >= Expensive))
    {
        return -3;
    }

    /* take care of the money situtation here */
    Spend(cost);
    UpdateFunds();

    mapV = holdMapV;

    for (rowNum = 0; rowNum <= 2; rowNum++)
    {
        mapH = holdMapH;

        for (columnNum = 0; columnNum <= 2; columnNum++)
        {
            if (columnNum == 1 && rowNum == 1)
            {
                Map[mapH++][mapV] = base + BNCNBIT + ZONEBIT;
            }
            else
            {
                Map[mapH++][mapV] = base + BNCNBIT;
            }
            base++;
        }
        mapV++;
    }

    check3x3border(xPos, yPos);
    return 1;
}


/* 4x4 */
void check4x4border(int xMap, int yMap)
{
    //Ptr tilePtr;
    int* tilePtr;
    int xPos, yPos;
    int cnt;

    xPos = xMap; yPos = yMap - 1;
    for (cnt = 0; cnt < 4; cnt++)
    {
        /* this will do the upper bordering row */
        tilePtr = /*(Ptr)*/ &Map[xPos][yPos];
        ConnecTile(xPos, yPos, tilePtr, 0);
        xPos++;
    }

    xPos = xMap - 1; yPos = yMap;
    for (cnt = 0; cnt < 4; cnt++)
    {
        /* this will do the left bordering row */
        tilePtr = /*(Ptr)*/ &Map[xPos][yPos];
        ConnecTile(xPos, yPos, tilePtr, 0);
        yPos++;
    }

    xPos = xMap; yPos = yMap + 4;
    for (cnt = 0; cnt < 4; cnt++)
    {
        /* this will do the bottom bordering row */
        tilePtr = /*(Ptr)*/ &Map[xPos][yPos];
        ConnecTile(xPos, yPos, tilePtr, 0);
        xPos++;
    }

    xPos = xMap + 4; yPos = yMap;
    for (cnt = 0; cnt < 4; cnt++)
    {
        /* this will do the right bordering row */
        tilePtr = /*(Ptr)*/ &Map[xPos][yPos];
        ConnecTile(xPos, yPos, tilePtr, 0);
        yPos++;
    }
}


int check4x4(int mapH, int mapV, int base, int aniFlag, Tool tool)
{
    int rowNum, columnNum;
    int h, v;
    int holdMapH;
    int xMap, yMap;
    int tileValue;
    int flag;
    int cost = 0;

    mapH--; mapV--;
    if ((mapH < 0) || (mapH > (SimWidth - 4)) ||
        (mapV < 0) || (mapV > (SimHeight - 4)))
    {
        return -1;
    }

    h = xMap = holdMapH = mapH;
    v = yMap = mapV;

    flag = 1;

    for (rowNum = 0; rowNum <= 3; rowNum++)
    {
        mapH = holdMapH;

        for (columnNum = 0; columnNum <= 3; columnNum++)
        {
            tileValue = Map[mapH++][mapV] & LOMASK;

            if (autoBulldoze)
            {
                /* if autoDoze is enabled, add up the cost of bulldozed tiles */
                if (tileValue != 0)
                {
                    if (tally(tileValue))
                    {
                        cost++;
                    }
                    else
                    {
                        flag = 0;
                    }
                }
            }
            else
            {
                /* check and see if the tile is clear or not  */
                if (tileValue != 0)
                {
                    flag = 0;
                }
            }
        }
        mapV++;
    }

    if (flag == 0)
    {
        return -1;
    }

    cost += Tools.at(tool).cost;

    if ((TotalFunds() - cost) < 0)
    {
        return -2;
    }

    if ((Players > 1) &&
        (OverRide == 0) &&
        (cost >= Expensive))
    {
        return -3;
    }

    /* take care of the money situtation here */
    Spend(cost);
    UpdateFunds();

    mapV = v; holdMapH = h;

    for (rowNum = 0; rowNum <= 3; rowNum++)
    {
        mapH = holdMapH;

        for (columnNum = 0; columnNum <= 3; columnNum++)
        {
            if (columnNum == 1 && rowNum == 1)
            {
                Map[mapH++][mapV] = base + BNCNBIT + ZONEBIT;
            }
            else if (columnNum == 1 && rowNum == 2 && aniFlag)
            {
                Map[mapH++][mapV] = base + BNCNBIT + ANIMBIT;
            }
            else
            {
                Map[mapH++][mapV] = base + BNCNBIT;
            }
            base++;
        }
        mapV++;
    }

    check4x4border(xMap, yMap);
    return 1;
}


/* 6x6 */
void check6x6border(int xMap, int yMap)
{
    int xPos, yPos;
    int cnt;

    xPos = xMap; yPos = yMap - 1;
    for (cnt = 0; cnt < 6; cnt++)
    {
        /* this will do the upper bordering row */
        ConnecTile(xPos, yPos, &Map[xPos][yPos], 0);
        xPos++;
    }

    xPos = xMap - 1; yPos = yMap;
    for (cnt = 0; cnt < 6; cnt++)
    {
        /* this will do the left bordering row */
        ConnecTile(xPos, yPos, &Map[xPos][yPos], 0);
        yPos++;
    }

    xPos = xMap; yPos = yMap + 6;
    for (cnt = 0; cnt < 6; cnt++)
    {
        /* this will do the bottom bordering row */
        ConnecTile(xPos, yPos, &Map[xPos][yPos], 0);
        xPos++;
    }

    xPos = xMap + 6; yPos = yMap;
    for (cnt = 0; cnt < 6; cnt++)
    {
        /* this will do the right bordering row */
        ConnecTile(xPos, yPos, &Map[xPos][yPos], 0);
        yPos++;
    }
}


int check6x6(int mapH, int mapV, int base, Tool tool)
{
    int rowNum, columnNum;
    int h, v;
    int holdMapH;
    int xMap, yMap;
    int flag;
    int tileValue;
    int cost = 0;

    mapH--; mapV--;
    if ((mapH < 0) || (mapH > (SimWidth - 6)) ||
        (mapV < 0) || (mapV > (SimHeight - 6)))
    {
        return -1;
    }

    h = xMap = holdMapH = mapH;
    v = yMap = mapV;

    flag = 1;

    for (rowNum = 0; rowNum <= 5; rowNum++)
    {
        mapH = holdMapH;


        for (columnNum = 0; columnNum <= 5; columnNum++) {
            tileValue = Map[mapH++][mapV] & LOMASK;

            if (autoBulldoze)
            {
                /* if autoDoze is enabled, add up the cost of bulldozed tiles */
                if (tileValue != 0)
                    if (tally(tileValue))
                    {
                        cost++;
                    }
                    else
                    {
                        flag = 0;
                    }
            }
            else
            {
                /* check and see if the tile is clear or not  */
                if (tileValue != 0)
                {
                    flag = 0;
                }
            }
        }
        mapV++;
    }

    if (flag == 0)
    {
        return -1;
    }

    cost += Tools.at(tool).cost;

    if ((TotalFunds() - cost) < 0)
    {
        return -2;
    }

    if ((Players > 1) &&
        (OverRide == 0) &&
        (cost >= Expensive))
    {
        return -3;
    }

    /* take care of the money situtation here */
    Spend(cost);
    UpdateFunds();

    mapV = v; holdMapH = h;

    for (rowNum = 0; rowNum <= 5; rowNum++)
    {
        mapH = holdMapH;

        for (columnNum = 0; columnNum <= 5; columnNum++)
        {
            if (columnNum == 1 && rowNum == 1)
            {
                Map[mapH++][mapV] = base + BNCNBIT + ZONEBIT;
            }
            else
            {
                Map[mapH++][mapV] = base + BNCNBIT;
            }
            base++;
        }
        mapV++;
    }

    check6x6border(xMap, yMap);
    return 1;
}


/* QUERY */

/* search table for zone status string match */
static int idArray[28] =
{
    DIRT,
    RIVER,
    TREEBASE,
    RUBBLE,
    FLOOD,
    RADTILE,
    FIRE,
    ROADBASE,
    POWERBASE,
    RAILBASE,
    RESBASE,
    COMBASE,
    INDBASE,
    PORTBASE,
    AIRPORTBASE,
    COALBASE,
    FIRESTBASE,
    POLICESTBASE,
    STADIUMBASE,
    NUCLEARBASE,
    827, // crossed out lightning bolt
    832, // radar dish first frame
    FOUNTAIN,
    INDBASE2,
    FOOTBALLGAME1,
    VBRDG0,
    952, // radiation icon first frame
    956 // white tile?
};

/*
  0, 2, 21, 44, 
  48, 52, 53, 64,
  208, 224, 240, 423, 
  612, 693, 709, 745,
  761, 770, 779, 811, 
  827, 832, 840, 844,
  932, 948, 952, 956

  Clear, Water, Trees, Rubble, 
  Flood, Radioactive Waste, Fire, Road,
  Power, Rail, Residential, Commercial,
  Industrial, Port, AirPort, Coal Power,
  Fire Department, Police Department, Stadium, Nuclear Power, 
  Draw Bridge, Radar Dish, Fountain, Industrial,
  49er's 38  Bears 3, Draw Bridge, Ur 238
*/


int getDensityStr(int catNo, int mapH, int mapV)
{
    int z;

    switch (catNo)
    {
    case 0:
        z = PopDensity[mapH >> 1][mapV >> 1];
        z = z >> 6;
        z = z & 3;
        return (z);

    case 1:
        z = LandValueMem[mapH >> 1][mapV >> 1];
        if (z < 30) return (4);
        if (z < 80) return (5);
        if (z < 150) return (6);
        return (7);

    case 2:
        z = CrimeMem[mapH >> 1][mapV >> 1];
        z = z >> 6;
        z = z & 3;
        return (z + 8);

    case 3:
        z = PollutionMem[mapH >> 1][mapV >> 1];
        if ((z < 64) && (z > 0)) return (13);
        z = z >> 6;
        z = z & 3;
        return (z + 12);

    case 4:
        z = RateOGMem[mapH >> 3][mapV >> 3];
        if (z < 0) return (16);
        if (z == 0) return (17);
        if (z > 100) return (19);
        return (18);

    default:
        throw std::runtime_error("");
    }
}


struct ZoneStatsStrings
{
    const std::string str;
    const std::string s0;
    const std::string s1;
    const std::string s2;
    const std::string s3;
    const std::string s4;
};


void DoShowZoneStatus(const ZoneStatsStrings zoneStats, int x, int y)
{
    const std::string msg{
        "UIShowZoneStatus {" +
        zoneStats.str + "} {" +
        zoneStats.s0 + "} {" +
        zoneStats.s1 + "} {" +
        zoneStats.s2 + "} {" +
        zoneStats.s3 + "} {" +
        zoneStats.s4 + "} {" +
        std::to_string(x) + "} {" +
        std::to_string(y) + "}"
    };

    Eval(msg);
}


const std::string& queryString(int tileValue)
{
    for (int i = 1; i < 29; ++i)
    {
        if (tileValue < idArray[i])
        {
            /*
            int queryId = i - 1;
            if (queryId < 1 || queryId > 28)
            {
                queryId = 28;
            }
            */

            int queryId = std::clamp(i - 1, 0, 28);

            // \fixme ugly cast
            return QueryStatsString(static_cast<QueryStatsId>(queryId));

            break;
        }
    }

    return QueryStatsString(QueryStatsId::Padding2);
}


void doZoneStatus(int x, int y)
{
    int tileNum = Map[x][y] & LOMASK;
    if (tileNum >= COALSMOKE1 && tileNum < FOOTBALLGAME1)
    {
        tileNum = COALBASE;
    }

    std::string localStr = queryString(tileNum);
    std::array<std::string, 5> statusStr;

    for (int _x = 0; _x < 5; ++_x)
    {
        int id = getDensityStr(_x, x, y);
        id++;
        
        // \fixme ugly cast
        statusStr[_x] = ZoneStatsString(static_cast<ZoneStatsId>(std::clamp(id, 1, 20)));
    }

    DoShowZoneStatus({ localStr, statusStr[0], statusStr[1], statusStr[2], statusStr[3], statusStr[4] }, x, y);
}


void put3x3Rubble(int x, int y)
{
    for (int xx = x - 1; xx < x + 2; xx++)
    {
        for (int yy = y - 1; yy < y + 2; yy++)
        {
            if (CoordinatesValid(xx, yy, SimWidth, SimHeight))
            {
                int zz = Map[xx][yy] & LOMASK;
                if ((zz != RADTILE) && (zz != 0))
                {
                    Map[xx][yy] = (DoAnimation ? (TINYEXP + RandomRange(0, 2)) : SOMETINYEXP) | ANIMBIT | BULLBIT;
                }
            }
        }
    }
}


/* comefrom: processWand */
void put4x4Rubble(int x, int y)
{
    int xx, yy, zz;

    for (xx = x - 1; xx < x + 3; xx++)
    {
        for (yy = y - 1; yy < y + 3; yy++)
        {
            if (CoordinatesValid(xx, yy, SimWidth, SimHeight))
            {
                zz = Map[xx][yy] & LOMASK;
                if ((zz != RADTILE) && (zz != 0))
                {
                    Map[xx][yy] = (DoAnimation ? (TINYEXP + RandomRange(0, 2)) : SOMETINYEXP) | ANIMBIT | BULLBIT;
                }
            }
        }
    }
}


/* comefrom: processWand */
void put6x6Rubble(int x, int y)
{
    int xx, yy, zz;

    for (xx = x - 1; xx < x + 5; xx++)
    {
        for (yy = y - 1; yy < y + 5; yy++)
        {
            if (CoordinatesValid(xx, yy, SimWidth, SimHeight))
            {
                zz = Map[xx][yy] & LOMASK;
                if ((zz != RADTILE) && (zz != 0))
                {
                    Map[xx][yy] = (DoAnimation ? (TINYEXP + RandomRange(0, 2)) : SOMETINYEXP) | ANIMBIT | BULLBIT;
                }
            }
        }
    }
}


/************************************************************************/
/* TOOLS */


int query_tool(int x, int y)
{
    if (!CoordinatesValid(x, y, SimWidth - 1, SimHeight - 1))
    {
        return -1;
    }

    doZoneStatus(x, y);
    return 1;
}


int bulldozer_tool(int x, int y)
{
    unsigned int currTile, temp;
    int zoneSize, deltaH, deltaV;
    int result = 1;

    if ((x < 0) || (x > (SimWidth - 1)) || (y < 0) || (y > (SimHeight - 1)))
    {
        return -1;
    }

    currTile = Map[x][y];
    temp = currTile & LOMASK;

    if (currTile & ZONEBIT)
    { /* zone center bit is set */
        if (TotalFunds() > 0)
        {
            Spend(1);
            switch (checkSize(temp))
            {
            case 3:
                MakeSound("city", "Explosion-High");
                put3x3Rubble(x, y);
                break;

            case 4:
                put4x4Rubble(x, y);
                MakeSound("city", "Explosion-Low");
                break;

            case 6:
                MakeSound("city", "Explosion-High");
                MakeSound("city", "Explosion-Low");
                put6x6Rubble(x, y);
                break;

            default:
                break;
            }
        }
    }
    else if ((zoneSize = checkBigZone(temp, &deltaH, &deltaV)))
    {
        if (TotalFunds() > 0)
        {
            Spend(1);
            switch (zoneSize)
            {
            case 3:
                MakeSound("city", "Explosion-High");
                break;

            case 4:
                MakeSound("city", "Explosion-Low");
                put4x4Rubble(x + deltaH, y + deltaV);
                break;

            case 6:
                MakeSound("city", "Explosion-High");
                MakeSound("city", "Explosion-Low");
                put6x6Rubble(x + deltaH, y + deltaV);
                break;
            }
        }
    }
    else
    {
        if (temp == RIVER || temp == REDGE || temp == CHANNEL)
        {
            if (TotalFunds() >= 6)
            {
                result = ConnecTile(x, y, &Map[x][y], 1);
                if (temp != (Map[x][y] & LOMASK))
                {
                    Spend(5);
                }
            }
            else
            {
                result = 0;
            }
        }
        else
        {
            result = ConnecTile(x, y, &Map[x][y], 1);
        }
    }
    UpdateFunds();
    return result;
}


int road_tool(int x, int y)
{
    int result;

    if ((x < 0) || (x > (SimWidth - 1)) || (y < 0) || (y > (SimHeight - 1)))
    {
        return -1;
    }

    result = ConnecTile(x, y, &Map[x][y], 2);
    UpdateFunds();
    return result;
}


int rail_tool(int x, int y)
{
    int result;

    if ((x < 0) || (x > (SimWidth - 1)) ||
        (y < 0) || (y > (SimHeight - 1))) {
        return -1;
    }

    result = ConnecTile(x, y, &Map[x][y], 3);
    UpdateFunds();
    return result;
}


int wire_tool(int x, int y)
{
    int result;

    if ((x < 0) || (x > (SimWidth - 1)) ||
        (y < 0) || (y > (SimHeight - 1))) {
        return -1;
    }

    result = ConnecTile(x, y, &Map[x][y], 4);
    UpdateFunds();
    return result;
}


int park_tool(int x, int y)
{
    if (!CoordinatesValid(x, y, SimWidth - 1, SimHeight - 1))
    {
        return -1;
    }

    return putDownPark(x, y);
}


int residential_tool(int x, int y)
{
    if (!CoordinatesValid(x, y, SimWidth - 1, SimHeight - 1))
    {
        return -1;
    }

    return check3x3(x, y, RESBASE, Tool::Residential);
}


int commercial_tool(int x, int y)
{
    if (!CoordinatesValid(x, y, SimWidth - 1, SimHeight - 1))
    {
        return -1;
    }

    return  check3x3(x, y, COMBASE, Tool::Commercial);
}


int industrial_tool(int x, int y)
{
    if (!CoordinatesValid(x, y, SimWidth - 1, SimHeight - 1))
    {
        return -1;
    }

    return check3x3(x, y, INDBASE, Tool::Industrial);
}


int police_dept_tool(int x, int y)
{
    if (!CoordinatesValid(x, y, SimWidth - 1, SimHeight - 1))
    {
        return -1;
    }

    return check3x3(x, y, POLICESTBASE, Tool::Police);
}


int fire_dept_tool(int x, int y)
{
    if (!CoordinatesValid(x, y, SimWidth - 1, SimHeight - 1))
    {
        return -1;
    }

    return check3x3(x, y, FIRESTBASE, Tool::Fire);
}


int stadium_tool(int x, int y)
{
    if (!CoordinatesValid(x, y, SimWidth - 1, SimHeight - 1))
    {
        return -1;
    }

    return check4x4(x, y, STADIUMBASE, 0, Tool::Stadium);
}


int coal_power_plant_tool(int x, int y)
{
    if (!CoordinatesValid(x, y, SimWidth - 1, SimHeight - 1))
    {
        return -1;
    }

    return check4x4(x, y, COALBASE, 1, Tool::Coal);
}


int nuclear_power_plant_tool(int x, int y)
{
    if (!CoordinatesValid(x, y, SimWidth - 1, SimHeight - 1))
    {
        return -1;
    }

    return check4x4(x, y, NUCLEARBASE, 1, Tool::Nuclear);
}


int seaport_tool(int x, int y)
{
    if (!CoordinatesValid(x, y, SimWidth - 1, SimHeight - 1))
    {
        return -1;
    }

    return check4x4(x, y, PORTBASE, 0, Tool::Seaport);
}


int airport_tool(int x, int y)
{
    if (!CoordinatesValid(x, y, SimWidth - 1, SimHeight - 1))
    {
        return -1;
    }

    return check6x6(x, y, AIRPORTBASE, Tool::Airport);
}


int network_tool(int x, int y)
{
    if (!CoordinatesValid(x, y, SimWidth - 1, SimHeight - 1))
    {
        return -1;
    }

    return putDownNetwork(x, y);
}


int do_tool(Tool state, int x, int y, int first)
{
    switch (state)
    {
    case Tool::Residential:
        return residential_tool(x >> 4, y >> 4);
        break;

    case Tool::Commercial:
        return commercial_tool(x >> 4, y >> 4);
        break;

    case Tool::Industrial:
        return industrial_tool(x >> 4, y >> 4);
        break;

    case Tool::Fire:
        return fire_dept_tool(x >> 4, y >> 4);
        break;

    case Tool::Query:
        return query_tool(x >> 4, y >> 4);
        break;

    case Tool::Police:
        return police_dept_tool(x >> 4, y >> 4);
        break;

    case Tool::Wire:
        return wire_tool(x >> 4, y >> 4);
        break;

    case Tool::Bulldoze:
        return bulldozer_tool(x >> 4, y >> 4);
        break;

    case Tool::Rail:
        return rail_tool(x >> 4, y >> 4);
        break;

    case Tool::Road:
        return road_tool(x >> 4, y >> 4);
        break;

    case Tool::Stadium:
        return stadium_tool(x >> 4, y >> 4);
        break;

    case Tool::Park:
        return park_tool(x >> 4, y >> 4);
        break;

    case Tool::Seaport:
        return seaport_tool(x >> 4, y >> 4);
        break;

    case Tool::Coal:
        return coal_power_plant_tool(x >> 4, y >> 4);
        break;

    case Tool::Nuclear:
        return nuclear_power_plant_tool(x >> 4, y >> 4);
        break;

    case Tool::Airport:
        return airport_tool(x >> 4, y >> 4);
        break;

    case Tool::Network:
        return network_tool(x >> 4, y >> 4);
        break;

    default:
        return 0;
        break;
    }

    return 0;
}


/*
int current_tool(int x, int y, int first)
{
    return do_tool(tool_state, x, y, first);
}
*/

void DoTool(Tool tool, int x, int y)
{
    int result = do_tool(tool, x << 4, y << 4, 1);

    if (result == -1)
    {
        ClearMes();
        SendMes(NotificationId::MustBulldoze);
        MakeSoundOn(nullptr, "edit", "UhUh");
    }
    else if (result == -2)
    {
        ClearMes();
        SendMes(NotificationId::InsufficientFunds);
        MakeSoundOn(nullptr, "edit", "Sorry");
    }

    sim_skip = 0;
    InvalidateEditors();
}


void DoPendTool(int tool, int x, int y)
{
    Eval(std::string("DoPendTool: ") + "'winId' " + std::to_string(tool) + " " + std::to_string(x) + " " + std::to_string(y));
}


void ToolDown(int x, int y)
{
    /*
    int result;

    ViewToPixelCoords(view, x, y, x, y);
    view->last_x = x;
    view->last_y = y;

    result = current_tool(view, x, y, 1);

    if (result == -1)
    {
        ClearMes();
        SendMes(NotificationId::MustBulldoze);
        MakeSoundOn(view, "edit", "UhUh");
    }
    else if (result == -2)
    {
        ClearMes();
        SendMes(NotificationId::InsufficientFunds);
        MakeSoundOn(view, "edit", "Sorry");
    }
    else if (result == -3)
    {
        DoPendTool(view, view->tool_state, x >> 4, y >> 4);
    }

    sim_skip = 0;
    view->skip = 0;
    view->invalid = 1;
    */
}


void ToolDrag(int px, int py)
{
    /*
    int x, y, dx, dy, adx, ady, lx, ly, dist;
    float i, step, tx, ty, dtx, dty, rx, ry;

    ViewToPixelCoords(view, px, py, x, y);
    view->tool_x = x; view->tool_y = y;

    current_tool(view, x, y, 0);
    view->last_x = x; view->last_y = y;

    dist = ToolSize[view->tool_state];

    x >>= 4; y >>= 4;
    lx = view->last_x >> 4;
    ly = view->last_y >> 4;

    dx = x - lx;
    dy = y - ly;

    if (dx == 0 && dy == 0)
    {
        return;
    }

    adx = std::abs(dx); ady = std::abs(dy);

    if (adx > ady)
    {
        step = 0.3f / adx;
    }
    else
    {
        step = 0.3f / ady;
    }

    rx = (dx < 0 ? 1.0f : 0.0f);
    ry = (dy < 0 ? 1.0f : 0.0f);

    if (dist == 1)
    {
        for (i = 0.0; i <= 1 + step; i += step)
        {
            tx = (view->last_x >> 4) + i * dx;
            ty = (view->last_y >> 4) + i * dy;
            dtx = std::abs(tx - lx);
            dty = std::abs(ty - ly);
            if (dtx >= 1 || dty >= 1)
            {
                /* fill in corners
                if ((dtx >= 1) && (dty >= 1))
                {
                    if (dtx > dty)
                    {
                        current_tool(view, ((int)(tx + rx)) << 4, ly << 4, 0);
                    }
                    else
                    {
                        current_tool(view, lx << 4, ((int)(ty + ry)) << 4, 0);
                    }
                }
                lx = (int)(tx + rx);
                ly = (int)(ty + ry);
                current_tool(view, lx << 4, ly << 4, 0);
            }
        }
    }
    else
    {
        for (i = 0.0; i <= 1 + step; i += step)
        {
            tx = (view->last_x >> 4) + i * dx;
            ty = (view->last_y >> 4) + i * dy;
            dtx = std::abs(tx - lx);
            dty = std::abs(ty - ly);
            lx = (int)(tx + rx);
            ly = (int)(ty + ry);
            current_tool(view, lx << 4, ly << 4, 0);
        }

        view->last_x = (lx << 4) + 8;
        view->last_y = (ly << 4) + 8;
    }

    sim_skip = 0; /* update editors overlapping this one 
    view->skip = 0;
    view->invalid = 1;
    */
}


int ToolUp(int x, int y)
{
    //ToolDrag(view, x, y);
    return 0;
}
