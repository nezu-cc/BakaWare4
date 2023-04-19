#include "crash_handler.h"
#include <windows.h>
#include <psapi.h>
#include <format>
#include <fstream>
#include <ctime>
#include <sstream>
#include "logger.h"

static uintptr_t cheat_base{ };

void save_crash_log(std::string_view log) noexcept {
    auto file = std::ofstream(std::vformat(XOR("BakaWare_crash_{}.txt"), std::make_format_args(std::time(nullptr))));
    file << log;
}

void copy_crash_log_to_clipboard(std::string_view log) noexcept {
    if (OpenClipboard(NULL)) {
        HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, log.size() + 1);
        if (!hg) {
            CloseClipboard();
            return;
        }
        memcpy(GlobalLock(hg), log.data(), log.size() + 1);
        GlobalUnlock(hg);
        EmptyClipboard();
        SetClipboardData(CF_TEXT, hg);
        CloseClipboard();
        GlobalFree(hg);
    }
}

LONG CALLBACK veh_handler(EXCEPTION_POINTERS* ExceptionInfo) noexcept {
	auto exception_code = ExceptionInfo->ExceptionRecord->ExceptionCode;
    auto exception_address = ExceptionInfo->ExceptionRecord->ExceptionAddress;
	auto exception_info0 = ExceptionInfo->ExceptionRecord->ExceptionInformation[0];
	auto exception_info1 = ExceptionInfo->ExceptionRecord->ExceptionInformation[1];
	auto exception_info2 = ExceptionInfo->ExceptionRecord->ExceptionInformation[2];

    // DEP bypass
    if (exception_code == EXCEPTION_ACCESS_VIOLATION) {
        // DEP Violation
        if (exception_info0 == 8) {
            // ignore DEP violation above 0x7FFFFFFEFFFF
            if (exception_info1 >= 0x7FFFFFFEFFFF)
                return EXCEPTION_CONTINUE_EXECUTION;
        }
    }

    if (exception_code != EXCEPTION_ACCESS_VIOLATION && exception_code != EXCEPTION_ARRAY_BOUNDS_EXCEEDED &&
		exception_code != EXCEPTION_DATATYPE_MISALIGNMENT && exception_code != EXCEPTION_FLT_DENORMAL_OPERAND &&
		exception_code != EXCEPTION_FLT_DIVIDE_BY_ZERO && exception_code != EXCEPTION_FLT_INEXACT_RESULT &&
		exception_code != EXCEPTION_FLT_INVALID_OPERATION && exception_code != EXCEPTION_FLT_OVERFLOW &&
		exception_code != EXCEPTION_FLT_STACK_CHECK && exception_code != EXCEPTION_FLT_UNDERFLOW &&
		exception_code != EXCEPTION_ILLEGAL_INSTRUCTION && exception_code != EXCEPTION_IN_PAGE_ERROR &&
		exception_code != EXCEPTION_INT_DIVIDE_BY_ZERO && exception_code != EXCEPTION_INT_OVERFLOW &&
		exception_code != EXCEPTION_INVALID_DISPOSITION && exception_code != EXCEPTION_NONCONTINUABLE_EXCEPTION &&
		exception_code != EXCEPTION_PRIV_INSTRUCTION && exception_code != EXCEPTION_STACK_OVERFLOW)
		return EXCEPTION_CONTINUE_SEARCH;

    std::stringstream ss;

    auto log = [&](std::string_view fmt, auto&&... args) {
        std::string str;
        if constexpr (sizeof...(args) > 0)
            str = std::vformat(fmt, std::make_format_args(std::forward<decltype(args)>(args)...));
        else
            str = fmt;

        ss << str << '\n';
    };

    switch (exception_code)
	{
	case EXCEPTION_ACCESS_VIOLATION:
        log(XOR("Cause: EXCEPTION_ACCESS_VIOLATION"));
		if (exception_info0 == 0) log(XOR("Attempted to read from: 0x{:016x}"), exception_info1);
        else if (exception_info0 == 1) log(XOR("Attempted to write to: 0x{:016x}"), exception_info1);
        else if (exception_info0 == 8) log(XOR("Data Execution Prevention (DEP) at: 0x{:016x}"), exception_info1);
        else log(XOR("Unknown access violation at: 0x{:016x}"), exception_info1);
		break;
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED: log(XOR("Cause: EXCEPTION_ARRAY_BOUNDS_EXCEEDED")); break;
	case EXCEPTION_DATATYPE_MISALIGNMENT: log(XOR("Cause: EXCEPTION_DATATYPE_MISALIGNMENT")); break;
	case EXCEPTION_FLT_DENORMAL_OPERAND: log(XOR("Cause: EXCEPTION_FLT_DENORMAL_OPERAND")); break;
	case EXCEPTION_FLT_DIVIDE_BY_ZERO: log(XOR("Cause: EXCEPTION_FLT_DIVIDE_BY_ZERO")); break;
	case EXCEPTION_FLT_INEXACT_RESULT: log(XOR("Cause: EXCEPTION_FLT_INEXACT_RESULT")); break;
	case EXCEPTION_FLT_INVALID_OPERATION: log(XOR("Cause: EXCEPTION_FLT_INVALID_OPERATION")); break;
	case EXCEPTION_FLT_OVERFLOW: log(XOR("Cause: EXCEPTION_FLT_OVERFLOW")); break;
	case EXCEPTION_FLT_STACK_CHECK: log(XOR("Cause: EXCEPTION_FLT_STACK_CHECK")); break;
	case EXCEPTION_FLT_UNDERFLOW: log(XOR("Cause: EXCEPTION_FLT_UNDERFLOW")); break;
	case EXCEPTION_ILLEGAL_INSTRUCTION: log(XOR("Cause: EXCEPTION_ILLEGAL_INSTRUCTION")); break;
	case EXCEPTION_IN_PAGE_ERROR:
		log( XOR("Cause: EXCEPTION_IN_PAGE_ERROR"));
		if (!exception_info0) log(XOR("Attempted to read from: 0x{:016x}"), exception_info1);
        else if (exception_info0 == 1) log(XOR("Attempted to write to: 0x{:016x}"), exception_info1);
        else if (exception_info0 == 8) log(XOR("Data Execution Prevention (DEP) at: 0x{:016x}"), exception_info1);
        else log(XOR("Unknown access violation at: 0x{:016x}"), exception_info1);
		log(XOR("NTSTATUS: 0x{:016x}"), exception_info2);
		break;
	case EXCEPTION_INT_DIVIDE_BY_ZERO: log(XOR("Cause: EXCEPTION_INT_DIVIDE_BY_ZERO")); break;
	case EXCEPTION_INT_OVERFLOW: log(XOR("Cause: EXCEPTION_INT_OVERFLOW")); break;
	case EXCEPTION_INVALID_DISPOSITION: log(XOR("Cause: EXCEPTION_INVALID_DISPOSITION")); break;
	case EXCEPTION_NONCONTINUABLE_EXCEPTION: log(XOR("Cause: EXCEPTION_NONCONTINUABLE_EXCEPTION")); break;
	case EXCEPTION_PRIV_INSTRUCTION: log(XOR("Cause: EXCEPTION_PRIV_INSTRUCTION")); break;
	case EXCEPTION_STACK_OVERFLOW: log(XOR("Cause: EXCEPTION_STACK_OVERFLOW")); break;
	}

	LPVOID allocation_Base = nullptr;
	MEMORY_BASIC_INFORMATION info = {};
	if (VirtualQuery(exception_address, &info, sizeof info))
        allocation_Base = info.AllocationBase;
    else if (cheat_base >= 0x7FFFFFFEFFFF)
        allocation_Base = (LPVOID)cheat_base;

    CHAR module_name[MAX_PATH] = "";
    if (!GetModuleBaseNameA(GetCurrentProcess(), (HMODULE)allocation_Base, module_name, sizeof(module_name))) {
        if (!cheat_base)
            sprintf_s(module_name, XOR("Unknown(%p)"), allocation_Base);
        else if ((uintptr_t)allocation_Base == cheat_base)
            sprintf_s(module_name, XOR("BakaWare4"));
        else {
            //parse header of manual mapped module
            PIMAGE_DOS_HEADER dos_header = (PIMAGE_DOS_HEADER)cheat_base;
            if (dos_header->e_magic == 0x5A4D) { //MZ
                PIMAGE_NT_HEADERS ntHeader = (PIMAGE_NT_HEADERS)(cheat_base + dos_header->e_lfanew);
                auto image_size = ntHeader->OptionalHeader.SizeOfImage;
                if ((uintptr_t)exception_address >= cheat_base && (uintptr_t)exception_address <= cheat_base + image_size) {
                    sprintf_s(module_name, XOR("BakaWare4"));
                }
            }
        }
    }

    log(XOR("Cheat base: 0x{}"), (void*)cheat_base);
    log(XOR("Crash at: 0x{:016x} ({}+0x{:x})"), (uintptr_t)ExceptionInfo->ContextRecord->Rip, module_name, (uintptr_t)ExceptionInfo->ContextRecord->Rip - (uintptr_t)allocation_Base);
    
    log(XOR("Registers:"));
    log(XOR("RAX: 0x{:016x}"), ExceptionInfo->ContextRecord->Rax);
    log(XOR("RCX: 0x{:016x}"), ExceptionInfo->ContextRecord->Rcx);
    log(XOR("RDX: 0x{:016x}"), ExceptionInfo->ContextRecord->Rdx);
    log(XOR("RBX: 0x{:016x}"), ExceptionInfo->ContextRecord->Rbx);
    log(XOR("RSP: 0x{:016x}"), ExceptionInfo->ContextRecord->Rsp);
    log(XOR("RBP: 0x{:016x}"), ExceptionInfo->ContextRecord->Rbp);
    log(XOR("RSI: 0x{:016x}"), ExceptionInfo->ContextRecord->Rsi);
    log(XOR("RDI: 0x{:016x}"), ExceptionInfo->ContextRecord->Rdi);
    log(XOR("R8 : 0x{:016x}"), ExceptionInfo->ContextRecord->R8);
    log(XOR("R9 : 0x{:016x}"), ExceptionInfo->ContextRecord->R9);
    log(XOR("R10: 0x{:016x}"), ExceptionInfo->ContextRecord->R10);
    log(XOR("R11: 0x{:016x}"), ExceptionInfo->ContextRecord->R11);
    log(XOR("R12: 0x{:016x}"), ExceptionInfo->ContextRecord->R12);
    log(XOR("R13: 0x{:016x}"), ExceptionInfo->ContextRecord->R13);
    log(XOR("R14: 0x{:016x}"), ExceptionInfo->ContextRecord->R14);
    log(XOR("R15: 0x{:016x}"), ExceptionInfo->ContextRecord->R15);

    crash_handler::handle_crash(ss.str());

	return EXCEPTION_CONTINUE_SEARCH;
}

void crash_handler::initialize(const uintptr_t base) noexcept {
    cheat_base = base;
    PVOID handle = AddVectoredExceptionHandler(FALSE, veh_handler);
    LOG_INFO(XOR("VEH handler at: {}"), handle);
}

void crash_handler::end() noexcept {
    RemoveVectoredExceptionHandler(veh_handler);
}

void crash_handler::handle_crash(std::string_view crash_info) noexcept {
    // copy to clipboard first in case save_crash_log fails
    copy_crash_log_to_clipboard(crash_info);
    save_crash_log(crash_info);
    MessageBoxA(NULL, crash_info.data(), XOR("BakaWare4 has crashed!"), MB_OK | MB_ICONERROR);
    std::abort();
}
