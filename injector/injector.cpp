#include "stdafx.h"

#include <iostream>
#include <string>
#include <cstring>

#include <easyhook.h>

int main()
{
	std::cout << "Enter target process ID: ";
	DWORD processId;
	std::cin >> processId;

	WCHAR* helper64 = (WCHAR*)L"..\\Debug\\helper64.dll";

	// Inject dllToInject into the target process Id, passing 
	// freqOffset as the pass through data.
	if (NTSTATUS nt = RhInjectLibrary(
		processId,   // The process to inject into
		0,           // ThreadId to wake up upon injection
		EASYHOOK_INJECT_DEFAULT,
		NULL, // 32-bit
		helper64, // 64-bit
		NULL, // data to send to injected DLL entry point
		0 // size of data to send
		); nt != 0)
	{
		std::wcout << L"RhInjectLibrary failed: " << RtlGetLastErrorString();
	}
	else
	{
		std::wcout << L"Library injected successfully.\n";
	}

	std::wcout << "Press Enter to exit";
	std::wstring input;
	std::getline(std::wcin, input);
	std::getline(std::wcin, input);
	return 0;
}

