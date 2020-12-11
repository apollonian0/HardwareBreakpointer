// Copyright Ben Howenstein 2020. All rights reserved.
//

#pragma once

#include "WinDebugBreakpoint.h"

//int Data[]{ 0, 1, 2 };
char Data[] = { 0, 1, 2, 3, 4 };

int main()
{
	WinDebugBreakpointManager::SetBreakpoint(&Data, WinDebugBreakpointManager::RegisterIndex::FIRST, WinDebugBreakpointManager::DataSize::TWO_BYTES);
	Data[0] = 5;
	Data[1] = 3;
	Data[2] = 4;

	WinDebugBreakpointManager::ClearBreakpoint(WinDebugBreakpointManager::RegisterIndex::FIRST);
	Data[2] = 3;
	Data[0] = 6;

	return 0;
}
