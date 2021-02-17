/**
    $Id: Main.c, 1.0, 2021/02/06 11:10:00, betajaen Exp $

    Virtual Machine Opcode Generator
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

#include <exec/types.h>
#include <proto/exec.h>
#include <proto/dos.h>

#include <Parrot/Parrot.h>
#include <Parrot/String.h>

struct DosLibrary* DOSBase;

/* Constants */

#define OPCODE_DIR "Parrot:Code/Documentation/Development/Schemas/Opcodes"
#define DOCUMENTATION_PATH "Parrot:Code/Documentation/Development/opcodes.txt"
#define CWRITER_PATH "Parrot:Code/Tools/Squawk/Include/Squawk/Writer/Script_Opcodes.inc"
#define CREADER_PATH "Parrot:Code/Source/Vm_RunOpcode.inc"
#define COPCODES_HEADER_PATH "Parrot:Code/Include/Parrot/Vm_Opcodes.h"
#define COPCODES_SOURCE_PATH "Parrot:Code/Source/Vm_Opcodes.c"

/* Opcodes */

#define AT_NONE       0
#define AT_STACK      1
#define AT_IMM        2
#define AT_SIGNED_IMM 3
#define AT_CONSTANT   4

struct Argument
{
  PtChar  VarName;
  PtChar* Name;
  PtUnsigned8 Type;
  PtByte  StackAddress;
};

#define MAX_OPCODES 64
#define MAX_ARGUMENTS 4

struct Opcode
{
  char* Name;
  char* Description;
  char* Code;
  PtBool  IsBig;
  PtUnsigned16 NumArguments;
  struct Argument Args[MAX_ARGUMENTS];
};

STATIC PtUnsigned16 NumOpcodes;
STATIC struct Opcode Opcodes[MAX_OPCODES] = { 0 };

STATIC PtUnsigned32 sTmpStrLen;
STATIC PtChar sTmpStr[256];
STATIC PtChar Line[512];
STATIC PtBool LineIsEof;
STATIC PtChar SectionText[1024];
STATIC PtUnsigned16 SectionTextLength;

void ReadOpcode(char* path);

/* Printing Macros */

#define CLI_PRINT(TEXT) \
  Write(Output(), TEXT, sizeof(TEXT));

#define CLI_PRINTF(FMT, ...) \
  sTmpStrLen = StrFormat(sTmpStr, sizeof(sTmpStr), FMT, __VA_ARGS__); \
  Write(Output(), sTmpStr, sTmpStrLen-1);

#define BUFFERSIZE 512

/* File Macros */

#define FILE_PRINT(FILE, TEXT) \
  Write(FILE, TEXT, sizeof(TEXT)-1);

#define FILE_PRINTF(FILE, FMT, ...) \
  sTmpStrLen = StrFormat(sTmpStr, sizeof(sTmpStr), FMT, __VA_ARGS__); \
  Write(FILE, sTmpStr, sTmpStrLen-1);

#define FILE_PRINTCHAR(FILE, CH) \
  Write(FILE, &CH, 1);

void PrintUpperCase(BPTR file, const char* text)
{
  PtChar temp;

  while (*text != 0)
  {
    if (*text >= 'a' && *text <= 'z')
    {
      temp = *text - 'a' + 'A';
      Write(file, &temp, 1);
    }
    else
    {
      Write(file, text, 1);
    }

    text++;
  }
}

