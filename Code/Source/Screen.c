/**
    $Id: Screen.c, 0.1, 2020/05/07 15:55:00, betajaen Exp $

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

#include "Common.h"

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>

#define PARROT_SCREEN_CLASS 0x44a2d27d 

struct SCREEN_TARGET
{
  struct Screen*       st_Screen;
  struct Window*       st_Window;
  UWORD                st_ReadBuffer;
  UWORD                st_WriteBuffer;
  struct ScreenBuffer* st_Buffers[2];
  struct RastPort      st_RastPorts[2];
};

LONG RequesterF(CONST_STRPTR pOptions, CONST_STRPTR pFmt, ...);
APTR ObjAlloc(APTR arena, ULONG size, ULONG class);
ULONG ObjGetClass(APTR alloc);

VOID ScreenClear(APTR screen);

VOID ScreenSwapBuffers(APTR screen);

EXPORT APTR ScreenNew(APTR arena, struct SCREEN_INFO* info)
{
  struct NewScreen      newScreen;
  struct NewWindow      newWindow;
  struct SCREEN_TARGET* screen;

  screen = NULL;

  if (info->si_Width < 320)
  {
    info->si_Width = 320;
  }
  
  if (info->si_Height < 240)
  {
    info->si_Height = 240;
  }

  if (info->si_Depth < 2)
  {
    info->si_Depth = 2;
  }

  if (NULL == info->si_Title || '\0' == info->si_Title[0])
  {
    info->si_Title = "Parrot";
  }

  newScreen.LeftEdge = info->si_Left;
  newScreen.TopEdge = info->si_Top;
  newScreen.Width = info->si_Width;
  newScreen.Height = info->si_Height;
  newScreen.Depth = info->si_Depth;
  newScreen.ViewModes = 0;

  if ((info->si_Flags & SIF_IS_HIRES) != 0)
  {
    newScreen.ViewModes |= HIRES;
  }

  if ((info->si_Flags & SIF_IS_LACE) != 0)
  {
    newScreen.ViewModes |= LACE;
  }

  newScreen.Type = CUSTOMSCREEN;

  // if ((info->si_Flags & SIF_IS_PUBLIC) != 0)
  // {
  //   newScreen.Type |= PUBLICSCREEN;
  // }

  newScreen.BlockPen = BLOCKPEN;
  newScreen.DetailPen = DETAILPEN;

  newScreen.Font = NULL;
  newScreen.DefaultTitle = (UBYTE*)info->si_Title;
  newScreen.Gadgets = NULL;
  newScreen.CustomBitMap = NULL;

  screen = (struct SCREEN_TARGET*) ObjAlloc(arena, sizeof(struct SCREEN_TARGET), PARROT_SCREEN_CLASS);

  if (NULL == screen)
  {
    RequesterF("Close", "Out of arena memory for a new screen!");
    goto CLEAN_EXIT;
  }

  screen->st_Screen = OpenScreenTags(&newScreen,
    SA_Quiet, TRUE,
    SA_ShowTitle, FALSE,
    SA_Draggable, FALSE,
    SA_Exclusive, TRUE,
    SA_Type, CUSTOMSCREEN,
    TAG_END
  );

  if (NULL == screen->st_Screen)
  {
    RequesterF("Close", "Could not open screen %ldx%ldx%ld for Parrot", info->si_Width, info->si_Height, info->si_Depth);
    goto CLEAN_EXIT;
  }

  screen->st_ReadBuffer = 0;
  screen->st_WriteBuffer = 1;

  screen->st_Buffers[0] = AllocScreenBuffer(screen->st_Screen, NULL, SB_SCREEN_BITMAP);
  InitRastPort(&screen->st_RastPorts[0]);
  screen->st_RastPorts[0].BitMap = screen->st_Buffers[0]->sb_BitMap;

  screen->st_Buffers[1] = AllocScreenBuffer(screen->st_Screen, NULL, 0);
  InitRastPort(&screen->st_RastPorts[1]);
  screen->st_RastPorts[1].BitMap = screen->st_Buffers[1]->sb_BitMap;

  newWindow.LeftEdge = 0;
  newWindow.TopEdge = 0;
  newWindow.Width = info->si_Width;
  newWindow.Height = info->si_Height;

  screen->st_Window = OpenWindowTags(&newWindow,
    WA_CustomScreen, (Tag) screen->st_Screen,
    WA_Backdrop, TRUE,
    WA_DragBar, FALSE,
    WA_Borderless, TRUE,
    WA_SimpleRefresh, TRUE,
    WA_NoCareRefresh, TRUE,
    WA_Activate, TRUE,
    TAG_END
  );

  if (NULL == screen->st_Window)
  {
    RequesterF("Close", "Could not open window for Parrot", info->si_Width, info->si_Height, info->si_Depth);
    goto CLEAN_EXIT;
  }

  CLEAN_EXIT:

  return (APTR) screen;
}


EXPORT VOID ScreenDelete(APTR obj)
{
  struct SCREEN_TARGET* screen;

  if (NULL != obj && PARROT_SCREEN_CLASS == ObjGetClass(obj))
  {
    screen = (struct SCREEN_TARGET*) (obj);

    if (NULL != screen->st_Window)
    {
      ClearPointer(screen->st_Window);

      CloseWindow(screen->st_Window);
      screen->st_Window = 0;
    }

    if (NULL != screen->st_Screen)
    {
      if (NULL != screen->st_Buffers[0])
      {
        ChangeScreenBuffer(screen->st_Screen, screen->st_Buffers[0]);
        WaitTOF();
        WaitTOF();
        FreeScreenBuffer(screen->st_Screen, screen->st_Buffers[0]);
      }

      if (NULL != screen->st_Buffers[1])
      {
        ChangeScreenBuffer(screen->st_Screen, screen->st_Buffers[1]);
        WaitTOF();
        WaitTOF();
        FreeScreenBuffer(screen->st_Screen, screen->st_Buffers[1]);
      }

      screen->st_RastPorts[0].BitMap = NULL;
      screen->st_RastPorts[1].BitMap = NULL;

      CloseScreen(screen->st_Screen);
      screen->st_Screen = 0;

    }
  }
}

EXPORT VOID ScreenSetCursor(APTR screen, UBYTE cursor)
{
}

EXPORT UBYTE ScreenGetCursor(APTR screen)
{
  return CURSOR_POINT;
}

EXPORT VOID ScreenSetColour(APTR screen, UWORD index, UBYTE r, UBYTE g, UBYTE b)
{
}

EXPORT VOID ScreenClear(APTR obj)
{
  struct SCREEN_TARGET* screen;
  
  if (NULL != obj && PARROT_SCREEN_CLASS == ObjGetClass(obj))
  {
    screen = (struct SCREEN_TARGET*) (obj);

    ClearScreen(&screen->st_RastPorts[screen->st_WriteBuffer]);
  }
}

EXPORT VOID ScreenSwapBuffers(APTR obj)
{
  struct SCREEN_TARGET* screen;

  if (NULL != obj && PARROT_SCREEN_CLASS == ObjGetClass(obj))
  {
    screen = (struct SCREEN_TARGET*) (obj);

    if (ChangeScreenBuffer(screen->st_Screen, screen->st_Buffers[screen->st_WriteBuffer]))
    {
      screen->st_ReadBuffer ^= 1;
      screen->st_WriteBuffer ^= 1;
    }
  }
}
