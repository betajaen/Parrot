/**
    $Id: Ui.h 1.5 2021/04/17 09:37:00, betajaen Exp $

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

#ifndef _PARROT_UI_H_
#define _PARROT_UI_H_

#include <Parrot/Parrot.h>

union _Event;
typedef struct _Button Button;

struct _Button
{
  char* text;
  uint16 left, top, right, bottom;
  uint8  view, state;
};


typedef enum _KeyState KeyState;

enum _KeyState
{
  ButtonState_New = 0,
  ButtonState_InsideUp = 1,
  ButtonState_InsideDown = 2,
  ButtonState_Inside = 3,
  ButtonState_Outside = 4
};



typedef enum _UiKeyState UiKeyState;

enum _UiKeyState
{
  UiKeyState_Up = 0,
  UiKeyState_Down = 1
};

typedef enum _UiKeyCode UiKeyCode;

enum _UiKeyCode
{
  UiKeyCode_Escape = 0x45,
  UiKeyCode_F1 = 0x50,
  UiKeyCode_F2 = 0x51,
  UiKeyCode_LeftShift = 0x60,
};

typedef struct _Key Key;

struct _Key
{
  uint8 key, state;
};

bool ui_handle_event(union _Event* evt);

bool ui_button(Button* btn);

bool ui_key(Key* key);

#endif
