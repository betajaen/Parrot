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

#include "maniac/lfl.h"
#include "platform/shared/string.h"

namespace Parrot
{
	Lfl::Lfl() {
		this->Num = 0xFFFF;
	}

	Lfl::~Lfl() {
		File.CloseFile();
	}

	bool Lfl::Open(Uint16 num) {
		static char path[255];

		Num = num;

		CStringFmt(path, sizeof(path), "PROGDIR:Data/Rooms/%02d.LFL", Num);
		
		return File.OpenFile(path);	
	}

	void Lfl::Close() {
		File.CloseFile();
	}

	Uint32 Lfl::GetLength() const {
		return File.GetLength();
	}

	Uint32 Lfl::GetPosition() const {
		return File.GetPosition();
	}

	void Lfl::SetPosition(Uint32 Offset) {
		File.SetPosition(Offset);
	}

	void Lfl::MovePosition(Uint32 Offset) {
		File.MovePosition(Offset);
	}

	bool Lfl::IsEof() const {
		return File.IsEof();
	}

	Byte Lfl::ReadByte() {
		Byte b = File.ReadByte();
		return b ^ 0xFF;
	}

	Uint32 Lfl::ReadBytes(Byte* Data, Uint32 Length) {
		Uint32 readLength = File.ReadBytes(Data, Length);
		for (Uint32 i = 0; i < readLength; i++) {
			Data[i] = Data[i] ^ 0xFF;
		}
		return readLength;
	}

	
	Uint16 Lfl::ReadUint16LE() {
		Uint8 lo = File.ReadByte() ^ 0xFF;
		Uint8 hi = File.ReadByte() ^ 0xFF;

		return hi << 8 | lo;
	}

}
