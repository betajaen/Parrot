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

#define ID_MNIC MAKE_ID('M','N','I','C')

#define MAKE_ASSET_ID(ROOM, TYPE, ID) ( ((128+TYPE) << 24) | (ROOM << 24) | (ID & 0xFFff) )

#define ASSET_TYPE_ROOM_BACKDROP 1
#define ASSET_TYPE_ROOM_PALETTE  2

/* 
    Version
*/

#define CHUNK_VERSION_ID MAKE_ID('V','E','R','S')

struct CHUNK_VERSION
{
  ULONG GameId;
  UWORD ParrotVersionMajor;
  UWORD ParrotVersionMinor;
};

/*
    Backdrop
*/

#define CHUNK_BACKDROP_ID MAKE_ID('I','M','G','E')

struct CHUNK_BACKDROP
{
  UWORD Width;
  UWORD Height;
  UWORD Format;
  UWORD Compression;
  ULONG PaletteAssetId;
  ULONG ImageAssetId;
};

#define IMAGE_FORMAT_PALETTE 1

#define IMAGE_COMPRESSION_NONE 0

#endif
