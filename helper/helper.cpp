#include "stdafx.h"

#include <string>
#include <iostream>
#include <vector>
#include <tuple>

#include <Windows.h>

#include <easyhook.h>

LONG WINAPI
_RegOpenKeyExA(HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult)
{
	std::cout << __func__ << std::endl;

	return RegOpenKeyExA(hKey, lpSubKey, ulOptions, samDesired, phkResult);
}


LONG WINAPI
_RegQueryValueExA(HKEY hKey, LPCSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData)
{
	std::cout << __func__ << std::endl;

	return RegQueryValueExA(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
}

LONG WINAPI
_RegSetValueExA(HKEY hKey, LPCSTR lpValueName, DWORD Reserved, DWORD dwType, const BYTE *lpData, DWORD cbData)
{
	std::cout << __func__ << std::endl;

	return RegSetValueExA(hKey, lpValueName, Reserved, dwType, lpData, cbData);
}

// EasyHook will be looking for this export to support DLL injection. If not found then 
// DLL injection will fail.
extern "C" void __declspec(dllexport) __stdcall NativeInjectionEntryPoint(REMOTE_ENTRY_INFO* inRemoteInfo);

void __stdcall NativeInjectionEntryPoint(REMOTE_ENTRY_INFO* inRemoteInfo)
{

	/* Xal - to access shared buffer, use inRemoteInfo->UserData / inRemoteInfo->UserDataSize	*/

	// Perform hooking
	HOOK_TRACE_INFO hHook = { NULL }; // keep track of our hook

	std::vector<std::tuple<std::string, FARPROC, FARPROC, HOOK_TRACE_INFO>> hooks {
		{ "RegOpenKeyExA", (FARPROC)GetProcAddress(GetModuleHandle(TEXT("advapi32")), "RegOpenKeyExA"), (FARPROC)_RegOpenKeyExA, { NULL } },
		{ "RegQueryValueExA", (FARPROC)GetProcAddress(GetModuleHandle(TEXT("advapi32")), "RegQueryValueExA"), (FARPROC)_RegQueryValueExA, { NULL } },
		{ "RegSetValueExA", (FARPROC)GetProcAddress(GetModuleHandle(TEXT("advapi32")), "RegSetValueExA"), (FARPROC)_RegSetValueExA, { NULL } }
	};

	for (auto& hook : hooks)
	{
		auto [name, target, interceptor, info] = hook;

		std::cout << "Installing hook for " << name << "... ";

		std::cout << target << " -> " << interceptor << std::endl;

		NTSTATUS result = ::LhInstallHook(target, interceptor, NULL, &info);
		if (FAILED(result))
		{
			std::wstring s(RtlGetLastErrorString());
			std::wcout << "[error: " << RtlGetLastErrorString() << "]";
		}
		else
		{
			std::cout << "[ok]";
		}

		// If the threadId in the ACL is set to 0,
		// then internally EasyHook uses GetCurrentThreadId()
		ULONG ACLEntries[1] = { 0 };

		// Disable the hook for the provided threadIds, enable for all others
		LhSetExclusiveACL(ACLEntries, 1, &info);

		std::cout << std::endl;
	}

	return;
}
