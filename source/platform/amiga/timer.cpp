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

#include "timer.h"

#include <exec/types.h>
#include <exec/ports.h>

#include <proto/timer.h>
#include <inline/timer.h>

struct Device* TimerBase;

#include <clib/exec_protos.h>
#include <clib/alib_protos.h>

namespace Parrot
{
	struct timeval _time0;

	void InitTimer() {
		GetSysTime(&_time0);
	}

    Int32 getMSec() {

		struct timeval time1;
		GetSysTime(&time1);
		SubTime(&time1, &_time0);

		Uint32 msec = 0;

		if (time1.tv_sec > 0) {
			msec = time1.tv_sec * 1000;
		}

		msec += time1.tv_micro / 1000;

		return msec;

    }


	AmigaTimer::AmigaTimer() {
		MsgPort = nullptr;
		TimerReq = nullptr;
		SignalBit = 0;
	}

	AmigaTimer::~AmigaTimer() {
		Destroy();
	}
	
	bool AmigaTimer::Create() {

		if (MsgPort != nullptr) {
			Destroy();
		}

		MsgPort = CreatePort(NULL, 0);

		if (MsgPort == NULL) {
			PARROT_ERROR("Could not open Timer message port");
			return false;
		}

		TimerReq = (struct timerequest*) CreateExtIO(MsgPort, sizeof(struct timerequest));

		if (TimerReq == NULL) {
			DeletePort(MsgPort);
			MsgPort = NULL;
			PARROT_ERROR("Could not open Timer TimerReq");
			return false;
		}

		if (OpenDevice(TIMERNAME, UNIT_MICROHZ, (struct IORequest*) TimerReq, 0) != 0) {
			DeleteExtIO((struct IORequest*) TimerReq);
			TimerReq = NULL;
			DeletePort(MsgPort);
			MsgPort = NULL;
			return false;
		}

		TimerBase = (struct Device*) TimerReq->tr_node.io_Device;

		return true;
	}

	void AmigaTimer::Destroy() {
		if (TimerReq != nullptr) {
			AbortIO((struct IORequest*) TimerReq);
			WaitIO((struct IORequest*) TimerReq);

			CloseDevice((struct IORequest*) TimerReq);
			DeleteExtIO((struct IORequest*) TimerReq);
			TimerReq = nullptr;
		}
		
		if (MsgPort != nullptr) {
			DeletePort(MsgPort);
			MsgPort = NULL;
		}
	}

	Uint32 AmigaTimer::Start(Uint32 microSeconds) {
		TimerReq->tr_time.tv_secs = 0;
		TimerReq->tr_time.tv_micro = microSeconds;
		TimerReq->tr_node.io_Command = TR_ADDREQUEST;
		SendIO(&TimerReq->tr_node);

		return 1 << MsgPort->mp_SigBit;
	}

	bool AmigaTimer::IsReady() const {
		WaitPort(MsgPort);
		GetMsg(MsgPort);

		return true;
	}

	Uint32 AmigaTimer::GetSignalBit() {
		if (MsgPort != NULL) {
			return 1 << MsgPort->mp_SigBit;
		}
		else {
			return 0;
		}
	}

}