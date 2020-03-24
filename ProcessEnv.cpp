// ProcessEnv.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <Psapi.h>
using namespace std;

typedef long NTSTATUS;
#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)
typedef enum _PROCESSINFOCLASS { ProcessBasicInformation } PROCESSINFOCLASS;
typedef struct _INFOBLOCK
{
	unsigned long dwFiller[16];
	unsigned short wLength;
	unsigned short wMaxLength;
	const unsigned short *dwCmdLineAddress;
	const unsigned short *env;
} INFOBLOCK, *PINFOBLOCK;
typedef struct _PEB
{
	unsigned long dwFiller[4];
	PINFOBLOCK dwInfoBlockAddress;
} PEB, *PPEB;
typedef struct _PROCESS_BASIC_INFORMATION
{
	NTSTATUS ExitStatus;
	PPEB PebBaseAddress;
	unsigned long AffinityMask;
	long BasePriority;
	unsigned long UniqueProcessId;
	unsigned long InheritedFromUniqueProcessId;
} PBI;
typedef NTSTATUS(NTAPI *ZWQueryInformationProcessW)(HANDLE, PROCESSINFOCLASS, PVOID, ULONG, PULONG);

void Get(HANDLE hProcess)
{
	ZWQueryInformationProcessW ZwQueryInformationProcessA;
	HMODULE hModule = GetModuleHandle(_T("ntdll"));
	ZwQueryInformationProcessA = (ZWQueryInformationProcessW)GetProcAddress(hModule, "ZwQueryInformationProcess");
	if (ZwQueryInformationProcessA == NULL) exit(1);
	PBI ProcInfo;
	PEB ProcPEB;
	INFOBLOCK ProcBlock;
	unsigned long ReturnLength;
	//HANDLE hProcess;
	unsigned short *pszCmdLine = NULL;
	int bSuccess;
	//hProcess = GetCurrentProcess();
	//hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, 2780);
	if (!NT_SUCCESS(ZwQueryInformationProcessA(hProcess, ProcessBasicInformation, &ProcInfo, sizeof(ProcInfo), &ReturnLength))) exit(1);
	bSuccess = ReadProcessMemory(hProcess, (const void *)ProcInfo.PebBaseAddress, &ProcPEB, sizeof(ProcPEB), &ReturnLength);
	if (bSuccess != false)
	{
		bSuccess = ReadProcessMemory(hProcess, (const void *)ProcPEB.dwInfoBlockAddress, &ProcBlock, sizeof(ProcBlock), &ReturnLength);
		pszCmdLine = (unsigned short *) new BYTE[ProcBlock.wMaxLength];
	}
	if (bSuccess != false)
	{
		bSuccess = ReadProcessMemory(hProcess, ProcBlock.dwCmdLineAddress, pszCmdLine, ProcBlock.wMaxLength, &ReturnLength);
	}
	_tprintf(TEXT("%S\n"), pszCmdLine);
	if (NULL != pszCmdLine) delete[] pszCmdLine;
	// CloseHandle(hProcess);
	return;
}

int main()
{
	// Get the list of process identifiers.
	unsigned long processID[1024];
	unsigned long size;
	unsigned long n_processID;
	char szProcessName[MAX_PATH] = TEXT("<unknown>");
	HANDLE hProcess;
	HMODULE hModule;
	std::vector<std::pair<unsigned long, std::string> > processes;
	if (!EnumProcesses(processID, sizeof(processID), &size)) return -1;
	// Calculate how many process identifiers were returned.
	n_processID = size / sizeof(unsigned long);
	// Print the name and process identifier for each process.
	for (unsigned ii = 0; ii < n_processID; ++ii)
	{
		if (processID[ii] == 0) continue;

		// Get a handle to the process.
		hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, processID[ii]);
		// Get the process name.
		if (hProcess == NULL) continue;
		if (EnumProcessModules(hProcess, &hModule, sizeof(hModule), &size))
		{
			GetModuleBaseName(hProcess, hModule, szProcessName, sizeof(szProcessName) / sizeof(char));
			processes.push_back(std::make_pair(processID[ii], szProcessName));
		}
		if (!_stricmp(szProcessName, "slack.exe"))
		{
			Get(hProcess);
			std::cout << "Found PID: " << processID[ii] << std::endl;
		}
		//_tprintf(TEXT("%s  (PID: %u)\n"), szProcessName, processID[ii]);
		CloseHandle(hProcess);
	}
	std::sort(processes.begin(), processes.end());
	return 0;
}