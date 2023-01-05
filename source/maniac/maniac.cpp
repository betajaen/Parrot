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

#include "required.h"
#include "screen.h"

namespace Parrot
{
	int Main(int param)
	{
		Screen screen;
		screen.CreateScreen(320, 200, 4, true, "Parrot");
		screen.CreateWindow(320, 200, true, nullptr);

		return 0;
	}
}
