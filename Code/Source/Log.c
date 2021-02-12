/**
    $Id: Log.c, 1.2 2021/02/10 16:56:00, betajaen Exp $

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

#include <proto/exec.h>
#include <proto/dos.h>

static BPTR sLog = NULL;

void Log_Initialise()
{
  if (sLog == NULL)
  {
    sLog = Open("Parrot:Parrot.log", MODE_NEWFILE);
  }
}

void Log_Shutdown()
{
  if (sLog != NULL)
  {
    Close(sLog);
    sLog = NULL;
  }
}

void Log(UWORD logType, CONST_STRPTR text)
{
  switch (logType)
  {
    case LOG_TYPE_TRACE:   FPuts(sLog, "TRC "); break;
    case LOG_TYPE_VERBOSE: FPuts(sLog, "VRB "); break;
    case LOG_TYPE_INFO:    FPuts(sLog, "INF "); break;
    case LOG_TYPE_WARNING: FPuts(sLog, "WAR "); break;
    case LOG_TYPE_ERROR:   FPuts(sLog, "ERR "); break;
  }

  FPuts(sLog, text);
  FPutC(sLog, '\n');
  Flush(sLog);
}

void LogF(UWORD logType, CONST_STRPTR format, ...)
{
  APTR arg;
  arg = (CONST_STRPTR*)(&format + 1);

  switch (logType)
  {
    case LOG_TYPE_TRACE:    FPuts(sLog, "TRC "); break;
    case LOG_TYPE_VERBOSE:  FPuts(sLog, "VRB "); break;
    case LOG_TYPE_INFO:     FPuts(sLog, "INF "); break;
    case LOG_TYPE_WARNING:  FPuts(sLog, "WAR "); break;
    case LOG_TYPE_ERROR:    FPuts(sLog, "ERR "); break;
  }

  VFPrintf(sLog, format, arg);
  FPutC(sLog, '\n');
  Flush(sLog);
}
