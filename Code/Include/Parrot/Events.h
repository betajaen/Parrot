/**
    $Id: Events.h 1.1 2020/05/24 12:51:00, betajaen Exp $

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

UWORD WaitForEvents(UWORD screen);

#define WE_NONE    (1 << 0)
#define WE_KEY     (1 << 1)
#define WE_CURSOR  (1 << 2)
#define WE_SELECT  (1 << 3)
#define WE_MENU    (1 << 4)

extern UWORD EvtKey;
extern WORD  EvtMouseX;
extern WORD  EvtMouseY;


#define KC_ESC 0x45
#define KC_F1  0x50
#define KC_F2  0x51
#define KC_F3  0x52
#define KC_LEFT 0x4F
#define KC_RIGHT 0x4E
#define KC_DOWN 0x4D
#define KC_UP 0x4C
