/*
	Parrot
	Copyright (C) 2023 Robin Southern https://github.com/betajaen/parrot

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>

#include <dos/dos.h>
#include <workbench/workbench.h>
#include <inline/exec.h>
#include <inline/dos.h>

#include "types.h"

#ifndef PARROT_MIN_STACK_SIZE
#define PARROT_MIN_STACK_SIZE 65536U
#endif

int __nocommandline = 1;
int __initlibraries = 0;
extern char * __commandline;
struct DosLibrary* DOSBase = NULL;
struct IntuitionBase* IntuitionBase = NULL;
extern struct WBStartup* _WBenchMsg;
int Parrot_StartedFromCli = 0;

namespace Parrot {
	int Main(int param);
	Uint32 CStringFmt(CString str, Uint32 strCapacity, ConstCString fmt, ...) ;
}

int main(void) {

	int param;

	if ((DOSBase = (struct DosLibrary*)OpenLibrary("dos.library", 33)) == NULL) {
		return RETURN_FAIL;
	}
	
	struct Task* thisTask = FindTask(NULL);
	ULONG currentStack=(ULONG) thisTask->tc_SPUpper-(ULONG)thisTask->tc_SPLower;

	if (currentStack < PARROT_MIN_STACK_SIZE) {

		if (_WBenchMsg) {
			if ((IntuitionBase = (struct IntuitionBase*)OpenLibrary("intuition.library", 33)) != NULL) {
				struct EasyStruct str;
				str.es_StructSize = sizeof(struct EasyStruct);
				str.es_Flags = 0;
				str.es_GadgetFormat = (UBYTE*)"OK";
				str.es_TextFormat = (UBYTE*)"Not enough stack space!\n\n%ld bytes given.\n\nPlease increase it to at least %ld bytes\nin the Workbench Information Window.";
				str.es_Title = (UBYTE*)"Parrot";

				EasyRequest(NULL, &str, NULL, currentStack, PARROT_MIN_STACK_SIZE);
				CloseLibrary((struct Library*)IntuitionBase);
			}
		}
		else {
			Printf("Not enough stack space!\n\n%ld bytes given.\n\nPlease run the command \"stack %lu\" before running this program from the CLI.\n", currentStack, PARROT_MIN_STACK_SIZE);
		}

		CloseLibrary((struct Library*)DOSBase);

		return RETURN_FAIL;
	}

	if ((IntuitionBase = (struct IntuitionBase*)OpenLibrary("intuition.library", 33)) == NULL) {
		CloseLibrary((struct Library*)DOSBase);
		return RETURN_FAIL;
	}

	param = Parrot::Main(param);

	if (_WBenchMsg) {
		struct EasyStruct str;
		str.es_StructSize = sizeof(struct EasyStruct);
		str.es_Flags = 0;
		str.es_GadgetFormat = (UBYTE*)"OK";
		str.es_TextFormat = (UBYTE*)"Thanks for playing!";
		str.es_Title = (UBYTE*)"Parrot";

		EasyRequest(NULL, &str, NULL);
	}


	CloseLibrary((struct Library*)IntuitionBase);
	CloseLibrary((struct Library*)DOSBase);

	return RETURN_OK;
}

namespace Parrot { 

	void Assert(ConstCString file, Uint32 line)
	{
		static char assert_text[1024] = { 0 };
		BPTR handle;

		CStringFmt(&assert_text[0], sizeof(assert_text), "Assertion!\n%s:%ld", file, line);

		if (Parrot_StartedFromCli) {
			// Printf((CONST_STRPTR) "Assertion!\n%s:%ld", (ULONG) file, line);
			Printf(&assert_text[0]);
		}
		else {

			struct EasyStruct str;
			str.es_StructSize = sizeof(struct EasyStruct);
			str.es_Flags = 0;
			str.es_GadgetFormat = (UBYTE*)"OK";
			str.es_TextFormat = (UBYTE*) &assert_text[0];
			str.es_Title = (UBYTE*)"Parrot";

			EasyRequest(NULL, &str, NULL);

		}

		handle = Open("RAM:Parrot.txt", MODE_NEWFILE);
		
		if (handle != 0) {
			FPuts(handle, (CONST_STRPTR) &assert_text[0]);
			Close(handle);
		}

		Exit(1);
	}

}