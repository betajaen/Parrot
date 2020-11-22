/**
    $Id: Dialogue.c, 1.0, 2020/05/13 07:16:00, betajaen Exp $

    Maniac Game Converter for Parrot
    ================================

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

#include <Squawk/Squawk.h>

#define MAX_STRINGS_PER_TABLE 256
#define MAX_STRING_ALLOCATION (255 + 1 + 1 + 1)   /* len + text + null + pad */
#define MAX_TABLE_SIZE (MAX_STRINGS_PER_TABLE * MAX_STRING_ALLOCATION)
#define MAX_TABLES 16

struct WRITE_STRING_TABLE
{
  UWORD st_Id;
  UWORD st_Count;
  ULONG st_Write;
  ULONG st_Hashes[256];
  struct STRING_TABLE st_Table;
};

static struct WRITE_STRING_TABLE*  StringTables[MAX_TABLES];

STATIC struct WRITE_STRING_TABLE*  NewTable(UWORD language)
{
  UWORD id;
  struct WRITE_STRING_TABLE* tbl;

  for (id = 0; id < MAX_TABLES; id++)
  {
    if (StringTables[id] == NULL)
    {
      break;
    }
  }

  if (id == MAX_TABLES)
  {
    PARROT_ERR0(
      "Unable serialise assets.\n"
      "Reason: Ran out of Dialogue Space"
    );
  }

  tbl = AllocMem(sizeof(struct WRITE_STRING_TABLE) + MAX_TABLE_SIZE, MEMF_CLEAR);

  tbl->st_Id = 1 + id;
  tbl->st_Table.st_Language = language;

  StringTables[id] = tbl;

  RequesterF("OK", "New Table = %ld", (ULONG)id);

  return tbl;
}

VOID StartDialogue()
{
  UWORD ii;

  for (ii = 0; ii < MAX_TABLES; ii++)
  {
    StringTables[ii] = NULL;
  }
}

VOID EndDialogue()
{
  UWORD ii;
  struct WRITE_STRING_TABLE* tbl;

  for (ii = 0; ii < MAX_TABLES; ii++)
  {
    tbl = StringTables[ii];

    if (tbl != NULL)
    {
      FreeMem(tbl, sizeof(struct WRITE_STRING_TABLE) + MAX_TABLE_SIZE);
    }

    StringTables[ii] = NULL;
  }
}

struct WRITE_STRING_TABLE* GetOrAddTable(UWORD language, ULONG estimatedNeedSize)
{
  UWORD ii;
  struct WRITE_STRING_TABLE* tbl;
  UWORD writeEnd;

  for (ii = 0; ii < MAX_TABLES; ii++)
  {
    tbl = StringTables[ii];

    if (tbl == NULL)
      continue;

    if (tbl->st_Table.st_Language != language)
      continue;

    if (tbl->st_Count == MAX_STRINGS_PER_TABLE)
      continue;

    writeEnd = tbl->st_Write + estimatedNeedSize;

    if (writeEnd >= 65535)
      continue;

    return tbl;
  }

  return NewTable(language);
}

VOID ExportDialogue(IffPtr iff)
{
  UWORD ii;
  struct WRITE_STRING_TABLE* tbl;
  BOOL hasWrittenHeader;

  hasWrittenHeader = FALSE;

  for (ii = 0; ii < MAX_TABLES; ii++)
  {
    tbl = StringTables[ii];

    if (tbl != NULL)
    {
      if (hasWrittenHeader == FALSE)
      {
        hasWrittenHeader = TRUE;
        StartAssetList(iff, CT_STRING_TABLE);
      }

      SaveAssetWithData(iff,
        &tbl->st_Table,
        sizeof(struct STRING_TABLE),
        &tbl->st_Table.st_Text[0],
        tbl->st_Write,
        CT_STRING_TABLE,
        ii,
        CHUNK_FLAG_ARCH_ANY
      );
    }
  }

  if (hasWrittenHeader)
  {
    EndAssetList(iff);
  }
}

STATIC ULONG FindDialogue(UWORD language, ULONG hash)
{
  UWORD ii, jj, count;
  struct WRITE_STRING_TABLE* tbl;
  union DIALOGUE_TEXT dialogue;

  for (ii = 0; ii < MAX_TABLES; ii++)
  {
    tbl = StringTables[ii];

    if (tbl != NULL)
    {
      count = tbl->st_Count;

      for (jj = 0; jj < count; jj++)
      {
        if (tbl->st_Hashes[jj] == hash)
        {
          dialogue.dt_Parts.dp_Magic = DIALOGUE_MAGIC;
          dialogue.dt_Parts.dp_Table = tbl->st_Id;
          dialogue.dt_Parts.dp_Item = jj;

          return dialogue.dt_AssetId;
        }
      }
    }
  }

  return 0;
}

STATIC ULONG WriteDialogue(UWORD language, UBYTE textLength, STRPTR text, ULONG hash)
{
  struct WRITE_STRING_TABLE* tbl;
  ULONG writeEnd;
  CHAR* dst;
  UWORD ii;
  union DIALOGUE_TEXT dialogue;

  tbl = GetOrAddTable(language, textLength + 3);
  writeEnd = tbl->st_Write + textLength + 2;  /* text + null + length */

  dst = &tbl->st_Table.st_Text[tbl->st_Write];

  *dst++ = textLength;  /* Length */

  for (ii = 0; ii < textLength; ii++)
  {
    *dst++ = *text++; /* Text */
  }

  if ((textLength & 1) != 0)
  {
    *dst++ = 0; /* Padding */
    writeEnd++;
  }
  
  tbl->st_Write = writeEnd;

  dialogue.dt_Parts.dp_Magic = DIALOGUE_MAGIC;
  dialogue.dt_Parts.dp_Table = tbl->st_Id;
  dialogue.dt_Parts.dp_Item = tbl->st_Count;

  tbl->st_Table.st_Offsets[tbl->st_Count] = tbl->st_Write;
  tbl->st_Hashes[tbl->st_Count] = hash;
  tbl->st_Write = writeEnd;
  tbl->st_Count++;

  return dialogue.dt_AssetId;
}


ULONG PushDialogue(UWORD language, UBYTE textLength, STRPTR text)
{
  ULONG hash, assetId;

  if (textLength == 0)
    return 0;

  hash = StrHash(textLength, text);
  assetId = FindDialogue(language, hash);

  if (assetId != 0)
    return assetId;

  return WriteDialogue(language, textLength, text, hash);
}
