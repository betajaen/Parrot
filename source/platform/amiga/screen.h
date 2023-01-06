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

struct Screen;
struct Window;
struct ScreenBuffer;
struct RastPort;
struct Menu;

namespace Parrot {

	struct AmigaScreen final
	{
	private:

		::Screen* Screen;
		struct Window* Window;
		struct ScreenBuffer* Buffer;
		struct RastPort* RastPort;
		Ptr32 ScreenVisualInfo;
		bool StopEvents;
		AmigaTimer Timer;
		struct Menu* Menu;

	public:

		AmigaScreen();
		~AmigaScreen();

		bool CreateScreen(Uint32 w, Uint32 h, Uint8 depth, bool tryRTG = true, ConstCString title = nullptr);
		bool CreateWindow(Uint32 w, Uint32 h, bool borderless, ConstCString title = nullptr);
		bool StartListening(Uint32 framesPerSecond);
		void SetMenu(Ptr32 menuData);

		void StopListening();
		void DestroyWindow();
		void DestroyScreen();


	};

}