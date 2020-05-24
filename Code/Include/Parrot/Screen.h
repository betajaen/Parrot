/**
    $Id: Screen.h 1.1 2020/05/17 16:10:00, betajaen Exp $

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

VOID ScreenOpen(UWORD id, struct SCREEN_INFO* info);

VOID ScreenClose(UWORD id);

VOID ScreenSetCursor(UWORD id, UBYTE cursor);

VOID ScreenClear(UWORD id);

VOID ScreenSwapBuffers(UWORD id);

VOID ScreenLoadPaletteTable(UWORD id, struct PALETTE_TABLE* paletteTable);

VOID ScreenRpDrawImage(UWORD id, struct IMAGE* data, WORD leftOff, WORD topOff);

VOID ScreenRpBlitBitmap(UWORD id, struct IMAGE* image, WORD dx, WORD dy, WORD sx, WORD sy, WORD sw, WORD sh);

BOOL ScreenIsDirty(UWORD id);

VOID ScreenGetWidthHeight(UWORD id, UWORD* out_W, UWORD* out_H);

VOID Busy();

VOID NotBusy();

struct Window* GetScreenWindow(UWORD id);

