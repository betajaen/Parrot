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

#include <exec/types.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/iffparse.h>
#include <libraries/iffparse.h>

#include <Parrot/Parrot.h>
#include <Parrot/Requester.h>
#include <Parrot/String.h>

typedef struct IFFHandle* IffPtr;

IffPtr OpenSquawkFile(UWORD id);
VOID CloseSquawkFile(IffPtr squawk);

VOID StartAssetList(IffPtr squawk, ULONG classType);
VOID EndAssetList(IffPtr squawk);
VOID SaveAssetQuick(IffPtr iff, APTR data, ULONG dataLength, ULONG classType, UWORD id, UWORD chunkHeaderflags);
VOID SaveAssetWithData(IffPtr iff, APTR data, ULONG dataLength, APTR data2, ULONG data2Length, ULONG classType, UWORD id, UWORD chunkHeaderflags);

UWORD GenerateAssetId(ULONG classType);
UWORD GenerateArchiveId();
VOID AddToTable(ULONG classType, UWORD id, UWORD archive, UWORD chapter);


#define GrowMem(VAL, NEED, CAPACITY) \
    if (NEED > CAPACITY)\
    {\
        if (NULL != VAL)\
          FreeMem(VAL, CAPACITY);\
        VAL = AllocMem(NEED, 0);\
        CAPACITY = NEED; \
    }
