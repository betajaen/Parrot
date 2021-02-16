/**
    $Id: Log.h 1.2 2020/02/10 16:59:00, betajaen Exp $

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

#ifndef PARROT_LOG_H
#define PARROT_LOG_H

#include <exec/types.h>

#define LOG_TYPE_TRACE   0
#define LOG_TYPE_VERBOSE 1
#define LOG_TYPE_INFO    2
#define LOG_TYPE_WARNING 3
#define LOG_TYPE_ERROR   4

#ifndef PARROT_LOG_LEVEL
#define PARROT_LOG_LEVEL LOG_TYPE_TRACE
#endif

void Log_Initialise();

void Log_Shutdown();

void Log(PtUnsigned16 logType, CONST_STRPTR text);

void LogF(PtUnsigned16 logType, CONST_STRPTR fmt, ...);

#if PARROT_LOG_LEVEL <= LOG_TYPE_TRACE
#define TRACE(text) Log(LOG_TYPE_TRACE, text)
#define TRACEF(format, ...) LogF(LOG_TYPE_TRACE, format, __VA_ARGS__)
#else
#define TRACE(text)
#define TRACEF(format, ...)
#endif

#if PARROT_LOG_LEVEL <= LOG_TYPE_VERBOSE
  #define VERBOSE(text) Log(LOG_TYPE_VERBOSE, text)
  #define VERBOSEF(format, ...) LogF(LOG_TYPE_VERBOSE, format, __VA_ARGS__)
#else
  #define VERBOSE(text)
  #define VERBOSEF(format, ...)
#endif

#if PARROT_LOG_LEVEL <= LOG_TYPE_INFO
  #define INFO(text) Log(LOG_TYPE_INFO, text)
  #define INFOF(format, ...) LogF(LOG_TYPE_INFO, format, __VA_ARGS__)
#else
  #define INFO(text)
  #define INFOF(format, ...)
#endif

#if PARROT_LOG_LEVEL <= LOG_TYPE_WARNING
  #define WARNING(text) Log(LOG_TYPE_WARNING, text)
  #define WARNINGF(format, ...) LogF(LOG_TYPE_WARNING, format, __VA_ARGS__)
#else
  #define WARNING(text)
  #define WARNINGF(format, ...)
#endif

#if PARROT_LOG_LEVEL <= LOG_TYPE_ERROR
  #define ERROR(text) Log(LOG_TYPE_ERROR, text)
  #define ERRORF(format, ...) LogF(LOG_TYPE_ERROR, format, __VA_ARGS__)
#else
  #define ERROR(text)
  #define ERRORF(format, ...)
#endif

#endif
