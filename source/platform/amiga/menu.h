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
	

	template<Uint16 Count>
	struct AmigaMenu {
		
		struct MenuItem {
			Uint8		Type;
			ConstCString		Label;
			ConstCString		CommandKey;
			Uint16		Flags;
			Uint32		MutualExclude;
			Ptr32		UserData;
		};

		MenuItem Menus[Count + 1] = { 0 };
		Uint16 It = 0;

		AmigaMenu() = default;
		~AmigaMenu() = default;

		AmigaMenu& Title(ConstCString title) {
			PARROT_ASSERT(It < Count);
			MenuItem& menu = Menus[It++];
			menu.Type = 1;
			menu.Label = title;
			return *this;
		}

		AmigaMenu& Item(ConstCString label, ConstCString commandKey = nullptr) {
			PARROT_ASSERT(It < Count);
			MenuItem& menu = Menus[It++];
			menu.Type = 2;
			menu.Label = label;
			menu.CommandKey = commandKey;
			return *this;
		}

		Ptr32 GetData() const {
			return (Ptr32) &Menus[0];
		}

	};

}