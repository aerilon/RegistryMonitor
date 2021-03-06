#include "stdafx.h"

#include <iostream>
#include <string>

#include <Windows.h>



void
access_registry()
{
	HKEY hKey;

	if (auto rv = ::RegOpenKeyExA(HKEY_CURRENT_USER, "Environment", 0, KEY_READ | KEY_WRITE, &hKey);
		rv != ERROR_SUCCESS)
	{
		std::cerr << "RegOpenKeyEx: failed with error: " << rv << std::endl;
		return;
	}

	std::string s;
	s.reserve(128);
	
	DWORD type;
	BYTE* buf = (BYTE*)s.data();
	DWORD buflen = (DWORD)s.capacity();
	if (auto rv = ::RegQueryValueExA(hKey, "DEMO", 0, &type, buf, &buflen);
		rv != ERROR_SUCCESS)
	{
		if (rv != ERROR_FILE_NOT_FOUND)
		{
			std::cerr << "RegQueryValueExA: failed with error: " << rv << std::endl;
			return;
		}

		s = "0";
	}
	
	// should run within a try/catch block to catch conversion errors
	auto ll = std::stoll(s);

	s = std::to_string(++ll);
	if (auto rv = ::RegSetValueExA(hKey, "DEMO", 0, REG_SZ, (BYTE*)s.c_str(), (DWORD)s.length() + 1);
		rv != ERROR_SUCCESS)
	{
		std::cerr << "RegSetValueKeyEx: failed with error: " << rv << std::endl;
		return;
	}	
}

int main()
{
	// Output the current process Id
	HANDLE currentThread = GetCurrentThread();
	std::cout << "Process id: " << GetProcessIdOfThread(currentThread);
	std::cout << "\n";
	CloseHandle(currentThread);

	while (true)
	{
		std::cout << "Press <enter> to perform registry access (Ctrl-C to exit): ";

		std::string value;
		std::getline(std::cin, value);

		access_registry();
	}
	return 0;
}

