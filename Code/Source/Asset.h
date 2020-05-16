/**
    $Id: Iff.h, 1.0, 2020/05/13 19:11:00, betajaen Exp $

    Game Converter for Parrot
    =========================

    Copyright 2020 Robin Southern http://github.com/betajaen/parrot

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#ifndef PARROT_IFF_H
#define PARROT_IFF_H

#include <exec/types.h>
#include "Common.h"

#define ID_SQWK MAKE_ID('S','Q','W','K')
#define ID_MNIC MAKE_ID('M','N','C','M')

#define MAKE_ASSET_ID(ROOM, TYPE, ID) ( ((128+TYPE) << 24) | (ROOM << 16) | (ID & 0xFFff) )

#define ASSET_TYPE_GAME_INFO            1
#define ASSET_TYPE_ROOM_BACKDROP        2
#define ASSET_TYPE_ROOM_COLOUR_PALETTE  3
#define ASSET_TYPE_IMAGE_DATA           4

struct CHUNK_HEADER
{
  UWORD Schema;
  UWORD MinVersion;
  ULONG Id;
};

/* 
    Game Info
*/

#define CHUNK_GAME_INFO_ID MAKE_ID('G','A','M','E')
#define CHUNK_GAME_INFO_SCHEMA_VERSION 0x0100
#define CHUNK_GAME_INFO_MIN_VERSION    0x0100

struct CHUNK_GAME_INFO
{
  struct CHUNK_HEADER Header;

  ULONG GameId;
  ULONG GameVersion;
  CHAR  Title[64];
  CHAR  ShortTitle[16];
  CHAR  Author[128];
  CHAR  Release[128];
  UWORD Width;
  UWORD Height;
  UWORD Depth;
};

/*
    Backdrop
*/

#define CHUNK_BACKDROP_ID MAKE_ID('B','K','D','P')
#define CHUNK_BACKDROP_SCHEMA 0x0100
#define CHUNK_BACKDROP_MIN_VERSION    0x0100

struct CHUNK_BACKDROP
{
  struct CHUNK_HEADER Header;

  UWORD Width;
  UWORD Height;
  ULONG PaletteId;
};

/*
    Colour Palette
*/

#define CHUNK_COLOUR_PALETTE_ID  MAKE_ID('C','P','A','L')
#define CHUNK_COLOUR_PALETTE_SCHEMA 0x0100
#define CHUNK_COLOUR_PALETTE_MIN_VERSION    0x0100

struct CHUNK_COLOUR_PALETTE
{
  struct CHUNK_HEADER Header;

  UBYTE NumColours;
  UBYTE Palette[256 * 3];
};

#endif
