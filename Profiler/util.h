#pragma once

extern "C" {
	BOOL WINAPI EnumProcessModules(
		HANDLE hProcess,
		HMODULE *lphModule,
		DWORD cb,
		LPDWORD lpcbNeeded
	);

	DWORD WINAPI GetModuleFileNameExA(
		_In_     HANDLE  hProcess,
		_In_opt_ HMODULE hModule,
		_Out_    LPSTR  lpFilename,
		_In_     DWORD   nSize
	);

	HMODULE WINAPI
		GetModuleHandleA(
			_In_opt_ LPCWSTR lpModuleName
		);
}

