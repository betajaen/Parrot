/**
    $Id: Input.h 1.2 2020/05/31 13:00:00, betajaen Exp $

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

#ifndef PARROT_INPUT_H
#define PARROT_INPUT_H

#include <Parrot/Parrot.h>

enum PtInputEventType
{
  PT_IET_KEYDOWN = 1,
  PT_IET_KEYUP = 2,
  PT_IET_SELECTDOWN = 3,
  PT_IET_SELECTUP = 4,
  PT_IET_SELECT = 5,
  PT_IET_MENUDOWN = 6,
  PT_IET_MENUUP = 7,
  PT_IET_MENU = 8,
  PT_IET_CURSOR = 9
};

enum PtKeyCode
{
  PT_KC_ESC    = 0x45,
  PT_KC_F1     = 0x50,
  PT_KC_F2     = 0x51,
  PT_KC_LSHIFT = 0x60
};

typedef struct PtInputEvent_t PtInputEvent;

struct PtInputEvent_t
{
  PtUnsigned16      ie_Type;
  PtUnsigned16      ie_Code;
  PtSigned16        ie_CursX;
  PtSigned16        ie_CursY;
};

PtPublic PtUnsigned16 InEvtForceQuit;
PtPublic PtUnsigned16 InEvtKey;
PtPublic PtSigned16  InMouseX;
PtPublic PtSigned16  InMouseY;

PtPublic PtByte  KeyState[];

PtPublic void InputInitialise();

PtPublic void InputExit();

PtPublic PtBool PopEvent(PtInputEvent* ie);

PtPublic PtBool IsMenuDown();

#endif
