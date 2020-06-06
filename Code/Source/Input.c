/**
    $Id: Events.c, 1.2 2020/05/24 12:50:00, betajaen Exp $

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
#include <Parrot/Input.h>
#include <Parrot/String.h>

#include <proto/exec.h>
#include <proto/input.h>
#include <proto/graphics.h>

#include <clib/alib_protos.h>

#include <exec/interrupts.h>
#include <devices/gameport.h>
#include <devices/timer.h>
#include <devices/keymap.h>
#include <devices/input.h>
#include <devices/inputevent.h>

#include <Parrot/Private/SDI_interrupt.h>

struct INPUTEVENT Inputs[MAX_INPUT_EVENT_SIZE] = { 0 };
UWORD InputWrite, InputRead;

UWORD CursorSelect[2], CursorMenu[2];

BYTE  KeyState[256] = { 0 };


EXTERN struct SimpleSprite CursorSprite;
EXTERN WORD CursorOffsetX;
EXTERN WORD CursorOffsetY;
EXTERN WORD CursorX;
EXTERN WORD CursorY;
EXTERN WORD CursorXLimit;
EXTERN WORD CursorYLimit;

#define SemAcquireInputRw()
#define SemReleaseInputRw() 

BOOL PopEvent(struct INPUTEVENT* ie)
{
  struct INPUTEVENT* popped;
  BOOL rc;

  rc = FALSE;

  if (InputWrite != InputRead)
  {
    popped = &Inputs[InputRead];
    ie->ie_Type = popped->ie_Type;
    ie->ie_Code = popped->ie_Code;
    ie->ie_CursX = popped->ie_CursX;
    ie->ie_CursY = popped->ie_CursY;
    rc = TRUE;

    InputRead = (InputRead + 1) & (MAX_INPUT_EVENT_SIZE - 1);
  }


  return rc;
}

VOID PushEvent(struct INPUTEVENT* ie)
{
  struct INPUTEVENT* pushed;

  pushed = &Inputs[InputWrite];
  pushed->ie_Type = ie->ie_Type;
  pushed->ie_Code = ie->ie_Code;
  pushed->ie_CursX = ie->ie_CursX;
  pushed->ie_CursY = ie->ie_CursY;

  InputWrite = (InputWrite + 1) & (MAX_INPUT_EVENT_SIZE - 1);
}

HANDLERPROTO(handlerfunc, ULONG, struct InputEvent* ie, APTR userdata)
{
  struct INPUTEVENT evt;
  BOOL hasMouseUpdate;
  WORD mouseX, mouseY, selectDown, selectUp, menuDown, menuUp;

  hasMouseUpdate = FALSE;
  mouseX = CursorX;
  mouseY = CursorY;

  while (ie != NULL)
  {
    if (ie->ie_Class == IECLASS_RAWKEY)
    {
      if ((ie->ie_Code & IECODE_UP_PREFIX) != 0)
      {
        evt.ie_Type = IET_KEYUP;
        evt.ie_Code = ie->ie_Code & ~IECODE_UP_PREFIX;
        evt.ie_CursX = 0;
        evt.ie_CursY = 0;
        PushEvent(&evt);
        KeyState[evt.ie_Code] = 0;
      }
      else
      {
        evt.ie_Type = IET_KEYDOWN;
        evt.ie_Code = ie->ie_Code;
        evt.ie_CursX = 0;
        evt.ie_CursY = 0;
        PushEvent(&evt);

        KeyState[evt.ie_Code] = 1;
      }
    }
    else if (ie->ie_Class == IECLASS_RAWMOUSE)
    {
      mouseX += ie->ie_X;
      mouseY += ie->ie_Y;
      
      if (mouseX < 0)
      {
        mouseX = 0;
      }
      else if (mouseX > CursorXLimit)
      {
        mouseX = CursorXLimit;
      }
      
      if (mouseY < 0)
      {
        mouseY = 0;
      }
      else if (mouseY > CursorYLimit)
      {
        mouseY = CursorYLimit;
      }

      if (ie->ie_Code != IECODE_NOBUTTON)
      {
        if (ie->ie_Code == IECODE_LBUTTON)
        {
          selectDown = TRUE;
        }

        if (ie->ie_Code == (IECODE_LBUTTON | IECODE_UP_PREFIX))
        {
          selectUp = TRUE;
        }

        if (ie->ie_Code == IECODE_RBUTTON)
        {
          menuDown = TRUE;
        }

        if (ie->ie_Code == (IECODE_RBUTTON | IECODE_UP_PREFIX))
        {
          menuUp = TRUE;
        }
      }
      
    }

    ie = ie->ie_NextEvent;
  }

  if ((CursorX != mouseX) || (CursorY != mouseY))
  {
    CursorX = mouseX;
    CursorY = mouseY;

    MoveSprite(NULL, &CursorSprite, CursorX + CursorOffsetX, CursorY + CursorOffsetY);

    evt.ie_Type = IET_CURSOR;
    evt.ie_Code = 0;
    evt.ie_CursX = CursorX;
    evt.ie_CursY = CursorY;
    PushEvent(&evt);
  }

  if (selectDown == TRUE)
  {
    evt.ie_Type = IET_SELECTDOWN;
    evt.ie_Code = 0;
    evt.ie_CursX = CursorX;
    evt.ie_CursY = CursorY;
    PushEvent(&evt);

    CursorSelect[0] = CursorSelect[1];
    CursorSelect[1] = TRUE;
  }

  if (selectUp == TRUE)
  {
    evt.ie_Type = IET_SELECTUP;
    evt.ie_Code = 0;
    evt.ie_CursX = CursorX;
    evt.ie_CursY = CursorY;
    PushEvent(&evt);

    CursorSelect[0] = CursorSelect[1];
    CursorSelect[1] = FALSE;

    if (CursorSelect[0] == TRUE)
    {
      evt.ie_Type = IET_SELECT;
      evt.ie_Code = 0;
      evt.ie_CursX = CursorX;
      evt.ie_CursY = CursorY;
      PushEvent(&evt);
    }
  }


  if (menuDown == TRUE)
  {
    evt.ie_Type = IET_MENUDOWN;
    evt.ie_Code = 0;
    evt.ie_CursX = CursorX;
    evt.ie_CursY = CursorY;
    PushEvent(&evt);

    CursorMenu[0] = CursorMenu[1];
    CursorMenu[1] = TRUE;
  }

  if (menuUp == TRUE)
  {
    evt.ie_Type = IET_MENUUP;
    evt.ie_Code = 0;
    evt.ie_CursX = CursorX;
    evt.ie_CursY = CursorY;
    PushEvent(&evt);

    CursorMenu[0] = CursorMenu[1];
    CursorMenu[1] = FALSE;

    if (CursorMenu[0] == TRUE)
    {
      evt.ie_Type = IET_MENU;
      evt.ie_Code = 0;
      evt.ie_CursX = CursorX;
      evt.ie_CursY = CursorY;
      PushEvent(&evt);
    }
  }

  return 0;
}

MakeHandlerPri(Handler, handlerfunc, "ParrotInput", NULL, 100);

STATIC struct Interrupt Handler;
STATIC struct MsgPort* KeyMsgPort = NULL;
STATIC struct IOStdReq* KeyIOReq = NULL;
UWORD InEvtForceQuit = FALSE;
UWORD InEvtKey = 0;

EXPORT VOID InputInitialise()
{
  InEvtForceQuit = FALSE;
  InEvtKey = 0;
  InputRead = 0;
  InputWrite = 1;
  CursorX = 0;
  CursorY = 0;

  KeyMsgPort = CreateMsgPort();
  KeyIOReq = (struct IOStdReq*) CreateExtIO(KeyMsgPort, sizeof(struct IOStdReq));

  OpenDevice("input.device", 0, (struct IORequest*) KeyIOReq, 0);

  KeyIOReq->io_Data = &Handler;
  KeyIOReq->io_Command = IND_ADDHANDLER;

  DoIO((struct IORequest*) KeyIOReq);

  // UBYTE port = 0;
  // KeyIOReq->io_Data = &port;
  // KeyIOReq->io_Flags = IOF_QUICK;
  // KeyIOReq->io_Length = 1;
  // KeyIOReq->io_Command = IND_SETMPORT;
  // BeginIO((struct IORequest*)KeyIOReq);

}

EXPORT VOID InputExit()
{
  if (KeyIOReq)
  {
    KeyIOReq->io_Data = &Handler;
    KeyIOReq->io_Command = IND_REMHANDLER;

    DoIO((struct IORequest*) KeyIOReq);

    if (!CheckIO((struct IORequest*)KeyIOReq))
    {
      AbortIO((struct IORequest*) KeyIOReq);
    }
    WaitIO((struct IORequest*) KeyIOReq);

    CloseDevice((struct IORequest*) KeyIOReq);
    DeleteExtIO((struct IORequest*) KeyIOReq);
    KeyIOReq = NULL;
  }

  if (KeyMsgPort)
  {
    DeleteMsgPort(KeyMsgPort);
  }
}
