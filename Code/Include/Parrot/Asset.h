/**
    $Id: Asset.h 1.2 2020/05/17 16:49:00, betajaen Exp $

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

#define ID_SQWK MAKE_NODE_ID('S','Q','W','K')
#define ID_MNIC MAKE_NODE_ID('M','N','C','M')

#if 1

VOID LoadAssetTables(UWORD archive, UWORD chapter, UWORD count);

APTR GetAsset(UWORD id, UWORD chapter, ULONG assetType, struct ARENA* arena);

VOID ReleaseAsset(APTR asset);

#else

APTR LoadAsset(struct ARENA* arena, UWORD archiveId, ULONG nodeType, UWORD assetId, UWORD arch);

void UnloadAsset(struct ARENA* arena, APTR asset);

#define LoadAssetT(T, ARENA, ARCHIVE, TYPE, ID, ARCH) \
  ( (T*) LoadAsset(ARENA, ARCHIVE, TYPE, ID, ARCH) )

UWORD FindAssetArchive(UWORD assetId, ULONG classType, ULONG arch);

#endif
