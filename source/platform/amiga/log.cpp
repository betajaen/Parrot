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

#include "platform/amiga/types.h"
#include "platform/amiga/sdi/SDI_stdarg.h"

namespace Parrot { 

	typedef VOID(*CharacterProcessor)();

	static const Uint32 PutChar = 0x16c04e75;
	static const Uint32 LenChar = 0x52934e75;

	void PrintFmt(ConstCString Fmt, ...) {
		static char buf[1024];
		
		Uint32 length = 0;
		VA_LIST args;

		if (Fmt == NULL) {
			return;
		}

		VA_START(args, Fmt);
		
		RawDoFmt((CONST_STRPTR)Fmt, static_cast<Ptr32>(args), (CharacterProcessor)&LenChar, &length);
		
		if (length >= 1024) {
			VA_END(args);
			return;
		}
		
		RawDoFmt((CONST_STRPTR)Fmt, static_cast<Ptr32>(args), (CharacterProcessor)&PutChar, buf);
		buf[length] = 0;

		PutStr(buf);
		PutStr("\n");

		VA_END(args);

	}

	void PrintStr(ConstCString Str) {
		PutStr(Str);
		PutStr("\n");
	}

	void PrintLogFmt(Uint16 LogLevel, ConstCString Fmt, ...) {
		
		static char buf[1024];
		
		Uint32 length = 0;
		VA_LIST args;

		if (Fmt == NULL) {
			return;
		}

		VA_START(args, Fmt);
		
		RawDoFmt((CONST_STRPTR)Fmt, static_cast<Ptr32>(args), (CharacterProcessor)&LenChar, &length);
		
		if (length >= 1024) {
			VA_END(args);
			return;
		}
		
		RawDoFmt((CONST_STRPTR)Fmt, static_cast<Ptr32>(args), (CharacterProcessor)&PutChar, buf);
		buf[length] = 0;

		switch (LogLevel)
		{
			case 0: PutStr("D "); break;
			case 1: PutStr("E "); break;
			case 2: PutStr("W "); break;
			case 3: PutStr("I "); break;
			default: break;
		}

		PutStr(buf);
		PutStr("\n");

		VA_END(args);
	}

	void PrintLogStr(Uint16 LogLevel, ConstCString Str) {

		switch (LogLevel)
		{
			case 0: PutStr("D "); break;
			case 1: PutStr("E "); break;
			case 2: PutStr("W "); break;
			case 3: PutStr("I "); break;
			default: break;
		}

		PutStr(Str);
		PutStr("\n");

	}

}