void ReadOpcodeDirectory(const char* source)
{
  struct ExAllControl* excontrol;
  struct ExAllData* ead, * buffer;
  BPTR            sourcelock;
  PtBool            exmore;
  PtSigned32            error;

  if ( (buffer = AllocMem(BUFFERSIZE, MEMF_CLEAR)) == 0)
    return;

  if ((sourcelock = Lock(source, SHARED_LOCK)) == 0)
  {
    FreeMem(buffer, BUFFERSIZE);
    CLI_PRINT("Cannot lock source");
    return;
  }

  if ((excontrol = AllocDosObject(DOS_EXALLCONTROL, NULL)) == 0)
  {
    UnLock(sourcelock);
    FreeMem(buffer, 512);
    CLI_PRINT("Cannot alloc Doc Object");
    return;
  }

  do
  {
    exmore = ExAll(sourcelock, buffer, BUFFERSIZE, ED_NAME, excontrol);
    error = IoErr();

    if ((exmore == NULL && (error != ERROR_NO_MORE_ENTRIES)))
      break;

    if (excontrol->eac_Entries == 0)
      continue;

    ead = buffer;

    do {
      StrFormat(sTmpStr, sizeof(sTmpStr), "%s/%s", source, ead->ed_Name);
      
      char* path = StrDuplicate(sTmpStr);
      ReadOpcode(path);
      FreeMem(path, StrLength(path) + 1);
      NumOpcodes++;

      ead = ead->ed_Next;
    } while (ead);

  } while (exmore);

  if (error != ERROR_NO_MORE_ENTRIES)
  {
    PrintFault(error, NULL);
  }

  FreeDosObject(DOS_EXALLCONTROL, excontrol);
  UnLock(sourcelock);
  FreeMem(buffer, BUFFERSIZE);
}

PtBool ReadLine(BPTR file)
{
  PtChar ch;
  PtUnsigned16 ii;

  for (ii = 0; ii < sizeof(Line)-1; ii++)
  {
    ch = FGetC(file);

    if (ch == -1)
    {
      LineIsEof = TRUE;
      break;
    }

    if (ch == '\n')
    {
      break;
    }

    Line[ii] = (PtChar) ch;
  }

  Line[ii] = 0;

  return ii;
}

#define SECTION_NONE 0
#define SECTION_NAME 1
#define SECTION_DESCRIPTION 2
#define SECTION_ARGUMENT 3
#define SECTION_CODE 4

PtChar* DuplicateSectionText(PtUnsigned16 start)
{
  PtChar* text;
  PtUnsigned16 ii,jj;
  
  text = AllocVec(SectionTextLength + 1, MEMF_ANY);

  for (ii = start, jj = 0; ii < SectionTextLength; ii++, jj++)
  {
    text[jj] = SectionText[ii];
  }

  text[jj] = 0;

  return text;
}

void FreeOpcode(PtUnsigned16 id)
{
  PtUnsigned16 ii;

  struct Opcode* opcode;
  opcode = &Opcodes[id];

  if (opcode->Name != NULL)
  {
    FreeVec(opcode->Name);
    opcode->Name = NULL;
  }

  if (opcode->Description != NULL)
  {
    FreeVec(opcode->Description);
    opcode->Description = NULL;
  }

  if (opcode->Code != NULL)
  {
    FreeVec(opcode->Code);
    opcode->Code = NULL;
  }

  for (ii = 0; ii < opcode->NumArguments; ii++)
  {
    FreeVec(opcode->Args[ii].Name);
    opcode->Args[ii].Name = NULL;
  }

}

void CommitOpcodeSection(PtUnsigned16 section)
{
  struct Opcode* opcode;
  opcode = &Opcodes[NumOpcodes];

  if (section == SECTION_NAME)
  {
    opcode->Name = DuplicateSectionText(0);
  }
  else if (section == SECTION_DESCRIPTION)
  {
    opcode->Description = DuplicateSectionText(0);
  }
  else if (section == SECTION_CODE)
  {
    opcode->Code = DuplicateSectionText(0);
  }
}

