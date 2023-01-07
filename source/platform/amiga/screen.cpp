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

#include "platform/amiga/screen.h"

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/cybergraphics.h>
#include <proto/graphics.h>
#include <proto/datatypes.h>

#include <intuition/intuition.h>
#include <intuition/screens.h>
#include <cybergraphx/cybergraphics.h>

#include <datatypes/datatypes.h>
#include <datatypes/datatypesclass.h>
#include <datatypes/pictureclass.h>
#include <graphics/gfx.h>
#include <graphics/scale.h>
#include <graphics/displayinfo.h>
#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>

#include <clib/gadtools_protos.h>

#include "platform/shared/events.h"
#include "platform/shared/string.h"
#include "platform/shared/log.h"

extern struct Library* CyberGfxBase;

namespace Parrot {

#define PARROT_NO_MODE 0xFFFFFFFE;

	void InitTimer();

	static Opt<ULONG> FindRTGModeId(Uint32 width, Uint32 height, Uint8 depth)
	{
		if (CyberGfxBase == NULL) {
			if ((CyberGfxBase  = (struct Library*)OpenLibrary("cybergraphics.library", 41)) == NULL) {
				PARROT_ERROR("Could not open CyberGraphics library!!\n");
				return Opt<ULONG>();
			}
		}
		
		ULONG modeId = BestCModeIDTags(
			CYBRBIDTG_NominalWidth, width,
			CYBRBIDTG_NominalHeight, height,
			CYBRBIDTG_Depth, depth,
			TAG_DONE
		);

		if (modeId == 0) {
			return Opt<ULONG>();
		}

		return Opt<ULONG>(modeId);
	}

	static ULONG FindModeId(Uint32 width, Uint32 height, Uint8 depth)
	{
		ULONG modeId = BestModeID(
			BIDTAG_NominalWidth, width, 
			BIDTAG_NominalHeight, height, 
			BIDTAG_Depth, depth
		);

		if (modeId == 0) {
			return Opt<ULONG>();
		}

		return Opt<ULONG>(modeId);
	}

	AmigaScreen::AmigaScreen() {
		Screen = nullptr;
		Window = nullptr;
		Buffer = nullptr;
		RastPort = nullptr;
		ScreenVisualInfo = nullptr;
		Menu = nullptr;
	}

	AmigaScreen::~AmigaScreen() {
		// Timer.Destroy();
		DestroyWindow();
		DestroyScreen();
	}

	bool AmigaScreen::CreateScreen(Uint32 width, Uint32 height, Uint8 depth, bool tryRTG, ConstCString title) {

		Opt<ULONG> modeId;

		if (tryRTG) {
			modeId = FindRTGModeId(width, height, depth);

			if (modeId == false) {
				PARROT_ERROR("Could not find appropriate RTG screen mode");
				return false;
			}
		}
		else {
			modeId = FindModeId(width, height, depth);
			
			if (modeId == false) {
				PARROT_ERROR("Could not find appropriate screen mode");
				return false;
			}
		}

		TagList<16> tags;
		tags.Value(SA_DisplayID, modeId.Value);
		tags.Value(SA_Left, 0);
		tags.Value(SA_Top, 0);
		tags.Value(SA_Width, width);
		tags.Value(SA_Height, height);
		tags.Value(SA_Depth, depth);
		if (title != nullptr) {
			tags.Ptr(SA_Title, title);
		}
		tags.Value(SA_ShowTitle, TRUE);
		tags.Value(SA_Type, CUSTOMSCREEN);
		tags.Value(SA_FullPalette, TRUE);
		tags.Value(SA_Colors32, TRUE);
		tags.Value(SA_Exclusive, TRUE);
		tags.Value(SA_AutoScroll, FALSE);
		tags.Done();

		Screen = OpenScreenTagList(NULL, tags.GetTags());

		if (Screen == nullptr) {
			return false;
		}

		Buffer = AllocScreenBuffer(
			Screen,
			nullptr,
			SB_SCREEN_BITMAP
		);

		if (Buffer == nullptr) {
			CloseScreen(Screen);
			Screen = nullptr;
			return false;
		}

		return true;
	}

