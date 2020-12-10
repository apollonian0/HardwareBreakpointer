// Copyright Ben Howenstein 2020. All rights reserved.
//

#pragma once

#include <windows.h>
#include <memory>
#include <assert.h>

class WinDebugBreakpointManager
{
public:

	enum class RegisterIndex { FIRST, SECOND, THIRD, FOURTH };
	enum class BreakCondition { WRITE, READ_WRITE};
	enum class DataSize { ONE_BYTE, TWO_BYTES, FOUR_BYTES, EIGHT_BYTES };

	template<class Value_t>
	static void SetBreakpoint(const Value_t* const address, const RegisterIndex registerIndex, const DataSize dataSize, const BreakCondition breakCondition = BreakCondition::WRITE)
	{
		HANDLE threadHandle = 0;
		auto success = DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &threadHandle, 0, false, DUPLICATE_SAME_ACCESS);
		assert(success);

		auto eventHandle = CreateEvent(nullptr, true, false, TEXT("DebugAddBreakpoint"));
		assert(eventHandle);

		const auto pBreakpointInfo = std::unique_ptr<BreakpointInfo>(new BreakpointInfo{
			reinterpret_cast<DWORD_PTR>(address), threadHandle, eventHandle, registerIndex, breakCondition, dataSize });

		auto addBreakPointThread = CreateThread(nullptr, 0, AddBreakpoint, reinterpret_cast<LPVOID>(&*pBreakpointInfo), 0, 0);
		assert(addBreakPointThread);

		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(addBreakPointThread);
	}

	static void ClearBreakpoint(RegisterIndex registerIndex)
	{
		HANDLE threadHandle = 0;
		auto success = DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &threadHandle, 0, false, DUPLICATE_SAME_ACCESS);
		assert(success);

		auto pBreakpointInfo = std::make_unique<BreakpointInfo>();
		pBreakpointInfo->m_threadHandle = threadHandle;
		pBreakpointInfo->m_registerIndex = registerIndex;
		pBreakpointInfo->m_eventHandle = CreateEvent(nullptr, true, false, TEXT("DebugRemoveBreakpoint"));
		
		auto removeBreakPointThread = CreateThread(nullptr, 0, RemoveBreakpoint, reinterpret_cast<LPVOID>(&*pBreakpointInfo), 0, 0);
		assert(removeBreakPointThread);

		WaitForSingleObject(pBreakpointInfo->m_eventHandle, INFINITE);
		CloseHandle(removeBreakPointThread);
	}

private:

	template<class Value_t>
	static Value_t& SetBit(Value_t& value, const int bit)
	{
		return value |= (static_cast<Value_t>(1) << bit);
	};

	static DWORD WINAPI AddBreakpoint(LPVOID const pData)
	{
		const auto& breakpointInfo = *reinterpret_cast<BreakpointInfo*>(pData);

		auto suspendCount = SuspendThread(breakpointInfo.m_threadHandle);
		assert(suspendCount >= 0);

		CONTEXT context;
		context.ContextFlags = CONTEXT_DEBUG_REGISTERS;

		auto success = GetThreadContext(breakpointInfo.m_threadHandle, &context);
		assert(success);

		const int registerOffset = static_cast<int>(breakpointInfo.m_registerIndex);
		*(&context.Dr0 + registerOffset) = breakpointInfo.m_pVariableAddress;
		
		SetBit(context.Dr7, registerOffset * 2);
		if (breakpointInfo.m_breakCondition == BreakCondition::WRITE)
		{
			SetBit(context.Dr7, 16 + registerOffset * 2);
		}
		else
		{
			SetBit(context.Dr7, 16 + registerOffset * 2);
			SetBit(context.Dr7, 17 + registerOffset * 2);
		}

		switch (breakpointInfo.m_dataSize)
		{
		case DataSize::TWO_BYTES:
		{
			SetBit(context.Dr7, 18 + registerOffset * 2);
			break;
		}
		case DataSize::FOUR_BYTES:
		{
			SetBit(context.Dr7, 18 + registerOffset * 2);
			SetBit(context.Dr7, 19 + registerOffset * 2);
			break;
		}
		case DataSize::EIGHT_BYTES:
		{
			SetBit(context.Dr7, 19 + registerOffset * 2);
			break;
		}
		default:
			break;
		}

		success = SetThreadContext(breakpointInfo.m_threadHandle, &context);
		assert(success);

		suspendCount = ResumeThread(breakpointInfo.m_threadHandle);
		assert(suspendCount >= 0);

		SetEvent(breakpointInfo.m_eventHandle);

		return 0;
	}

	template<class Value_t>
	static Value_t& ClearBit(Value_t& value, const int bit)
	{
		return value &= ~(static_cast<Value_t>(1) << bit);
	};

	static DWORD WINAPI RemoveBreakpoint(LPVOID const pData)
	{
		const auto& breakpointInfo = *reinterpret_cast<BreakpointInfo*>(pData);

		auto suspendCount = SuspendThread(breakpointInfo.m_threadHandle);
		assert(suspendCount >= 0);

		CONTEXT context;
		context.ContextFlags = CONTEXT_DEBUG_REGISTERS;

		auto success = GetThreadContext(breakpointInfo.m_threadHandle, &context);
		assert(success);

		const int registerOffset = static_cast<int>(breakpointInfo.m_registerIndex);
		*(&context.Dr0 + registerOffset) = NULL;

		ClearBit(context.Dr7, 16 + registerOffset * 2);
		ClearBit(context.Dr7, 17 + registerOffset * 2);
		ClearBit(context.Dr7, 18 + registerOffset * 2);
		ClearBit(context.Dr7, 19 + registerOffset * 2);

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
		HANDLE m_eventHandle{ 0 };
		RegisterIndex m_registerIndex{ RegisterIndex::FIRST };
		BreakCondition m_breakCondition{ BreakCondition::WRITE };
		DataSize m_dataSize{ DataSize::ONE_BYTE };
	};
};