void CommitArgument()
{
  PtUnsigned16 ii;
  struct Opcode* opcode;
  opcode = &Opcodes[NumOpcodes];
  PtUnsigned16 nameText = 0;
  PtChar ch = 0;

  if (opcode->NumArguments == MAX_ARGUMENTS)
  {
    CLI_PRINTF("Opcode %ld %s has too many arguments", NumOpcodes, opcode->Name);
    return;
  }

  struct Argument* arg;
  arg = &opcode->Args[opcode->NumArguments++];

  PtChar type = SectionText[0];

  switch (type)
  {
    case 's':
    case 'S':
    {
      arg->Type = AT_SIGNED_IMM;
    }
    break;
    case 'i':
    case 'I':
    {
      arg->Type = AT_IMM;
    }
    break;
    case '0':
    {
      arg->Type = AT_STACK;
      arg->StackAddress = 0;
    }
    break;
    case '1':
    {
      arg->Type = AT_STACK;
      arg->StackAddress = -1;
    }
    break;
    case '2':
    {
      arg->Type = AT_STACK;
      arg->StackAddress = -2;
    }
    break;
    case '3':
    {
      arg->Type = AT_STACK;
      arg->StackAddress = -3;
    }
    break;
    default:
    {
      CLI_PRINTF("Opcode has unknown arg type! %ld %s -> %s", NumOpcodes, opcode->Name, (PtChar*) SectionText);
      return;
    }
    return;
  }

  for (ii = 1; ii < SectionTextLength; ii++)
  {
    ch = SectionText[ii];

    if (nameText == NULL)
    {
      if (ch == ':' || ch == ' ')
        continue;

      nameText = ii;
    }

    if (ch >= 'A' && ch <= 'Z')
    {
      arg->VarName = ch;

      if (nameText != 0)
        break;
    }
  }

  if (nameText == 0)
  {
    CLI_PRINTF("Opcode has no description %ld %s -> %s", NumOpcodes, opcode->Name, (PtChar*)SectionText);
    return;
  }

  if (arg->VarName == 0)
  {
    CLI_PRINTF("Opcode has no Id; the first capital letter of the description. %ld %s -> %s", NumOpcodes, opcode->Name, (PtChar*)SectionText);
    return;
  }

  arg->Name = DuplicateSectionText(nameText);

}

void WriteDocumentation(PtUnsigned16 id, BPTR file)
{
  struct Opcode* opcode;
  struct Argument* arg;
  PtChar tmpStr[2];
  PtUnsigned16 ii;

  opcode = &Opcodes[id];

  if (opcode->Name == NULL)
    return;

  FILE_PRINTF(file, "## %s\n\n", opcode->Name);
  FILE_PRINTF(file, "%s\n\n", opcode->Description);
  FILE_PRINTF(file, "Id\n  %ld\n", (PtUnsigned32) id);
  FILE_PRINTF(file, "Code\n%s\n", opcode->Code);

  for (ii = 0; ii < opcode->NumArguments; ii++)
  {
    arg = &opcode->Args[ii];
    
    tmpStr[0] = arg->VarName;
    tmpStr[1] = 0;

    FILE_PRINTF(file, "Arg '#%s'\n", &tmpStr);
    FILE_PRINTF(file, "  Name\n    %s\n", arg->Name);
    FILE_PRINT(file, "  Type\n    ");
    switch (arg->Type)
    {
      case AT_STACK:
      {
        FILE_PRINTF(file, "Value at Stack[%ld]\n", (PtSigned32)arg->StackAddress);
      }
      break;
      case AT_IMM:
      {
        FILE_PRINT(file, "10-bit\n  Encoding\n    Bits 6-15\n");
      }
      break;
      case AT_SIGNED_IMM:
      {
        FILE_PRINT(file, "10-bit signed\n  Encoding\n    Bits 6-15\n");
      }
      break;
    }
  }

  FILE_PRINT(file, "\n");
}

void WriteDocumentationFile()
{
  BPTR file;
  PtSigned32 err;
  PtUnsigned16 ii;

  file = Open(DOCUMENTATION_PATH, MODE_NEWFILE);

  if (file == 0)
  {
    err = IoErr();
    PrintFault(err, NULL);
    CLI_PRINTF("Cannot open file %s\n", DOCUMENTATION_PATH);
    return;
  }

  FILE_PRINT(file, "Opcodes\n");
  FILE_PRINT(file, "# Opcodes\n");


  for (ii = 0; ii < MAX_OPCODES; ii++)
  {
    WriteDocumentation(ii, file);
  }

  Flush(file);
  Close(file);
}

