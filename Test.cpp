// Copyright Ben Howenstein 2020. All rights reserved.
//

#pragma once

#include "WinDebugBreakpoint.h"

//int Data[]{ 0, 1, 2 };
char Data[] = { 0, 1, 2, 3, 4 };

int main()
{
	WinDebugBreakpointManager::SetBreakpoint(&Data, WinDebugBreakpointManager::RegisterIndex::FIRST, WinDebugBreakpointManager::DataSize::FOUR_BYTES);
	Data[2] = 0;
	
	WinDebugBreakpointManager::ClearBreakpoint(WinDebugBreakpointManager::RegisterIndex::FIRST);
	Sleep(10);

	Data[2] = 3;

	
	Data[4] = 0;

	return 0;
}
