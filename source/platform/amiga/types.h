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

#pragma once

#include <exec/types.h>

namespace Parrot {

    typedef LONG			Int32;
    typedef ULONG			Uint32;
    typedef WORD			Int16;
    typedef UWORD			Uint16;
    typedef BYTE			Int8;
    typedef UBYTE			Uint8;
    typedef APTR			Ptr32;
    typedef ULONG			Size;
    typedef STRPTR			CString;
    typedef CONST_STRPTR	ConstCString;
    typedef TEXT			Char;
}