void WriteCWriter(PtUnsigned16 id, BPTR file)
{
  struct Opcode* opcode;
  struct Argument* arg;
  PtChar tmpStr[2];
  PtUnsigned16 ii;
  PtBool hasStack, hasArgs;

  opcode = &Opcodes[id];

  if (opcode->Name == NULL)
    return;

  FILE_PRINT(file, "/*\n");
  
  FILE_PRINTF(file, "  %s\n", opcode->Name);

  FILE_PRINTF(file, "  %s\n", opcode->Description);
  
  hasStack = FALSE;
  hasArgs = FALSE;

  if (opcode->NumArguments > 0)
  {
    for (ii = 0; ii < opcode->NumArguments; ii++)
    {
      arg = &opcode->Args[ii];

      switch (arg->Type)
      {
      case AT_STACK:
        hasStack = TRUE;
        continue;
      case AT_IMM:
      case AT_SIGNED_IMM:
        hasArgs = TRUE;
        continue;
      }
    }
  }

  if (hasArgs)
  {
    FILE_PRINTF(file, "  Args:\n", opcode->Name);

    for (ii = 0; ii < opcode->NumArguments; ii++)
    {
      arg = &opcode->Args[ii];

      tmpStr[0] = arg->VarName;
      tmpStr[1] = 0;

      switch (arg->Type)
      {
      case AT_STACK:
        continue;
      case AT_IMM:
      case AT_SIGNED_IMM:
        FILE_PRINTF(file, "    arg_%s = %s\n", tmpStr, arg->Name);
        continue;
      }
    }
  }

  if (hasStack)
  {
    FILE_PRINTF(file, "  Stack:\n", opcode->Name);

    for (ii = 0; ii < opcode->NumArguments; ii++)
    {
      arg = &opcode->Args[ii];

      tmpStr[0] = arg->VarName;
      tmpStr[1] = 0;

      switch (arg->Type)
      {
      case AT_STACK:
        FILE_PRINTF(file, "    %ld = %s\n", arg->StackAddress, arg->Name);
        continue;
      case AT_IMM:
      case AT_SIGNED_IMM:
        continue;
      }
    }
  }

  FILE_PRINT(file, "*/\n");
  
  FILE_PRINTF(file, "inline void Op_%s(", opcode->Name);

  if (opcode->NumArguments > 0)
  {
    for (ii = 0; ii < opcode->NumArguments; ii++)
    {
      arg = &opcode->Args[ii];

      tmpStr[0] = arg->VarName;
      tmpStr[1] = 0;

      switch (arg->Type)
      {
        case AT_STACK:
          continue;
        case AT_IMM:
          if (ii > 0)
          {
            FILE_PRINT(file, ", ");
          }
          FILE_PRINTF(file, "PtUnsigned16 arg_%s", tmpStr);
        continue;
        case AT_SIGNED_IMM:
          if (ii > 0)
          {
            FILE_PRINT(file, ", ");
          }
          FILE_PRINTF(file, "PtSigned16 arg_%s", tmpStr);
        continue;
      }
    }
  }
  FILE_PRINT(file, ")\n");
  FILE_PRINT(file, "{\n");

  FILE_PRINT(file, "  ScriptEnforceSpace();\n")
  FILE_PRINT(file, "  ScriptData[ScriptSize++] =  VM_OP_");

  PrintUpperCase(file, opcode->Name);

  if (opcode->NumArguments)
  {
    for (ii = 0; ii < opcode->NumArguments; ii++)
    {
      arg = &opcode->Args[ii];

      tmpStr[0] = arg->VarName;
      tmpStr[1] = 0;

      switch (arg->Type)
      {
        case AT_STACK:
          continue;
        case AT_IMM:
          FILE_PRINTF(file, " | ScriptU16tou10(arg_%s)", tmpStr);
          continue;
        case AT_SIGNED_IMM:
          FILE_PRINTF(file, " | ScriptS16tos10(arg_%s)", tmpStr);
          continue;
      }
    }
  }

  FILE_PRINT(file, ";\n");
  
  FILE_PRINT(file, "}\n\n");

  FILE_PRINT(file, "\n");
}

void WriteCWriterFile()
{
  BPTR file;
  PtSigned32 err;
  PtUnsigned16 ii;

  file = Open(CWRITER_PATH, MODE_NEWFILE);

  if (file == 0)
  {
    err = IoErr();
    PrintFault(err, NULL);
    CLI_PRINTF("Cannot open file %s\n", DOCUMENTATION_PATH);
    return;
  }

  FILE_PRINT(file, "/* This is an automatically generated file */\n\n");

  for (ii = 0; ii < MAX_OPCODES; ii++)
  {
    WriteCWriter(ii, file);
  }

  Flush(file);
  Close(file);
}

