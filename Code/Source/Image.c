/**
    $Id: Room.c, 1.2 2020/05/11 15:49:00, betajaen Exp $

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
#include <Parrot/Requester.h>

#include <proto/iffparse.h>
#include <proto/graphics.h>

EXPORT VOID UnpackBitmap(APTR asset, struct IFFHandle* iff)
{
  struct IMAGE*  img;
  UWORD          ii;
  
  img = (struct IMAGE*) asset;

  if (img->im_Planes[0] != NULL)
  {
    goto CLEAN_EXIT;
  }

  img->im_pad = 0;
  img->im_Flags = 0;

  for (ii = 0; ii < img->im_Depth; ii++)
  {
    img->im_Planes[ii] = (PLANEPTR) AllocRaster(img->im_Width, img->im_Height);

    if (img->im_Planes[ii] == NULL)
    {
      PARROT_ERR(
        "Out of Memory!\n"
        "Reason: No Chip Memory available for Raster"
        PARROT_ERR_INT("IMAGE::im_Width")
        PARROT_ERR_INT("IMAGE::im_Height")
        PARROT_ERR_INT("IMAGE::im_Depth")
        PARROT_ERR_INT("Bitplane"),
        img->im_Width, img->im_Height, img->im_Depth, (ULONG)ii
      );

      goto CLEAN_EXIT;
    }

    ReadChunkBytes(iff, img->im_Planes[ii], img->im_PlaneSize);
  }

  CLEAN_EXIT:
}

EXPORT VOID PackBitmap(APTR asset)
{
  struct IMAGE* img;
  UWORD ii;

  img = (struct IMAGE*) asset;

  if (img->im_Planes[0] != NULL)
  {
    for (ii = 0; ii < img->im_Depth; ii++)
    {
      FreeRaster(img->im_Planes[ii], img->im_Width, img->im_Height);
      img->im_Planes[ii] = NULL;
    }
  }
}
