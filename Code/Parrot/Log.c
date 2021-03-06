/**
    $Id: Log.c 1.5 2021/02/21 10:22:00, betajaen Exp $

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
#include <proto/dos.h>

extern void exit(); /* minstart.o */


static const char* LOG_LINES_PREFIX[5] =
{
  "[TRCE] ",
  "[DBUG] ",
  "[INFO] ",
  "[WARN] ",
  "[ERR!] "
};

static BPTR sLogFile = NULL;

void log_open()
{
  if (sLogFile != NULL)
    return;

  sLogFile = Open("Parrot:Parrot.log", MODE_NEWFILE);
}

void log_close()
{
  if (NULL == sLogFile)
    return;

  Close(sLogFile);
  sLogFile = NULL;
}

void log_text(uint8 level, const char* text)
{
  if (level > 4)
  {
    level = 4;
  }

  if (NULL == sLogFile)
  {
    log_open();
  }

  FPuts(sLogFile, LOG_LINES_PREFIX[level]);
  FPuts(sLogFile, text);
  FPutC(sLogFile, '\n');
  Flush(sLogFile);
}

void log_fmt(uint8 level, const char* fmt, ...)
{
  if (level > 4)
  {
    level = 4;
  }

  if (NULL == sLogFile)
  {
    log_open();
  }

  APTR arg;
  arg = (const char*)(&fmt + 1);


  FPuts(sLogFile, LOG_LINES_PREFIX[level]);
  VFPrintf(sLogFile, fmt, arg);
  FPutC(sLogFile, '\n');
  Flush(sLogFile);
}

void log_varadic(uint8 level, const char* fmt, APTR arg)
{
  if (level > 4)
  {
    level = 4;
  }

  if (NULL == sLogFile)
  {
    log_open();
  }

  FPuts(sLogFile, LOG_LINES_PREFIX[level]);
  VFPrintf(sLogFile, fmt, arg);
  FPutC(sLogFile, '\n');
  Flush(sLogFile);
}

void log_warn(const char* text)
{
  log_text(3, text);
}

void log_warn_fmt(const char* fmt, ...)
{
  APTR arg;
  arg = (const char*)(&fmt + 1);

  log_varadic(3, fmt, arg);
}

void log_error(const char* fmt)
{
  log_text(4, fmt);
  exit();
}

void log_error_fmt(const char* fmt, ...)
{
  APTR arg;
  arg = (const char*)(&fmt + 1);

  log_varadic(4, fmt, arg);

  exit();
}
