// Copyright Ben Howenstein 2020. All rights reserved.
//

#pragma once

#include <windows.h>
#include <memory>
#include <assert.h>

class WinDebugBreakpointManager
{
public:

	template<class Value_t>
	static void SetBreakpoint(const Value_t* const address, const int index)
	{
		HANDLE threadHandle = 0;
		auto success = DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &threadHandle, 0, false, DUPLICATE_SAME_ACCESS);
		assert(success);

		auto eventHandle = CreateEvent(nullptr, true, false, TEXT("DebugBreakpoint"));
		assert(eventHandle);

		const auto pBreakpointInfo = std::unique_ptr<BreakpointInfo>(new BreakpointInfo{
			reinterpret_cast<DWORD_PTR>(address), threadHandle, eventHandle, index, false });

		HANDLE setBreakPointThread = CreateThread(nullptr, 0, SetBreakpointOnThread, reinterpret_cast<LPVOID>(&*pBreakpointInfo), 0, 0);
		assert(setBreakPointThread);

		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(setBreakPointThread);
	}

private:

	template<class Value_t>
	static Value_t& SetBit(Value_t& value, const int bit)
	{
		return value |= (static_cast<Value_t>(1) << bit);
	};

	static DWORD WINAPI SetBreakpointOnThread(LPVOID const pParameter)
	{
		const auto& breakpointInfo = *reinterpret_cast<BreakpointInfo*>(pParameter);

		auto suspendCount = SuspendThread(breakpointInfo.m_threadHandle);
		assert(suspendCount >= 0);

		CONTEXT context;
		context.ContextFlags = CONTEXT_DEBUG_REGISTERS;

		auto success = GetThreadContext(breakpointInfo.m_threadHandle, &context);
		assert(success);

		context.Dr0 = breakpointInfo.m_pVariableAddress;
		context.Dr7 = 1;
		SetBit(context.Dr7, 16);
		SetBit(context.Dr7, 18);
		SetBit(context.Dr7, 19);

		success = SetThreadContext(breakpointInfo.m_threadHandle, &context);
		assert(success);

		suspendCount = ResumeThread(breakpointInfo.m_threadHandle);
		assert(suspendCount >= 0);

		SetEvent(breakpointInfo.m_eventHandle);

		return 0;
	}

	struct BreakpointInfo
	{
		DWORD_PTR m_pVariableAddress{ 0 };
		HANDLE m_threadHandle{ 0 };
		HANDLE m_eventHandle{ 0};
		int  m_registerIndex{ 0 };
		bool m_breakOnRead{ false };
	};
};
