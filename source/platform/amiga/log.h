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

#include "platform/shared/required.h"

namespace Parrot {

	void PrintFmt(ConstCString Fmt, ...);
	void PrintStr(ConstCString Str);
	void PrintLogFmt(Uint16 LogLevel, ConstCString Fmt, ...);
	void PrintLogStr(Uint16 LogLevel, ConstCString Str);

	void ParrotStop(ConstCString Str);

}

#define PARROT_PRINT_FMT(FMT, ...) ::Parrot::PrintFmt(FMT, __VA_ARGS__)
#define PARROT_PRINT_STR(STR) ::Parrot::PrintStr(STR)

#if defined(PARROT_LOG) == false
#define PARROT_LOG 1
#endif

#if PARROT_LOG != 0 && PARROT_LOG <= 3
#define PARROT_INFO_FMT(FMT, ...) ::Parrot::PrintLogFmt(3, FMT, __VA_ARGS__)
#define PARROT_INFO_STR(STR) ::Parrot::PrintLogStr(3, STR)
#else
#define PARROT_INFO_FMT(FMT, ...)
#define PARROT_INFO_STR(STR)
#endif


#if PARROT_LOG != 0 && PARROT_LOG <= 2
#define PARROT_WARN_FMT(FMT, ...) ::Parrot::PrintLogFmt(2, FMT, __VA_ARGS__)
#define PARROT_WARN_STR(STR) ::Parrot::PrintLogStr(2, STR)
#else
#define PARROT_WARN_FMT(FMT, ...)
#define PARROT_WARN_STR(STR)
#endif

#if PARROT_LOG != 0 && PARROT_LOG <= 1
#define PARROT_ERROR_FMT(FMT, ...) ::Parrot::PrintLogFmt(1, FMT, __VA_ARGS__)
#define PARROT_ERROR_STR(STR) ::Parrot::PrintLogStr(1, STR)
#else
#define PARROT_ERROR_FMT(FMT, ...)
#define PARROT_ERROR_STR(STR)
#endif

#if PARROT_LOG != 0 && defined(PARROT_DEBUG)
#define PARROT_DEBUG_FMT(FMT, ...) ::Parrot::PrintLogFmt(0, FMT, __VA_ARGS__)
#define PARROT_DEBUG_STR(STR) ::Parrot::PrintLogStr(0, STR)
#else
#define PARROT_DEBUG_FMT(FMT, ...)
#define PARROT_DEBUG_STR(STR)
#endif


#define PARROT_STOP(STR) ::Parrot::ParrotStop(STR);