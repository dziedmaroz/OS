#include <stdio.h>
#include <windows.h>
#include <conio.h>
int main1(int argc, char* argv[])
{
	printf ("Hello client app\n");
	if (argc>1)
	{
		printf ("Argc: %d\n",argc);
		printf ("%s\n",argv[0]);
		HANDLE hPut, hGet;
		char lpszPut[20];
		char lpszGet[20];  
		Sleep(100);
		wsprintf(lpszGet, "GET%d ",atoi(argv[1]));

		wsprintf(lpszPut, "PUT%d ",atoi(argv[1]));

		hPut = OpenEvent(EVENT_ALL_ACCESS, FALSE, lpszPut);
		
		if (!hPut) 
		{
			printf ("ERR: Can't open event PUT[%s]\n",lpszPut);
			Sleep (10000);
		

			return 100;
		}
		hGet = OpenEvent(EVENT_ALL_ACCESS,FALSE,lpszGet);
		if (!hPut)
		{
			printf ("ERR: Can't open event GET[%s]\n",lpszGet);
			Sleep (10000);
			return 100;
		}

		HANDLE hWrite = (HANDLE) atoi(argv[3]);
		HANDLE hRead = (HANDLE) atoi(argv[2]);

		printf ("hRead: %d\n",(int)hRead);
		printf ("hWrite: %d\n",(int) hWrite);

		while (true)
		{
			WaitForSingleObject(hPut,INFINITE);
			int nData;
			DWORD dwBytesRead;
			if (!ReadFile(hRead,&nData,	sizeof(nData),&dwBytesRead,	NULL))
			{
				_cputs("Read from the pipe failed.\n");
				_cputs("Press any key to finish.\n");
				_getch();
				return GetLastError();
			}
			_cprintf("The number %d is read from the pipe.\n", nData);
			SetEvent(hGet);
		}
	}
	_cputs("Press any key to finish.\n");
	_getch();
	return 0;
}