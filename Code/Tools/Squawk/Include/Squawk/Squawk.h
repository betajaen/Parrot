/**
    $Id: Squawk.h, 1.2, 2020/11/15 06:48:00, betajaen Exp $

    Squawk Header for Parrot Exporters
    ==================================

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

#ifndef SQUAWK_H
#define SQUAWK_H

#include <exec/types.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/iffparse.h>
#include <libraries/iffparse.h>

#include <Parrot/Parrot.h>
#include <Parrot/Requester.h>
#include <Parrot/String.h>

typedef struct Archive* SquawkPtr;

SquawkPtr OpenSquawkFile(PtUnsigned16 id);
void CloseSquawkFile(SquawkPtr squawk);

void StartAssetList(SquawkPtr squawk, PtUnsigned32 classType, PtUnsigned16 chapter);
void EndAssetList(SquawkPtr squawk);
void SaveAsset(SquawkPtr squawk, PtAsset* asset, PtUnsigned32 assetSize);
void SaveAssetExtra(SquawkPtr squawk, PtAsset* asset, PtUnsigned32 assetSize, APTR data, PtUnsigned32 dataLength);

PtUnsigned16 GenerateAssetId(PtUnsigned32 classType);
PtUnsigned16 GenerateArchiveId();
void AddToTable(PtUnsigned32 classType, PtUnsigned16 id, PtUnsigned16 archive, PtUnsigned16 chapter);
PtUnsigned32 PushDialogue(PtUnsigned16 language, PtUnsigned8 textLength, STRPTR text);

#define GrowMem(VAL, NEED, CAPACITY) \
    if (NEED > CAPACITY)\
    {\
        if (NULL != VAL)\
          FreeMem(VAL, CAPACITY);\
        VAL = AllocMem(NEED, 0);\
        CAPACITY = NEED; \
    }

#endif
