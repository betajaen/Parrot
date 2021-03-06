/**
    $Id: Log.h 1.5 2021/02/21 10:19:00, betajaen Exp $

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

#ifndef _PARROT_LOG_H_
#define _PARROT_LOG_H_

#include <Parrot/Parrot.h>

void log_text(uint8 level, const char* text);
void log_fmt(uint8 level, const char* fmt, ...);

#ifndef PARROT_LOG_LEVEL
#define PARROT_LOG_LEVEL 0
#endif

void log_open();

void log_close();

#if PARROT_LOG_LEVEL <= 0
#define log_trace(text) log_text(0, text)
#define log_trace_fmt(fmt, ...) log_fmt(0, fmt, __VA_ARGS__)
#else
#define log_trace(text)
#define log_trace_fmt(fmt, ...)
#endif

#if PARROT_LOG_LEVEL <= 1
#define log_debug(text) log_text(1, text)
#define log_debug_fmt(fmt, ...) log_fmt(1, fmt, __VA_ARGS__)
#else
#define log_debug(text)
#define log_debug_fmt(fmt, ...)
#endif

#if PARROT_LOG_LEVEL <= 2
#define log_info(text) log_text(2, text)
#define log_info_fmt(fmt, ...) log_fmt(2, fmt, __VA_ARGS__)
#else
#define log_info(text)
#define log_info_fmt(fmt, ...)
#endif

void log_warn(const char* text);
void log_warn_fmt(const char* fmt, ...);

void log_error(const char* text);
void log_error_fmt(const char* fmt, ...);

#define assert(expression, text) \
  if ((expression) == FALSE)\
  { \
      log_error(text);\
  }

#endif