void WriteCReader(PtUnsigned16 id, BPTR file)
{
  struct Opcode* opcode;
  struct Argument* arg;
  PtChar tmpStr[2];
  PtChar ch, *code;
  PtUnsigned16 ii;
  PtBool hasStack, hasArgs;

  opcode = &Opcodes[id];

  if (opcode->Name == NULL)
    return;

  FILE_PRINT(file, "    case VM_OP_");
  PrintUpperCase(file, opcode->Name);
  FILE_PRINT(file, ":\n    {\n");

  if (opcode->NumArguments > 0)
  {
    for (ii = 0; ii < opcode->NumArguments; ii++)
    {
      arg = &opcode->Args[ii];

      tmpStr[0] = arg->VarName;
      tmpStr[1] = 0;

      switch (arg->Type)
      {
        case AT_IMM:
          FILE_PRINTF(file, "      PtSigned32 arg_%s = Vm_u10tou16(opcode);\n", tmpStr, arg->Name);
          continue;
        case AT_SIGNED_IMM:
          FILE_PRINTF(file, "      PtUnsigned32 arg_%s = Vm_s10tos16(opcode);\n", tmpStr, arg->Name);
          continue;
      }
    }
  }
  
  code = opcode->Code;
  
  FILE_PRINT(file, "    ");

  while (TRUE)
  {
    ch = *code++;
    if (ch == 0)
      break;
    if (ch == '\n')
    {
      FILE_PRINT(file, "\n    ");
      continue;
    }

    if (ch == '#')
    {
      FILE_PRINT(file, "arg_");
      continue;
    }

    FILE_PRINTCHAR(file, ch);
  }
  
  FILE_PRINT(file, "\n    }\n");
  FILE_PRINT(file, "    return 1;\n");
}

void WriteCReaderFile()
{
  BPTR file;
  PtSigned32 err;
  PtUnsigned16 ii;

  file = Open(CREADER_PATH, MODE_NEWFILE);

  if (file == 0)
  {
    err = IoErr();
    PrintFault(err, NULL);
    CLI_PRINTF("Cannot open file %s\n", DOCUMENTATION_PATH);
    return;
  }

  FILE_PRINT(file, "/* This is an automatically generated file */\n\n");

  FILE_PRINT(file, "PtSigned16 Vm_RunOpcode(OPCODE opcode)\n{\n");

  FILE_PRINT(file, "  switch(opcode & 0x3F)\n  {\n");

  for (ii = 0; ii < MAX_OPCODES; ii++)
  {
    WriteCReader(ii, file);
  }

  FILE_PRINT(file, "  }\n}\n");

  Flush(file);
  Close(file);
}

void WriteCOpcodesHeaderFile()
{
  BPTR file;
  PtSigned32 err;
  PtUnsigned16 ii;
  struct Opcode* opcode;

  file = Open(COPCODES_HEADER_PATH, MODE_NEWFILE);

  if (file == 0)
  {
    err = IoErr();
    PrintFault(err, NULL);
    CLI_PRINTF("Cannot open file %s\n", DOCUMENTATION_PATH);
    return;
  }

  FILE_PRINT(file, "/* This is an automatically generated file */\n\n");

  for (ii = 0; ii < MAX_OPCODES; ii++)
  {
    opcode = &Opcodes[ii];

    if (opcode->Name == NULL)
      continue;

    FILE_PRINT(file, "#define VM_OP_");
    PrintUpperCase(file, opcode->Name);
    FILE_PRINTF(file, " %ld\n", ii);
  }

  FILE_PRINT(file, "\n\nextern const char* OpcodesStr[];\n\n");

  Flush(file);
  Close(file);
}

void WriteCOpcodesSourceFile()
{
  BPTR file;
  PtSigned32 err;
  PtUnsigned16 ii;
  struct Opcode* opcode;

  file = Open(COPCODES_SOURCE_PATH, MODE_NEWFILE);

  if (file == 0)
  {
    err = IoErr();
    PrintFault(err, NULL);
    CLI_PRINTF("Cannot open file %s\n", DOCUMENTATION_PATH);
    return;
  }

  FILE_PRINT(file, "/* This is an automatically generated file */\n\n");

  FILE_PRINT(file, "const char* OpcodesStr[] = {\n");

  for (ii = 0; ii < MAX_OPCODES; ii++)
  {
    opcode = &Opcodes[ii];

    if (opcode->Name == NULL)
      continue;

    FILE_PRINTF(file, "  \"%s\",\n", opcode->Name);
  }

  FILE_PRINT(file, "};\n\n");

  Flush(file);
  Close(file);
}

