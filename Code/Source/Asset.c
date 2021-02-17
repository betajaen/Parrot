/**
    $Id: Object.c, 1.2 2020/02/16 16:42:00, betajaen Exp $

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
#include <Parrot/Log.h>

#include <proto/exec.h>

PtPrivate PtUnsigned16 sNextObjectInstanceId = 0x8000;

PtAsset* PtAsset_New(PtUnsigned32 size)
{
  PtAsset* asset;

  asset = AllocMem(size, MEMF_CLEAR);

  asset->as_Id = 0;
  asset->as_Flags = PT_AF_INSTANCE | PT_AF_ARCH_ANY;
  asset->as_Length = size;

  return asset;
}

PtAsset* PtAsset_New1(PtUnsigned32 size, PtUnsigned32 dataSize, PtUnsigned32 numDataElements)
{
  PtAsset* asset;
  PtUnsigned32 realSize;

  realSize = size + (dataSize * numDataElements);

  asset = AllocMem(realSize, MEMF_CLEAR);

  asset->as_Id = 0;
  asset->as_Flags = PT_AF_INSTANCE | PT_AF_ARCH_ANY | PT_AF_HAS_DATA;
  asset->as_Length = realSize;

  return asset;
}

void PtAsset_Destroy(PtAsset* asset)
{
  if (asset == NULL)
  {
    WARNING("Tried to release a NULL asset!");
    return;
  }

  if (asset->as_Flags & PT_AF_INSTANCE)
  {
    TRACEF("ASSET Release. Releasing Instance %ld", asset->as_Id);
    FreeMem(asset, asset->as_Length);
    return;
  }
  else
  {
    TRACEF("ASSET Release. Asset %ld was not released! Due to unimplementation", asset->as_Id);
  }
}

