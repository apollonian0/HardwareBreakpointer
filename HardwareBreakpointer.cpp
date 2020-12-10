// Copyright Ben Howenstein 2020. All rights reserved.
//

#pragma once

#include <windows.h>
#include <cassert>

template<class Value_t> Value_t& SetBit(Value_t& value, const int bit)
{
	return value |= (static_cast<Value_t>(1) << bit);
}

HANDLE MainThreadHandle;
HANDLE EventHandle;
int Data[] = { 0, 1, 2};

DWORD WINAPI SetBreakpoint(LPVOID const pParameter)
{
    auto suspendCount = SuspendThread(MainThreadHandle);
    assert(suspendCount >= 0);

	CONTEXT context;
	context.ContextFlags = CONTEXT_DEBUG_REGISTERS;

	auto success = GetThreadContext(MainThreadHandle, &context);
	assert(success);

	context.Dr0 = (DWORD_PTR)(Data + 2);
	context.Dr7 = 1;
	SetBit(context.Dr7, 16);
	SetBit(context.Dr7, 18);
	SetBit(context.Dr7, 19);
	
	success = SetThreadContext(MainThreadHandle, &context);
	assert(success);

	suspendCount = ResumeThread(MainThreadHandle);
	assert(suspendCount >= 0);

	SetEvent(EventHandle);

    return 0;
}

int main()
{
	auto success = DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &MainThreadHandle, 0, false, DUPLICATE_SAME_ACCESS);
	assert(success);

	EventHandle = CreateEvent(nullptr, true, false, TEXT("TestEvent"));
    assert(EventHandle);

    HANDLE testThreadHandle = CreateThread(nullptr, 0, SetBreakpoint, nullptr, 0, 0);
    assert(testThreadHandle);
    WaitForSingleObject(EventHandle, INFINITE);
	CloseHandle(testThreadHandle);

	Data[2] = 2;

	return 0;
}
