/**
    $Id: Events.c, 1.1 2020/05/24 12:50:00, betajaen Exp $

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
#include <Parrot/Screen.h>
#include <Parrot/Requester.h>
#include <Parrot/Events.h>

#include <proto/exec.h>
#include <proto/intuition.h>

UWORD EvtKey;
WORD EvtMouseX;
WORD EvtMouseY;

UWORD WaitForEvents(UWORD screen)
{
  struct Window* win;
  struct IntuiMessage* imsg;
  UWORD  rc;

  EvtKey = 0;
  rc = 0;
  win = GetScreenWindow(screen);
  /* Wait(1L << win->UserPort->mp_SigBit); */

  while (imsg = (struct IntuiMessage*)GetMsg(win->UserPort))
  {
    switch (imsg->Class)
    {
      case IDCMP_RAWKEY:
      {
        rc |= WE_KEY;
        EvtKey = imsg->Code;
      }
      break;
      case IDCMP_MOUSEMOVE:
      {
        rc |= WE_CURSOR;
        EvtMouseX = imsg->MouseX;
        EvtMouseY = imsg->MouseY;
      }
      break;
    }


    ReplyMsg((struct Message*) imsg);
  }
  
  return rc;
}
