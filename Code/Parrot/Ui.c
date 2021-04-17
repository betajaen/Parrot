/**
    $Id: Ui.c 1.5 2021/04/17 09:37:00, betajaen Exp $

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

#include <Parrot/Ui.h>
#include <Parrot/Log.h>
#include <Parrot/Graphics.h>
#include <Parrot/Events.h>

uint8 gKeyboard[255] = { 0 };

bool ui_handle_event(union _Event* evt)
{
  if (evt->key.type == EventType_KeyUp)
  {
    log_trace_fmt("Handle Key Event Key=%lx, State = Up ", evt->key.key);
    gKeyboard[evt->key.key] = 0;
    return TRUE;
  }
  else if (evt->key.type == EventType_KeyDown)
  {
    log_trace_fmt("Handle Key Event Key=%lx, State = Down ", evt->key.key);
    gKeyboard[evt->key.key] = 1;
    return TRUE;
  }
}

bool ui_button(Button* btn)
{

  if (btn == NULL)
  {
#if defined(PARROT_DEBUG)
    log_warn_fmt("Button used in %s is null", PARROT_STRINGIFY(__FUNCTION__));
#endif
    return FALSE;
  }

  switch (btn->state)
  {
    case ButtonState_New:
    {
      btn->state = ButtonState_Outside;
      gfx_apen(btn->view, 0);
      gfx_box(btn->view, btn->left, btn->top, btn->right, btn->bottom);

      log_trace_fmt("%s New Button! %lx", PARROT_STRINGIFY(__FUNCTION__), btn);

      return FALSE;
    }
  }

  return FALSE;
}

bool ui_key(Key* btn)
{

  if (btn == NULL)
  {
#if defined(PARROT_DEBUG)
    log_warn_fmt("KeyButton used in %s is null", PARROT_STRINGIFY(__FUNCTION__));
#endif
    return FALSE;
  }

  uint8 now = gKeyboard[btn->key];
  uint8 was = btn->state;

  btn->state = now;

  return (now == UiKeyState_Up && was == UiKeyState_Down);
}