void ReadOpcode(char* path)
{
  BPTR file;
  PtSigned32 err;
  PtUnsigned16 lineLen;
  PtUnsigned16 section;
  PtBool isTrimming;
  
  file = Open(path, MODE_OLDFILE);

  if (file == 0)
  {
    err = IoErr();
    PrintFault(err, NULL);
    CLI_PRINTF("Cannot open file %s\n", path);
    return;
  }

  LineIsEof = FALSE;
  SectionTextLength = 0;
  FillMem(SectionText, sizeof(SectionText), 0);

  while (LineIsEof == FALSE)
  {
    lineLen = ReadLine(file);

    if (lineLen == 0)
      continue;

    if (lineLen >= 2 && Line[0] == ' ' && Line[1] == ' ')
    {
      if (section == SECTION_NONE)
        continue;


      if (SectionTextLength > 0)
      {
        SectionText[SectionTextLength++] = '\n';
      }
      
      isTrimming = TRUE;

      for (PtUnsigned16 ii = 0; ii < lineLen; ii++)
      {

        if (SectionTextLength >= sizeof(SectionText))
        {
          break;
        }

        if (Line[ii] == 0)
          break;

        if (section != SECTION_CODE && isTrimming && Line[ii] == ' ')
        {
          continue;
        }

        isTrimming = FALSE;


        SectionText[SectionTextLength++] = Line[ii];
      }

      if (SectionTextLength >= sizeof(SectionText))
      {
        CLI_PRINTF("Text is to large for section -> %s\n", (PtChar*)path);
        Close(file);
        return;
      }

      if (section == SECTION_ARGUMENT)
      {
        CommitArgument();
        SectionTextLength = 0;
        FillMem(SectionText, sizeof(SectionText), 0);
      }

      continue;
    }
    else
    {
      if (SectionTextLength > 0)
      {
        CommitOpcodeSection(section);
      }

      SectionTextLength = 0;
      FillMem(SectionText, sizeof(SectionText), 0);

      if (StrStartsWithStr((const char*) Line, "Name:") == TRUE)
      {
        section = SECTION_NAME;
      }
      else if (StrStartsWithStr((const char*) Line, "Description:") == TRUE)
      {
        section = SECTION_DESCRIPTION;
      }
      else if (StrStartsWithStr((const char*) Line, "Argument:") == TRUE)
      {
        section = SECTION_ARGUMENT;
      }
      else if (StrStartsWithStr((const char*) Line, "Code:") == TRUE)
      {
        section = SECTION_CODE;
      }
      else
      {
        section = SECTION_NONE;
        CLI_PRINTF("Unknown Opcode Section -> %s\n", (PtChar*)Line);
        continue;
      }
    }
  }

  if (SectionTextLength > 0)
  {
    CommitOpcodeSection(section);
  }

  Close(file);
}

int main()
{
  PtSigned32 rc;
  PtUnsigned16 ii;

  rc = RETURN_OK;

  SysBase = *(struct ExecBase**)4L;
  DOSBase = (struct DosLibrary*)OpenLibrary("dos.library", 0);

  if (DOSBase == NULL)
  {
    goto CLEAN_EXIT;
  }

  CLI_PRINT("Parrot Opcode Generator\n");
  CLI_PRINTF("Opcode Dir = %s\n", OPCODE_DIR);

  ReadOpcodeDirectory(OPCODE_DIR);

  WriteDocumentationFile();
  WriteCWriterFile();
  WriteCReaderFile();
  WriteCOpcodesHeaderFile();
  WriteCOpcodesSourceFile();

  for (ii = 0; ii < MAX_OPCODES; ii++)
  {
    FreeOpcode(ii);
  }

CLEAN_EXIT:

  if (NULL != DOSBase)
  {
    CloseLibrary((struct Library*)DOSBase);
    DOSBase = NULL;
  }

  return RETURN_OK;
}
