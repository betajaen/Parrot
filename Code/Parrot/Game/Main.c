/**
    $Id: Main.c 1.5 2021/02/21 10:22:00, betajaen Exp $

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
#include <Parrot/Graphics.h>

static GraphicsViewInfo sGfxViews[] = {
  {
    .viewLeft = 0,
    .viewTop = 0,
    .viewWidth = 320,
    .viewHeight = 200,
    .depth = 4,
    .bitmapWidth = 640,
    .bitmapHeight = 200
  },
  {
    .viewLeft = 0,
    .viewTop = 200,
    .viewWidth = 320,
    .viewHeight = 80,
    .depth = 2,
    .bitmapWidth = 320,
    .bitmapHeight = 200
  },
  {
    .depth = 0
  }
};

void parrot_entry()
{
  log_info(PARROT_VERSION);
  
  if (gfx_setup() == FALSE)
    goto CleanExit;

  if (gfx_create_views(sGfxViews, 2) == FALSE)
  {
    gfx_teardown();
    goto CleanExit;
  }

  if (gfx_open_views() == FALSE)
  {
    gfx_destroy_views();
    gfx_teardown();
    goto CleanExit;
  }
  
  

CleanExit:
  gfx_teardown();
  
  log_info("Parrot Stopping.");
  log_close();
}
