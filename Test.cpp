// Copyright Ben Howenstein 2020. All rights reserved.
//

#pragma once

#include <array>
#include "WinDebugBreakpoint.h"

auto Data = std::array<char, 16>{};

int main()
{
	WinDebugBreakpointManager::SetBreakpoint(&Data[0], WinDebugBreakpointManager::RegisterIndex::FIRST, WinDebugBreakpointManager::DataSize::ONE_BYTE);

	WinDebugBreakpointManager::SetBreakpoint(&Data[2], WinDebugBreakpointManager::RegisterIndex::SECOND, WinDebugBreakpointManager::DataSize::TWO_BYTES);

	WinDebugBreakpointManager::SetBreakpoint(&Data[4], WinDebugBreakpointManager::RegisterIndex::THIRD, WinDebugBreakpointManager::DataSize::FOUR_BYTES);

	WinDebugBreakpointManager::SetBreakpoint(&Data[8], WinDebugBreakpointManager::RegisterIndex::FOURTH, WinDebugBreakpointManager::DataSize::EIGHT_BYTES);

	Data[0] = 1;
	Data[2] = 1;
	Data[5] = 1;
	Data[14] = 1;

	WinDebugBreakpointManager::ClearBreakpoint(WinDebugBreakpointManager::RegisterIndex::FIRST);
	Data[0] = 1;

	WinDebugBreakpointManager::ClearBreakpoint(WinDebugBreakpointManager::RegisterIndex::SECOND);
	Data[2] = 1;

	WinDebugBreakpointManager::ClearBreakpoint(WinDebugBreakpointManager::RegisterIndex::THIRD);
	Data[5] = 1;

	WinDebugBreakpointManager::ClearBreakpoint(WinDebugBreakpointManager::RegisterIndex::FOURTH);
	Data[15] = 1;
	
	return 0;
}
