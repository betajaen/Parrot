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

#include "platform/shared/required.h"
#include "platform/shared/string.h"
#include "platform/shared/log.h"

#include "maniac/context.h"

namespace Parrot
{
	ManiacContext* Ctx;

	void ManiacContext::Run() {	
		
		PMenu<21> Menu;
		Menu.Title("Maniac");
		Menu.Item("Load");
		Menu.Item("Save");
		Menu.Item("Restart");
		Menu.Item("Quit");
		Menu.Title("Verb");	
		Menu.Item("Push");
		Menu.Item("Pull");
		Menu.Item("Give");
		Menu.Item("Open");
		Menu.Item("Close");
		Menu.Item("Read");
		Menu.Item("Walk to");
		Menu.Item("Pick up");
		Menu.Item("What is");
		Menu.Item("New kid");
		Menu.Item("Unlock");
		Menu.Item("Use");
		Menu.Item("Turn on");
		Menu.Item("Turn off");
		Menu.Item("Fix");

		Screen.CreateScreen(320, 200, 4, true, "Parrot");
		Screen.CreateWindow(320, 200, true, nullptr);
		Screen.SetMenu(Menu.GetData());
		Screen.StartListening(25);

		Screen.DestroyWindow();
		Screen.DestroyScreen();
	}


	void OnMenuEvent(Uint16 menuNum, Uint16 menuItem, Uint16 subMenuItem) {

		PARROT_DEBUG_FMT("Menu %ld, %ld, %ld", (Uint32) menuNum, (Uint32) menuItem, (Uint32) subMenuItem);

		if (menuNum == 0 && menuItem == 3) {
			Ctx->Screen.StopListening();
		}
	}

	int Main(int param) {	
		ManiacContext context;
		Ctx = &context;
		Ctx->Run();

		return 0;
	}
}
