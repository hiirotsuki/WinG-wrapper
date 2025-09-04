/* SPDX-License-Identifier: GPL-2.0-or-later */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

unsigned char WINAPI SimulateVgaStatus()
{
	SYSTEMTIME st;
	GetSystemTime(&st);
	/* approximate vblank by toggling bit 3 during the first 1.5ms of a 16.6ms frame */
	unsigned int micros = (st.wMilliseconds % 17) * 1000;
	return (micros < 1500) ? 0x08 : 0x00;
}

LONG WINAPI VehHandler(EXCEPTION_POINTERS *ep)
{
	unsigned char *ip = (unsigned char *)ep->ContextRecord->Eip;
	unsigned char opcode = *ip;

	if(ep->ExceptionRecord->ExceptionCode == EXCEPTION_PRIV_INSTRUCTION)
	{
		switch(opcode)
		{
			case 0xEC: /* IN AL, DX */
			{
				unsigned char val = SimulateVgaStatus(); /* kinda sorta emulate VGA register, lol... */
				ep->ContextRecord->Eax = (ep->ContextRecord->Eax & 0xFFFFFF00) | val;
				ep->ContextRecord->Eip += 1;
				return EXCEPTION_CONTINUE_EXECUTION;
			}
			case 0xEE: /* OUT DX, AL */
			{
				ep->ContextRecord->Eip += 1;
				return EXCEPTION_CONTINUE_EXECUTION;
			}
		}
	}

	return EXCEPTION_CONTINUE_SEARCH;
}

BOOL WINAPI DllMainCRTStartup(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch(fdwReason)
	{
		case DLL_PROCESS_ATTACH:
		{
			AddVectoredExceptionHandler(1, VehHandler);
			break;
		}

		case DLL_PROCESS_DETACH:
		{
			RemoveVectoredExceptionHandler(VehHandler);
			break;
		}
	}
	return TRUE;
}
