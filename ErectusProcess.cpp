#include "app.h"
#include "renderer.h"
#include "threads.h"

#include <thread>
#include <TlHelp32.h>

#include "ErectusProcess.h"
#include "ErectusMemory.h"
#include "MsgSender.h"


void ErectusProcess::SetProcessError(const int errorId, const char* error)
{
	if (errorId == -1)
		return;
	processErrorId = errorId;
	processError = error;
}

void ErectusProcess::ResetProcessData()
{
	SetProcessError(0, "Process State: No process selected");

	if (Threads::threadCreationState)
	{
		auto areThreadsActive = false;

		while (!Threads::ThreadDestruction())
		{
			Threads::threadDestructionCounter++;
			if (Threads::threadDestructionCounter > 1440)
			{
				areThreadsActive = true;
				gApp->Shutdown();
				break;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}

		if (!areThreadsActive)
			MsgSender::Patcher(false);
	}

	pid = 0;
	hWnd = nullptr;
	exe = 0;

	if (handle != nullptr)
	{
		CloseHandle(handle);
		handle = nullptr;
	}
}

std::vector<DWORD> ErectusProcess::GetProcesses()
{
	std::vector<DWORD> result{ 0 };

	auto* const hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE)
		return result;

	PROCESSENTRY32 lppe;
	lppe.dwSize = sizeof lppe;

	while (Process32Next(hSnapshot, &lppe))
	{
		if (!strcmp(lppe.szExeFile, "Fallout76.exe"))
			result.push_back(lppe.th32ProcessID);
	}

	CloseHandle(hSnapshot);

	return result;
}

bool ErectusProcess::Rpm(const DWORD64 src, void* dst, const size_t size)
{
	return ReadProcessMemory(handle, reinterpret_cast<void*>(src), dst, size, nullptr);
}

bool ErectusProcess::Wpm(const DWORD64 dst, void* src, const size_t size)
{
	return WriteProcessMemory(handle, reinterpret_cast<void*>(dst), src, size, nullptr);
}

DWORD64 ErectusProcess::AllocEx(const size_t size)
{
	//this needs to be split, the game scans for PAGE_EXECUTE_READWRITE regions
	//1) alloc with PAGE_READWRITE
	//2) write the data
	//3) switch to PAGE_EXECUTE_READ
	//4) create the remote thread
	//see https://reverseengineering.stackexchange.com/questions/3482/does-code-injected-into-process-memory-always-belong-to-a-page-with-rwx-access
	return DWORD64(VirtualAllocEx(handle, nullptr, size, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE));
}

bool ErectusProcess::FreeEx(const DWORD64 src)
{
	return VirtualFreeEx(handle, reinterpret_cast<void*>(src), 0, MEM_RELEASE);
}

BOOL ErectusProcess::HwndEnumFunc(const HWND hwnd, const LPARAM lParam)
{
	DWORD lpdwProcessId;
	GetWindowThreadProcessId(hwnd, &lpdwProcessId);

	if (lpdwProcessId == lParam)
	{
		char buffer[sizeof"Fallout76"] = { '\0' };
		if (GetClassName(hwnd, buffer, sizeof buffer))
		{
			if (!strcmp(buffer, "Fallout76"))
			{
				hWnd = hwnd;
				return FALSE;
			}
		}
	}

	hWnd = nullptr;
	return TRUE;
}

bool ErectusProcess::HwndValid(const DWORD processId)
{
	pid = processId;
	if (!pid)
	{
		SetProcessError(2, "Process State: PID (Process Id) invalid");
		return false;
	}

	EnumWindows(WNDENUMPROC(HwndEnumFunc), pid);
	if (hWnd == nullptr)
	{
		SetProcessError(2, "Process State: HWND (Window) invalid");
		return false;
	}

	if (IsIconic(hWnd))
	{
		SetProcessError(2, "Process State: HWND (Window) minimized");
		return false;
	}

	RECT rect;
	if (GetClientRect(hWnd, &rect) == FALSE || rect.right < 16 || rect.bottom < 16)
	{
		SetProcessError(2, "Process State: HWND (Window) invalid/minimized");
		return false;
	}

	SetProcessError(1, "Process State: Process selected");
	return true;
}

DWORD64 ErectusProcess::GetModuleBaseAddress(const DWORD pid, const char* module)
{
	auto* const hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
	if (hSnapshot == INVALID_HANDLE_VALUE)
		return 0;

	MODULEENTRY32 lpme;
	lpme.dwSize = sizeof lpme;

	while (Module32Next(hSnapshot, &lpme))
	{
		if (!strcmp(lpme.szModule, module))
		{
			CloseHandle(hSnapshot);
			return DWORD64(lpme.modBaseAddr);
		}
	}

	CloseHandle(hSnapshot);
	return 0;
}

bool ErectusProcess::AttachToProcess(const DWORD processId)
{
	if(pid == processId)
		return  true;

	ResetProcessData();

	if (processId == 0)
		return false;
	
	pid = processId;

	exe = GetModuleBaseAddress(pid, "Fallout76.exe");
	if (!exe)
	{
		SetProcessError(2, "Process State: Base Address invalid");
		return false;
	}

	handle = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
	if (handle == nullptr || !HwndValid(processId))
	{
		SetProcessError(2, "Process State: HANDLE invalid");
		return false;
	}

	SetProcessError(1, "Process State: Process selected");
	
	return true;
}