	bool AmigaScreen::CreateWindow(Uint32 width, Uint32 height, bool borderless, ConstCString title) {
		
		TagList<16> tags;
		tags.Value(WA_Left, 0);
		tags.Value(WA_Top, 0);
		tags.Value(WA_Width, width);
		tags.Value(WA_Height, height);

		if (Screen != nullptr) {
			tags.Ptr(WA_CustomScreen, Screen);
		}

		if (borderless) {		
			tags.Value(WA_Backdrop, TRUE);
			tags.Value(WA_Borderless, TRUE);
			tags.Value(WA_DragBar, FALSE);
			tags.Value(WA_CloseGadget, FALSE);
			tags.Value(WA_DepthGadget, FALSE);
		}

		tags.Value(WA_Activate, TRUE);
		tags.Value(WA_SimpleRefresh, TRUE);
		tags.Value(WA_IDCMP, IDCMP_CLOSEWINDOW | IDCMP_VANILLAKEY | IDCMP_IDCMPUPDATE | IDCMP_MOUSEBUTTONS | IDCMP_MENUPICK);
		tags.Done();

		Window = OpenWindowTagList(NULL, tags.GetTags());

		if (Window != nullptr)
		{
			ScreenVisualInfo = GetVisualInfo(Window->WScreen, TAG_END);
		}

		return Window != nullptr;
	}

	bool AmigaScreen::StartListening(Uint32 framesPerSecond) {
		Uint32 waitTime = (1000000 / framesPerSecond);
		StopEvents = false;

		Timer.Create();
		InitTimer();

		Uint32 timerBit = Timer.Start(waitTime);
		Uint32 windowBit = (1 << Window->UserPort->mp_SigBit);
		Uint32 signalBits = windowBit | timerBit | SIGBREAKF_CTRL_C;
		
		Uint16 idcmpCode;
		struct MenuItem* menuItem;

		while (StopEvents == false) {
			
			ULONG signal = Wait(signalBits);

			if (signal & SIGBREAKF_CTRL_C) {
				StopEvents = true;
				break;
			}
			
			if (signal & windowBit) {
				
				struct IntuiMessage* msg;
				while (true)
				{
					msg = (struct IntuiMessage*)GetMsg(Window->UserPort);

					if (msg == NULL)
						break;

					struct TagItem* tstate, * tag, * tags;
					ULONG tiData;

					switch (msg->Class)
					{
						case IDCMP_CLOSEWINDOW: {
							StopEvents = true;
						}
						break;
						case IDCMP_MENUPICK: {
							idcmpCode = msg->Code;
							while ((idcmpCode != MENUNULL)) {
								menuItem = ItemAddress(Menu, idcmpCode);

								OnMenuEvent(
										MENUNUM(idcmpCode),
										ITEMNUM(idcmpCode),
										SUBNUM(idcmpCode)									
								);

								idcmpCode = menuItem->NextSelect;
							}
						}
						break;
					}

					ReplyMsg((struct Message*)msg);
				}

			}

			if (signal & timerBit) {

				if (Timer.IsReady()) {
					// Frame here.

					Timer.Start(waitTime);
				}

			}

		}

	}

	void AmigaScreen::DestroyWindow() {

		if (Menu != nullptr)
		{
			FreeMenus(Menu);
			Menu = nullptr;
		}

		if (ScreenVisualInfo != nullptr)
		{
			FreeVisualInfo(ScreenVisualInfo);
			ScreenVisualInfo = nullptr;
		}

		if (Window != nullptr) {
			CloseWindow(Window);
			Window = nullptr;
		}
	}

	void AmigaScreen::DestroyScreen() {
		if (Buffer != nullptr) {
			FreeScreenBuffer(Screen, Buffer);
			Buffer = nullptr;
		}

		if (Screen != nullptr) {
			CloseScreen(Screen);
			Screen = nullptr;
		}
	}

	void AmigaScreen::StopListening() {
		StopEvents = true;
		PARROT_INFO_STR("Stopping event loop by request");
	}

	void AmigaScreen::SetMenu(Ptr32 data) {
		struct NewMenu* menuData = (struct NewMenu*) data;
		Menu = CreateMenus(menuData, TAG_END);
		LayoutMenus(Menu, ScreenVisualInfo, TAG_END);
		SetMenuStrip(Window, Menu);
	}

}