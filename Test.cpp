// Copyright Ben Howenstein 2020. All rights reserved.
//

#pragma once

#include "WinDebugBreakpoint.h"

int Data[]{ 0, 1, 2 };

int main()
{
	WinDebugBreakpointManager::SetBreakpoint(Data + 1, 0);
	Data[1] = 2;

	return 0;
}
