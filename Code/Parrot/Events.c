/**
    $Id: Events.c 1.5 2021/04/17 09:52:00, betajaen Exp $

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

#include <Parrot/Events.h>
#include <Parrot/Log.h>

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

#include <Parrot/sdi/SDI_interrupt.h>

#define MAX_INPUT_EVENTS 32
Event EventRing[MAX_INPUT_EVENTS] = { 0 };
int EventSemaphore;
uint16 EventWrite, EventRead;
uint16 EventCounter;
int32  MouseX = 0;
int32  MouseY = 0;
int32  MouseLimitLeft = 0;
int32  MouseLimitTop = 0;
int32  MouseLimitRight = 319;
int32  MouseLimitBottom = 239;


#define PARROT_SEMAPHORE_ACQUIRE(X)
#define PARROT_SEMAPHORE_RELEASE(X)

static void evt_ring_push(Event* evt)
{
  PARROT_SEMAPHORE_ACQUIRE(EventSemaphore);

  Event* write = &EventRing[EventWrite];
  write->data[0] = evt->data[0];
  write->data[1] = evt->data[1];
  write->data[2] = evt->data[2];
  write->data[3] = evt->data[3];

  EventWrite = (EventWrite + 1) & (MAX_INPUT_EVENTS - 1);

  PARROT_SEMAPHORE_RELEASE(EventSemaphore);
}

static bool evt_ring_pop(Event* evt)
{
  bool didPop = FALSE;

  PARROT_SEMAPHORE_ACQUIRE(EventSemaphore);

  if (EventWrite != EventRead)
  {
    Event* read = &EventRing[EventRead];
    evt->data[0] = read->data[0];
    evt->data[1] = read->data[1];
    evt->data[2] = read->data[2];
    evt->data[3] = read->data[3];
    
    EventRead = (EventRead + 1) & (MAX_INPUT_EVENTS - 1);
    didPop = TRUE;
  }

  PARROT_SEMAPHORE_RELEASE(EventSemaphore);

  return didPop;
}


HANDLERPROTO(evt_handler, uint32, struct InputEvent* ie, APTR userdata)
{
  union _Event evt;

  while (ie != NULL)
  {
    if (ie->ie_Class == IECLASS_RAWKEY)
    {
      if ((ie->ie_Code & IECODE_UP_PREFIX))
      {
        evt.key.type = EventType_KeyUp;
        evt.key.num = EventCounter++;
        evt.key.key = ie->ie_Code & ~IECODE_UP_PREFIX;
        evt.key.pad = 0;
      }
      else
      {
        evt.key.type = EventType_KeyDown;
        evt.key.num = EventCounter++;
        evt.key.key = ie->ie_Code;
        evt.key.pad = 0;
      }

      evt_ring_push(&evt);
    }
    else if (ie->ie_Class == IECLASS_RAWMOUSE)
    {
      if (ie->ie_Code == IECODE_NOBUTTON)
      {
        MouseX += ie->ie_X;
        MouseY += ie->ie_Y;

        if (MouseX < MouseLimitLeft)
          MouseX = MouseLimitLeft;
        else if (MouseX > MouseLimitRight)
          MouseX = MouseLimitRight;

        if (MouseY < MouseLimitTop)
          MouseY = MouseLimitTop;
        else if (MouseY > MouseLimitBottom)
          MouseY = MouseLimitBottom;

        evt.mouse.type = EventType_MouseMove;
        evt.key.num = EventCounter++;
        evt.mouse.x = MouseX;
        evt.mouse.y = MouseY;

        evt_ring_push(&evt);
      }
      else
      {
        
      }
    }

    ie = ie->ie_NextEvent;
  }

  return 0;
}

MakeHandlerPri(Handler, evt_handler, "ParrotInput", NULL, 100);

STATIC struct Interrupt Handler;
STATIC struct MsgPort* KeyMsgPort = NULL;
STATIC struct IOStdReq* KeyIOReq = NULL;
uint16 InEvtForceQuit = FALSE;
uint16 InEvtKey = 0;

bool evt_initialise()
{
  KeyMsgPort = CreateMsgPort();
  KeyIOReq = (struct IOStdReq*)CreateExtIO(KeyMsgPort, sizeof(struct IOStdReq));

  OpenDevice("input.device", 0, (struct IORequest*)KeyIOReq, 0);

  KeyIOReq->io_Data = &Handler;
  KeyIOReq->io_Command = IND_ADDHANDLER;

  DoIO((struct IORequest*)KeyIOReq);

  return TRUE;
}

void evt_teardown()
{
  if (KeyIOReq)
  {
    KeyIOReq->io_Data = &Handler;
    KeyIOReq->io_Command = IND_REMHANDLER;

    DoIO((struct IORequest*)KeyIOReq);

    if (!CheckIO((struct IORequest*)KeyIOReq))
    {
      AbortIO((struct IORequest*)KeyIOReq);
    }
    WaitIO((struct IORequest*)KeyIOReq);

    CloseDevice((struct IORequest*)KeyIOReq);
    DeleteExtIO((struct IORequest*)KeyIOReq);
    KeyIOReq = NULL;
  }

  if (KeyMsgPort)
  {
    DeleteMsgPort(KeyMsgPort);
  }
}

bool evt_pop(Event* evt)
{
  return evt_ring_pop(evt);
}

void evt_warp_cursor(int32 x, int32 y)
{
  PARROT_SEMAPHORE_ACQUIRE(EventSemaphore);

  MouseX = x;
  MouseY = y;

  if (MouseX < MouseLimitLeft)
    MouseX = MouseLimitLeft;
  else if (MouseX > MouseLimitRight)
    MouseX = MouseLimitRight;

  if (MouseY < MouseLimitTop)
    MouseY = MouseLimitTop;
  else if (MouseY > MouseLimitBottom)
    MouseY = MouseLimitBottom;

  PARROT_SEMAPHORE_RELEASE(EventSemaphore);
}

void evt_limit_cursor(int32 left, int32 top, int32 right, int32 bottom)
{
  PARROT_SEMAPHORE_ACQUIRE(EventSemaphore);

  MouseLimitLeft = left;
  MouseLimitTop = top;
  MouseLimitRight = right;
  MouseLimitBottom = bottom;

  PARROT_SEMAPHORE_RELEASE(EventSemaphore);
}
