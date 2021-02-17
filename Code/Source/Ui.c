/**
    $Id: Ui.c, 1.2 2020/05/11 15:49:00, betajaen Exp $

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
#include <Parrot/Requester.h>
#include <Parrot/Log.h>
#include <Parrot/Api.h>
#include <Parrot/Squawk.h>

struct UiPanel* ActivePanels[MAX_UI_PANELS];

void Ui_Initialise()
{
  PtUnsigned16 ii;

  for (ii = 0; ii < MAX_UI_PANELS; ii++)
  {
    ActivePanels[ii] = NULL;
  }
}

void Ui_Teardown()
{
  PtUnsigned16 ii;

  for (ii = 0; ii < MAX_UI_PANELS; ii++)
  {
    if (ActivePanels[ii] != NULL)
    {
      FreeVec(ActivePanels[ii]);
      ActivePanels[ii] = NULL;
    }
  }
}

struct UiPanel* Ui_CreatePanel(PtUnsigned32 numElements, PtUnsigned16 screen)
{
  struct UiPanel* panel = PtAsset_New1(sizeof(struct UiPanel), sizeof(struct UiElement), numElements);

  panel->ui_NumElements = numElements;
  panel->ui_Screen = screen;
  
  return panel;
}

STATIC PtSigned32 FindFreeElementIndex(struct UiPanel* panel)
{
  PtUnsigned32 ii;

  for (ii = 0; ii < panel->ui_NumElements; ii++)
  {
    if (panel->ui_Elements[ii].ue_Type == UET_NONE)
      return ii;
  }

  return -1;
}

struct UiElement* Ui_CreateButton(struct UiPanel* panel, PtUnsigned16 index, PtUnsigned16 x, PtUnsigned16 y, PtUnsigned16 w, PtUnsigned16 h, PtDialogue text)
{
  struct UiElement* btn;
  PtSigned32 index;

  index = FindFreeElementIndex(panel);

  if (index == -1)
  {
    ERRORF("UI CreateButton. No Free Index in Panel %ld", panel->as_Id);
    return NULL;
  }

  btn = &panel->ui_Elements[index];

  btn->ue_Id = index;
  btn->ue_Type = UET_BUTTON;
  btn->ue_X = x;
  btn->ue_Y = y;
  btn->ue_Width = w;
  btn->ue_Height = h;

  btn->ue_Text = text;

  return btn;
}

void Ui_RunPanel(struct UiPanel* panel)
{

}

void Ui_Run()
{
  PtUnsigned16 ii;
  struct UiPanel* panel;

  for (ii = 0; ii < MAX_UI_PANELS; ii++)
  {
    panel = ActivePanels[ii];

    if (NULL != panel)
    {
      Ui_RunPanel(panel);
    }
  }
}

void Ui_AddPanel(struct UiPanel* panel)
{
  PtUnsigned16 ii;
  
  for (ii = 0; ii < MAX_UI_PANELS; ii++)
  {
    if (NULL == ActivePanels[ii])
    {
      ActivePanels[ii] = panel;
    }
  }
}

void Ui_ShowStartGamePanel()
{
  struct UiPanel* panel = Ui_CreatePanel(1, 0);
  

  struct UiElement* btn = Ui_CreateButton(panel, 1, 10, 10, 300, 50, "Start");

  if (btn != NULL)
  {
    btn->ue_Function[0] = UF_RunScript;
    btn->ue_Argument[0] = 1;
    btn->ue_Function[1] = UF_ReleaseThisPanel;
    btn->ue_Argument[0] = 0;
  }

  Ui_RunPanel(panel);
}
