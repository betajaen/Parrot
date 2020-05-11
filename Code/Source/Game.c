/**
    $Id: Game.c, 0.1, 2020/05/07 08:54:00, betajaen Exp $

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
#include <proto/dos.h>

#include <proto/parrot_game.h>

extern struct ParrotBase* ParrotBase;

LONG RequesterF(CONST_STRPTR pOptions, CONST_STRPTR pFmt, ...);
ULONG StrFormat(CHAR* pBuffer, LONG pBufferCapacity, CHAR* pFmt, ...);

EXPORT VOID GameStart(CONST_STRPTR name)
{
  CHAR buffer[1024];
  struct Library* GameBase;

  GameBase = NULL;

  if (0 == StrFormat(buffer, sizeof(buffer), "PROGDIR:Games/%s.library", name))
  {
    Alert(0x11110002);
    goto CLEAN_EXIT;
  }

  GameBase = OpenLibrary(buffer, 0);

  if (NULL == GameBase)
  {
    RequesterF("Okay", "Could not open %s", buffer);
    goto CLEAN_EXIT;
  }
  
  __GameInitialise(GameBase, ParrotBase);


CLEAN_EXIT:

  if (NULL != GameBase)
  {
    CloseLibrary(GameBase);
    GameBase = NULL;
  }

}

EXPORT VOID GameDelayTicks(UWORD ticks)
{
  Delay(ticks);
}

EXPORT VOID GameDelaySeconds(UWORD seconds)
{
  Delay(seconds * 50);
}
