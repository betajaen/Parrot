/**
    $Id: Screen.c, 1.1 2020/05/07 15:55:00, betajaen Exp $

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
#include <Parrot/String.h>
#include <Parrot/Screen.h>

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/dos.h>

#include "Cursor.inc"

struct SCREEN
{
  struct Screen*       st_Screen;
  struct Window*       st_Window;
  UWORD                st_ReadBuffer;
  UWORD                st_WriteBuffer;
  struct ScreenBuffer* st_Buffers[2];
  struct RastPort      st_RastPorts[2];
  UWORD                st_IsDirty;
};

struct SCREEN Screens[MAX_SCREENS] = { 0 };

STATIC struct TextAttr myta = {
    "topaz.font",
    11,
    0,
    NULL
};

VOID ExitScreenNow()
{
  UWORD ii;
  struct SCREEN* screen;

  for (ii = 0; ii < MAX_SCREENS; ii++)
  {
    screen = &Screens[ii];

    if (screen->st_Window != NULL && screen->st_Screen != NULL)
    {
      ScreenClose(ii);
    }
  }
}

EXPORT VOID ScreenOpen(UWORD id, struct SCREEN_INFO* info)
{
  struct NewScreen      newScreen;
  struct NewWindow      newWindow;
  struct SCREEN* screen;
  struct Process* process;

  screen = NULL;
  InitStackVar(struct NewScreen, newScreen);
  InitStackVar(struct NewWindow, newWindow);

  if (id >= MAX_SCREENS)
  {
    ErrorF("Could not open screen %ld. Limit has reached", (ULONG) id);
  }

  if (info->si_Width < 320)
  {
    info->si_Width = 320;
  }
  
  if (info->si_Height < 24)
  {
    info->si_Height = 24;
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

  newScreen.Type = CUSTOMSCREEN | SCREENQUIET;

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
 
  screen = &Screens[id];
  screen->st_Screen = OpenScreen(&newScreen);

  if (NULL == screen->st_Screen)
  {
    ErrorF("Could not open screen %ldx%ldx%ld for Parrot", info->si_Width, info->si_Height, info->si_Depth);
  }

  screen->st_ReadBuffer = 0;
  screen->st_WriteBuffer = 1;
  screen->st_IsDirty = FALSE;
  
  screen->st_Buffers[0] = AllocScreenBuffer(screen->st_Screen, NULL, SB_SCREEN_BITMAP);
  InitRastPort(&screen->st_RastPorts[0]);
  screen->st_RastPorts[0].BitMap = screen->st_Buffers[0]->sb_BitMap;
  
  screen->st_Buffers[1] = AllocScreenBuffer(screen->st_Screen, NULL, 0);
  InitRastPort(&screen->st_RastPorts[1]);
  screen->st_RastPorts[1].BitMap = screen->st_Buffers[1]->sb_BitMap;

  newWindow.Width = info->si_Width;
  newWindow.Height = info->si_Height;
  newWindow.Screen = screen->st_Screen;
  newWindow.Type = CUSTOMSCREEN;
  newWindow.Flags = WFLG_BACKDROP | WFLG_BORDERLESS | WFLG_SIMPLE_REFRESH | WFLG_ACTIVATE | WFLG_REPORTMOUSE | WFLG_RMBTRAP;
  newWindow.IDCMPFlags = IDCMP_RAWKEY | IDCMP_MOUSEMOVE;

  screen->st_Window = OpenWindow(&newWindow);
  
  if (NULL == screen->st_Window)
  {
    ErrorF("Could not open window for Parrot", info->si_Width, info->si_Height, info->si_Depth);
  }
  

  if (id == 0)
  {
    SetRequesterWindow((APTR)screen->st_Window);

    process = (struct Process*) FindTask(NULL);
    process->pr_WindowPtr = screen->st_Window;
  }

}


EXPORT VOID ScreenClose(UWORD id)
{
  struct SCREEN* screen;

  if (id >= MAX_SCREENS)
  {
    ErrorF("Could not close screen %ld. Limit has reached", (ULONG)id);
  }

  screen = &Screens[id];

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

EXPORT VOID ScreenSetCursor(UWORD id, UBYTE type)
{
  struct SCREEN* screen;

  if (id >= MAX_SCREENS)
  {
    ErrorF("Unknown screen %ld", (ULONG)id);
  }

  screen = &Screens[id];

  UWORD* cursor = NULL;

  switch (type)
  {
    case 0: ClearPointer(screen->st_Window); break;
    case 1: SetPointer(screen->st_Window, Cursor1, 5, 5, -3, -3);   break;
    case 2: SetPointer(screen->st_Window, Cursor2, 13, 13, -6, -6); break;
    case 3: SetPointer(screen->st_Window, Cursor3, 15, 15, -8, -8); break;
    case 4: SetPointer(screen->st_Window, Cursor4, 9, 9, 0, 0); break;
    case 5: SetPointer(screen->st_Window, Cursor5, 9, 11, -3, 0); break;
    case 6: SetPointer(screen->st_Window, Cursor6, 9, 9, -8, 0); break;
    case 7: SetPointer(screen->st_Window, Cursor7, 11, 9, -10, -3); break;
    case 8: SetPointer(screen->st_Window, Cursor8, 9, 9, -8, -8); break;
    case 9: SetPointer(screen->st_Window, Cursor9, 9, 11, -3, -8); break;
    case 10: SetPointer(screen->st_Window, Cursor10, 9, 9, 0, -8); break;
    case 11: SetPointer(screen->st_Window, Cursor11, 11, 9, 0, -3); break;
    case 12: SetPointer(screen->st_Window, Cursor12, 15, 15, -7, -7); break;
    case 13: SetPointer(screen->st_Window, Cursor13, 9, 15, -3, 0); break;
    case 14: SetPointer(screen->st_Window, Cursor14, 9, 15, -3, 0); break;
    case 15: SetPointer(screen->st_Window, Cursor15, 9, 15, -3, 0); break;
    case 16: SetPointer(screen->st_Window, Cursor16, 13, 13, -6, -3); break;
    case 17: SetPointer(screen->st_Window, Cursor17, 15, 13, -6, -3); break;
    case 18: SetPointer(screen->st_Window, Cursor18, 15, 13, -6, -3); break;
    case 19: SetPointer(screen->st_Window, Cursor19, 14, 13, -6, -3); break;
    case 20: SetPointer(screen->st_Window, Cursor20, 16, 13, -7, -3); break;
    case 21: SetPointer(screen->st_Window, Cursor21, 16, 13, -7, -3); break;
    case 22: SetPointer(screen->st_Window, Cursor22, 16, 13, -7, -3); break;
    case 23: SetPointer(screen->st_Window, Cursor23, 15, 13, -7, -3); break;
    case 24: SetPointer(screen->st_Window, Cursor24, 16, 13, -7, -3); break;
    case 25: SetPointer(screen->st_Window, Cursor25, 16, 13, -7, -3); break;
    case 26: SetPointer(screen->st_Window, Cursor26, 16, 13, -7, -3); break;
    case 27: SetPointer(screen->st_Window, Cursor27, 14, 13, -6, -3); break;
    case 28: SetPointer(screen->st_Window, Cursor28, 16, 13, -7, -3); break;
  }
  
}

EXPORT VOID ScreenLoadPaletteTable(UWORD id, struct PALETTE_TABLE* paletteTable)
{
  struct SCREEN* screen;

  if (id >= MAX_SCREENS)
  {
    ErrorF("Unknown screen %ld", (ULONG)id);
  }

  screen = &Screens[id];

  LoadRGB32(&screen->st_Screen->ViewPort, (CONST ULONG*) &paletteTable->pt_Data);
  
}

EXPORT VOID ScreenClear(UWORD id)
{
  struct SCREEN* screen;

  if (id >= MAX_SCREENS)
  {
    ErrorF("Unknown screen %ld", (ULONG)id);
  }

  screen = &Screens[id];

  ClearScreen(&screen->st_RastPorts[screen->st_WriteBuffer]);
  screen->st_IsDirty = TRUE;
}

EXPORT VOID ScreenSwapBuffers(UWORD id)
{
  struct SCREEN* screen;

  if (id >= MAX_SCREENS)
  {
    ErrorF("Unknown screen %ld", (ULONG)id);
  }

  screen = &Screens[id];

  if (ChangeScreenBuffer(screen->st_Screen, screen->st_Buffers[screen->st_WriteBuffer]))
  {
    screen->st_ReadBuffer ^= 1;
    screen->st_WriteBuffer ^= 1;
    screen->st_IsDirty = FALSE;
  }
}

EXPORT VOID ScreenRpDrawImage(UWORD id, struct IMAGE* data, WORD leftOff, WORD topOff)
{
  struct Image image;
  struct SCREEN* screen;
  struct RastPort* rp;

  if (id >= MAX_SCREENS)
  {
    ErrorF("Unknown screen %ld", (ULONG)id);
  }

  screen = &Screens[id];
  rp = &screen->st_RastPorts[screen->st_WriteBuffer];

  image.LeftEdge = 0;
  image.TopEdge = 0;
  image.Width = data->im_Width;
  image.Height = data->im_Height;
  image.Depth = data->im_Depth;
  image.ImageData = (APTR)(data + 1);
  image.PlanePick = 15;
  image.PlaneOnOff = 0;
  image.NextImage = NULL;

  DrawImage(rp, &image, leftOff, topOff);
  screen->st_IsDirty = TRUE;
}

EXPORT VOID ScreenRpBlitBitmap(UWORD id, struct IMAGE* image, WORD dx, WORD dy, WORD sx, WORD sy, WORD sw, WORD sh)
{
  struct SCREEN* screen;
  struct RastPort* rp;
  PLANEPTR ptr;

  if (id >= MAX_SCREENS)
  {
    ErrorF("Unknown screen %ld", (ULONG)id);
  }

  screen = &Screens[id];
  rp = &screen->st_RastPorts[screen->st_WriteBuffer];

  BltBitMapRastPort((struct BitMap*) image, sx, sy, rp, dx, dy, sw, sh, 0xC0);
  screen->st_IsDirty = TRUE;
}

BOOL ScreenIsDirty(UWORD id)
{
  struct SCREEN* screen;

  if (id >= MAX_SCREENS)
  {
    ErrorF("Unknown screen %ld", (ULONG)id);
  }

  screen = &Screens[id];

  return screen->st_IsDirty;
}

VOID ScreenGetWidthHeight(UWORD id, UWORD* out_W, UWORD* out_H)
{
  struct SCREEN* screen;

  if (id >= MAX_SCREENS)
  {
    ErrorF("Unknown screen %ld", (ULONG)id);
  }

  screen = &Screens[id];
  *out_W = screen->st_Window->Width;
  *out_H = screen->st_Window->Height;
}

VOID Busy(UWORD screen)
{
  ScreenSetCursor(0, CURSOR_BUSY);
}

VOID NotBusy(UWORD screen)
{
  ScreenSetCursor(0, CURSOR_POINT);
}

struct Window* GetScreenWindow(UWORD id)
{
  if (id >= MAX_SCREENS)
  {
    ErrorF("Unknown screen %ld", (ULONG)id);
  }

  return Screens[id].st_Window;
}

VOID ScreenRpDrawBox(UWORD id, struct RECT* rect)
{
  struct SCREEN* screen;
  struct RastPort* rp;
  PLANEPTR ptr;

  if (id >= MAX_SCREENS)
  {
    ErrorF("Unknown screen %ld", (ULONG)id);
  }

  screen = &Screens[id];
  rp = &screen->st_RastPorts[screen->st_WriteBuffer];

  Move(rp, rect->rt_Left, rect->rt_Top);
  Draw(rp, rect->rt_Right, rect->rt_Top);
  Draw(rp, rect->rt_Right, rect->rt_Bottom);
  Draw(rp, rect->rt_Left, rect->rt_Bottom);
  Draw(rp, rect->rt_Left, rect->rt_Top);
}

VOID ScreenRpSetAPen(UWORD id, UWORD pen)
{
  struct SCREEN* screen;
  struct RastPort* rp;
  PLANEPTR ptr;

  if (id >= MAX_SCREENS)
  {
    ErrorF("Unknown screen %ld", (ULONG)id);
  }

  screen = &Screens[id];
  rp = &screen->st_RastPorts[screen->st_WriteBuffer];


  SetAPen(rp, pen);
}
