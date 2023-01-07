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

#include "platform/amiga/file.h"
#include "platform/shared/log.h"

#include <proto/dos.h>

namespace Parrot {


	AmigaReadFile::AmigaReadFile() {
		Handle = 0;
		Position = 0;
		Length = 0;
	}

	AmigaReadFile::~AmigaReadFile() {
		if (Handle != 0) {
			CloseFile();
		}
	}

	bool AmigaReadFile::OpenFile(ConstCString Path) {

		if (Handle != 0) {
			CloseFile();
		}

		Handle = Open(Path, MODE_OLDFILE);

		if (Handle == 0) {
			PARROT_WARN_FMT("Did not open file %s", Path);
			return false;
		}

		
		Seek(Handle, 0, OFFSET_END);
		Length = Seek(Handle, 0, OFFSET_BEGINNING);
		Position = 0;

		return true;
	}

	void AmigaReadFile::CloseFile() {
		if (Handle != 0) {
			Close(Handle);
			Position = 0;
			Length = 0;
		}
	}


	void AmigaReadFile::SetPosition(Uint32 Offset) {
		Seek(Handle, Offset, OFFSET_BEGINNING);
		Position = Seek(Handle, 0, OFFSET_CURRENT);
	}

	void AmigaReadFile::MovePosition(Int32 Offset) {
		Seek(Handle, Offset, OFFSET_CURRENT);
		Position = Seek(Handle, 0, OFFSET_CURRENT);
	}

	Byte AmigaReadFile::ReadByte() {	
		Byte val;
		Position += (Uint32) Read(Handle, &val, sizeof(val));
		return val;
	}

	Uint32 AmigaReadFile::ReadBytes(Ptr32 Data, Uint32 Length) {
		
		Uint32 bytesRead = 0;
		bytesRead = (Uint32) Read(Handle, Data, Length);
		Position += bytesRead;
		return bytesRead;
	}


}