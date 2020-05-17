/**
    $Id: Arena.c, 0.1, 2020/05/11 10:48:00, betajaen Exp $

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

#include <Parrot/Parrot.h>
#include "Asset.h"

#include <proto/exec.h>

struct MinList Assets = {
    NULL,
    NULL,
    NULL
};

LONG RequesterF(CONST_STRPTR pOptions, CONST_STRPTR pFmt, ...);
struct ASSET* LoadRoom(APTR arena, ULONG id);
struct ASSET* LoadImage(APTR arena, ULONG id);

EXPORT struct ASSET* LoadAsset(APTR arena, ULONG id)
{
  struct ASSET* asset;
  UBYTE         type;

  for (asset = ((struct ASSET*) Assets.mlh_Head);
       asset != NULL; 
       asset = ((struct ASSET*)asset->as_Node.mln_Succ))
  {
    if (asset->as_Id == id)
    {
      return asset;
    }
  }

  type = (id >> 24) & 0xFF;
  type -= 128;

  switch (type)
  {
    case ASSET_TYPE_ROOM:
      asset = LoadRoom(arena, id);
      AddTail((struct List*) &Assets, (struct Node*) asset);
    return asset;
    case ASSET_TYPE_BACKDROP:
      asset = LoadImage(arena, id);
      AddTail((struct List*) &Assets, (struct Node*) asset);
    return asset;
  }

  RequesterF("OK", "Could not load asset %lx", id);

  return NULL;
}

EXPORT void UnloadAsset(APTR arena, struct ASSET* asset)
{
  
}
