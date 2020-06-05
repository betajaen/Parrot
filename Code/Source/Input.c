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

extern struct SimpleSprite CursorSimpleSprite;
struct INPUTEVENT Inputs[MAX_INPUT_EVENT_SIZE] = { 0 };
UWORD InputWrite, InputRead;

STATIC WORD MouseX = 0;
STATIC WORD MouseY = 0;

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
    // Alert(0x1234);
  }

  SemAcquireInputRw();
  InputRead = (InputRead + 1) & (MAX_INPUT_EVENT_SIZE-1);
  SemReleaseInputRw();

  return rc;
}

VOID PushEvent(struct INPUTEVENT* ie)
{
  struct INPUTEVENT* pushed;

  if (InputWrite != InputRead)
  {
    pushed = &Inputs[InputRead];
    pushed->ie_Type = ie->ie_Type;
    pushed->ie_Code = ie->ie_Code;
    pushed->ie_CursX = ie->ie_CursX;
    pushed->ie_CursY = ie->ie_CursY;
  }

  SemAcquireInputRw();
  InputWrite = (InputWrite + 1) & (MAX_INPUT_EVENT_SIZE - 1);
  SemReleaseInputRw();
}

HANDLERPROTO(handlerfunc, ULONG, struct InputEvent* ie, APTR userdata)
{
  struct INPUTEVENT evt;

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
      }
      else
      {
        evt.ie_Type = IET_KEYDOWN;
        evt.ie_Code = ie->ie_Code;
        evt.ie_CursX = 0;
        evt.ie_CursY = 0;
        PushEvent(&evt);
      }
    }
    else if (ie->ie_Class == IECLASS_RAWMOUSE)
    {
      MouseX += ie->ie_X;
      MouseY += ie->ie_Y;

      if (MouseX < 0)
      {
        MouseX = 0;
      }
      else if (MouseX >= 320)
      {
        MouseX = 319;
      }

      if (MouseY < 0)
      {
        MouseY = 0;
      }
      // else if (MouseY >= 128)
      // {
      //   MouseY = 127;
      // }

      MoveSprite(NULL, &CursorSimpleSprite, MouseX-7, MouseY-7);

      evt.ie_Type = IET_CURSOR;
      evt.ie_Code = 0;
      evt.ie_CursX = MouseX;
      evt.ie_CursY = MouseY;
      PushEvent(&evt);
    }

    ie = ie->ie_NextEvent;
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
  InputWrite = 0;
  MouseX = 0;
  MouseY = 0;

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
