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

struct SQUAWK_ASSET_LIST_HEADER
{
  PtUnsigned32             al_Type;
  PtUnsigned16             al_Count;
  PtUnsigned16             al_Chapter;
  PtUnsigned32             al_Length;
};

typedef void(*LoadSpecialCallback)(PtAsset* asset, PtUnsigned16 counter, PtUnsigned32* readLength, APTR* readInto);

void Archives_Initialise();

void Archives_Unload(PtUnsigned16 olderThan);

void AssetTables_Load(PtUnsigned16 archive, PtUnsigned16 chapter, PtUnsigned16 count);

PtUnsigned16 Asset_LoadAll(PtUnsigned32 classType, PtUnsigned16 archiveId, struct ARENA* arena, PtAsset** outAssets, PtUnsigned16 outCapacity);

PtAsset* Asset_Load_KnownArchive(PtUnsigned32 classType, PtUnsigned16 archiveId, PtUnsigned16 id, struct ARENA* arena);

PtBool Asset_LoadInto_KnownArchive(PtUnsigned16 id, PtUnsigned16 archiveId, PtUnsigned32 classType, PtAsset* outAsset, PtUnsigned32 assetSize);

PtAsset* Asset_Load(PtUnsigned16 id, PtUnsigned16 chapter, PtUnsigned32 assetType, struct ARENA* arena);

PtBool Asset_LoadInto(PtUnsigned16 id, PtUnsigned16 chapter, PtUnsigned32 assetType, PtAsset* asset, PtUnsigned32 assetSize);

PtBool Asset_LoadInto_Callback(PtUnsigned16 id, PtUnsigned16 chapter, PtUnsigned32 assetType, PtAsset* asset, PtUnsigned32 assetSize, LoadSpecialCallback cb);

void Asset_CallbackFor_Image(PtAsset* asset, PtUnsigned16 counter, PtUnsigned32* readLength, APTR* readInto);

PtAsset* PtAsset_New(PtUnsigned32 size);

PtAsset* PtAsset_New1(PtUnsigned32 size, PtUnsigned32 dataSize, PtUnsigned32 numDataElements);

void PtAsset_Destroy(PtAsset* asset);

#endif
