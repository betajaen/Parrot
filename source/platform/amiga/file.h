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
#include "platform/amiga/timer.h"
#include "platform/amiga/menu.h"


namespace Parrot {

	struct AmigaReadFile {
	private:
		PtrInt Handle;
		Uint32 Position, Length;
	public:
		AmigaReadFile();
		~AmigaReadFile();

		bool OpenFile(ConstCString path);
		void CloseFile();

		bool IsOpen() const {
			return Handle != 0;
		}

		bool IsEof() const {
			return Position >= Length;
		}

		bool GetLength() const {
			return Length;
		}

		Uint32 GetPosition() const {
			return Position;
		}

		void SetPosition(Uint32 Offset);
		void MovePosition(Int32 Offset);

		Byte ReadByte();
		Uint32 ReadBytes(Ptr32 Data, Uint32 Length);
	};

}