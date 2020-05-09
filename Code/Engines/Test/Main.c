/**
    $Id: Test.c, 0.1, 2020/05/09 08:53:00, betajaen Exp $

    Test Engine for Parrot
    ======================

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

#include "Parrot.h"
#include "SDI_lib.h"

#include <proto/parrot.h>

struct TEST_STATE
{
  ULONG ts_Dummy;
};

struct ParrotBase* ParrotBase;

VOID GameInitialise(
  REG(a6, UNUSED __BASE_OR_IFACE),
  REG(a0, struct ParrotBase* parrot)
)
{
  // ParrotBase = parrot;
  // ParrotBase->State = (APTR) 1234;
  // TestRequester(1701);

  // parrot->State = parrot->pi_MemNew(sizeof(struct TEST_STATE), 0);
}

VOID GameShutdown(
  REG(a6, UNUSED __BASE_OR_IFACE)
)
{
  // ParrotBase->State = 0;
  //if (0UL != parrot->State)
  //{
  //  parrot->pi_MemDelete(parrot->State);
  //  parrot->State = 0UL;
  //}
}

BOOL OnGameEvent(
  REG(a6, UNUSED __BASE_OR_IFACE),
  REG(d0, ULONG event),
  REG(d1, ULONG data)
)
{
  return TRUE;
}
