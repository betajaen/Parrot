/**
    $Id: Squawk.h 1.2 2020/11/24 06:30:00, betajaen Exp $

    Parrot - Point and Click Adventure Game Player
    ==============================================

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

#ifndef PARROT_SQUAWK_H
#define PARROT_SQUAWK_H

#include <Parrot/Parrot.h>

#define ID_SQWK MAKE_NODE_ID('S','Q','W','K')
#define ID_MNIC MAKE_NODE_ID('M','N','C','M')

struct ARENA;
struct ANY_ASSET;

typedef VOID(*LoadSpecialCallback)(struct ANY_ASSET* asset, UWORD counter, ULONG* readLength, APTR* readInto);

VOID Archives_Initialise();

struct ANY_ASSET* Archives_Unload(UWORD olderThan);

VOID AssetTables_Load(UWORD archive, UWORD chapter, UWORD count);

UWORD Asset_LoadAll(ULONG classType, UWORD archiveId, struct ARENA* arena, struct ANY_ASSET** outAssets, UWORD outCapacity);

struct ANY_ASSET* Asset_Load_KnownArchive(ULONG classType, UWORD archiveId, UWORD id, struct ARENA* arena);

BOOL Asset_LoadInto_KnownArchive(UWORD id, UWORD archiveId, ULONG classType, struct ANY_ASSET* outAsset, ULONG assetSize);

struct ANY_ASSET* Asset_Load(UWORD id, UWORD chapter, ULONG assetType, struct ARENA* arena);

BOOL Asset_LoadInto(UWORD id, UWORD chapter, ULONG assetType, struct ANY_ASSET* asset, ULONG assetSize);

VOID Asset_Unload(struct ANY_ASSET* asset);

VOID Asset_LoadInto_Callback(UWORD id, UWORD chapter, ULONG assetType, struct ANY_ASSET* asset, ULONG assetSize, LoadSpecialCallback cb);

VOID Asset_CallbackFor_Image(struct ANY_ASSET* asset, UWORD counter, ULONG* readLength, APTR* readInto);

#endif
