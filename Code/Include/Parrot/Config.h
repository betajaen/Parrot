/**
    $Id: Config.h 1.2 2021/02/12 17:06:00, betajaen Exp $

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

#ifndef PARROT_CONFIG_H
#define PARROT_CONFIG_H

#define MAX_SCREENS                   2
#define MAX_ROOM_BACKDROPS            2
#define MAX_ROOM_EXITS                10
#define MAX_ROOM_ENTITIES             20
#define MAX_ENTITY_NAME_LENGTH        29
#define MAX_VIEW_LAYOUTS              2
#define MAX_INPUT_EVENT_SIZE          32
#define MAX_UI_PANELS                 2

/* 
  Maximum number of globals for scripting.
  Value must be power-of-two
*/
#define MAX_SCRIPT_GLOBALS            64


#define MAX_ROOM_SCRIPTS              8
#define MAX_ENTITY_SCRIPTS            4
#define MAX_ENTITY_IMAGES             4
#define MAX_VIRTUAL_MACHINES          8
#define MAX_VM_STACK_SIZE             16
#define MAX_VM_VARIABLES              16
#define MAX_VM_GLOBALS                64
#define MAX_OPEN_ARCHIVES             8
#define DEFAULT_ARCHIVE_PATH_FSTR     "PROGDIR:Tools/%lld.parrot"

/*
  Maximum number of constants per script.
  Value must be power-of-two
*/
#define MAX_CONSTANTS_PER_SCRIPT      8

#endif
