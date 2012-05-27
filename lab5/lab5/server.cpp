#include <windows.h>
#include "common_header.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <conio.h>

using namespace std;
CRITICAL_SECTION cs;
enum States {MAKE,START_CLIENT,PRINT, SET_PENDING,BAD_INPUT,EMPTY, SHUTDOWN, HELP};
struct Arguments 
{
	HANDLE hWrite;
	HANDLE hRead;
	int ID;	
	FILE* file;
};


bool die = false;
 

//функция разбора командной строки
States parseCmd ()
{
	printf (">");
	rewind (stdin);
	char* input = new char [100];
	States state = BAD_INPUT;
	cin.getline(input,100);
	printf ("\'%s\'\n",input);
	if (strcmp ("make",input)==0) state = MAKE;
	if (strcmp ("start clients", input) == 0) state = START_CLIENT;
	if (strcmp ("print", input)==0) state = PRINT;
	if (strcmp ("set pending", input)==0) state = SET_PENDING;
	if (strcmp ("die",input)==0) state = SHUTDOWN;
	if (strcmp ("help",input)==0) state = HELP;
	if (strcmp ("", input)==0) state = EMPTY;	
	delete [] input;
	return state;
}

// функция создания бинарного файла
void make ()
{
	printf ("Output file: ");
	char* filename = new char [100];
	scanf ("%s",filename);
	FILE* fout = fopen (filename,"wb");
	if (!fout)
	{
		printf ("ERR:Can't write\n");
		return;
	}
	delete [] filename;
	printf ("Record count: ");
	int count =0;
	scanf ("%d",&count);
	for (int i=0;i<count;i++)
	{
		Order  order; 
		printf ("\nRecord #%d:\n",i+1);
		printf ("\t\tName: ");
		scanf ("%s",&order.name);
		printf ("\t\tAmount: ");
		scanf ("%d",&order.amount);
		printf ("\t\tPrice: ");
		scanf ("%lf",&order.price);
		fwrite(&order,sizeof(Order),1,fout);
	}
	fclose (fout);
}

void help ()
{
	printf ("make \t\t-\t make binary file\n");
	printf ("start clients \t-\t start clients\n");
	printf ("print \t\t-\t print binary file\n");
	printf ("set pending \t-\t set active file\n");
	printf ("die \t\t-\t kill server\n");
	printf ("help \t\t-\t prints this message\n");
}
// выключить сервер
void kill(HANDLE* hServerThread, HANDLE* hClients, Arguments* args,DWORD* serverThreadID, int clientCount)
{
	{
		for (int i=0;i<clientCount;i++)
		{			 
			TerminateProcess(hClients[i],0);
			CloseHandle(hServerThread);
			CloseHandle(hClients);
		}
		delete [] hServerThread;
		delete [] args;
		delete [] serverThreadID;
		delete [] hClients;

	}
}


// поток обслуживающий клиентов
DWORD WINAPI serverThread (LPVOID arg)
{

	Arguments args = *((Arguments*) arg);
	HANDLE hPut, hGet;
	char lpszPut[20];
	char lpszGet[20];  
	wsprintf(lpszGet, "GET%d ",args.ID);
	wsprintf(lpszPut, "PUT%d ",args.ID);
	hPut = CreateEvent(NULL,FALSE,FALSE,lpszPut);
	hGet = CreateEvent(NULL,FALSE,FALSE,lpszGet);
	while(!die)
	{
		
		DWORD dwBytesWritten;
		if (!WriteFile(args.hWrite,&args.ID,sizeof(args.ID),&dwBytesWritten,NULL))
		{
			_cputs("Write to file failed.\n");
			_cputs("Press any key to finish.\n");
			_getch();
			return GetLastError();
		}
		SetEvent(hPut);
		_cprintf("The number %d is written to the pipe.\n", args.ID);
		WaitForSingleObject(hGet,INFINITE);
	}
	ExitThread (0);
}
//запустить клиенты
bool startClients (HANDLE* hServerThread, HANDLE* hClients, Arguments* args,DWORD* serverThreadID, int &clientCount,FILE* file)
{
	bool noErr = true;
	// если уже были созданы какие-то клиенты, отключаем их	 
	{
		for (int i=0;i<clientCount;i++)
		{		
			TerminateThread(hServerThread[i],0);
			TerminateProcess(hClients[i],0);
			CloseHandle(hServerThread);
			CloseHandle(hClients);
		}
		delete [] hServerThread;
		delete [] args;
		delete [] serverThreadID;
		delete [] hClients;

	}
	printf ("Client count:");
	scanf("%d",&clientCount);
	hServerThread = new HANDLE[clientCount];
	hClients = new HANDLE [clientCount];
	args = new Arguments [clientCount];
	serverThreadID = new DWORD [clientCount];

	printf ("Process ID \t\t[Path\t\tID\thWrite\thRead\t]\n" );
	for (int i=0;i<clientCount;i++)
	{
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		HANDLE hWritePipe, hReadPipe;
		// создаем анонимный канал
		// устанавливает атрибуты защиты канала
		SECURITY_ATTRIBUTES sa;

		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = NULL;
		// защита по умолчанию
		sa.bInheritHandle = TRUE;
		// дескрипторы наследуемые

		if(!CreatePipe(	&hReadPipe,	&hWritePipe, &sa, 0))			
		{
			_cputs("Create pipe failed.\n");
			_cputs("Press any key to finish.\n");
			_getch();
			return false;
		}


		char lpszComLine[80];
		wsprintf(lpszComLine, "client.exe %d %d %d",i,(int) hReadPipe,(int) hWritePipe);
		ZeroMemory(&si, sizeof(STARTUPINFO));
		si.cb = sizeof(STARTUPINFO);

		// запускаем клиента
		if (!CreateProcess(NULL,lpszComLine,NULL,	NULL,TRUE,	CREATE_NEW_CONSOLE,	NULL,	NULL, &si,&pi)			)
		{
			_cputs("Create process failed.\n");
			_cputs("Press any key to finish.\n");
			_getch();
			return false;
		}
		else
		{
			printf ("Process %d \t\t[%s\t%d\t%d\t%d\t]\t [OK]\n",i+1,"client.exe",i,(int) hReadPipe,(int) hWritePipe);
		}
		hClients [i]= pi.hProcess;

		args[i].file = file;
		args[i].hRead = hReadPipe;
		args[i].hWrite = hWritePipe;
		args[i].ID = i;		
		// запускаем поток, связанный с этим клиентом
		hServerThread[i] = CreateThread(NULL,NULL,serverThread,(void*)&args[i],NULL,&serverThreadID[i]);
		if (!hServerThread[i]) 
		{
			_cputs("Create server threa failed.\n");
			_cputs("Press any key to finish.\n");
			_getch();
			return false;
		}
	}
}

// выбираем какой файл обрабатывать
void setPending (FILE* file)
{

	printf ("Set filename: ");
	char filename[100];
	scanf ("%s",&filename);
	file = fopen (filename,"r+b");
}

// вывести файл
void print ()
{
	char* filename = new char [100];
	printf ("Print what: ");
	scanf ("%s",filename);
	FILE* fin = fopen (filename,"rb");
	if (!fin) 
	{
		printf ("ERR:No such file\n");
		return;
	}
	int i=0;
	while (!feof(fin))
	{
		Order order;
		fread (&order,sizeof(Order),1,fin);
		printf ("Record %d:\n\t\tName: %s\n\t\tAmount: \%d\n\t\tPrice: %3.3lf\n",i+1,order.name,order.amount,order.price);
	}
	fclose (fin);
}
int main()
{
	FILE* activeFile = NULL;
	HANDLE* hServerThread=NULL;
	HANDLE* hClients=NULL;
	DWORD* serverThreadID=NULL;
	Arguments* args=NULL;
	int clientCount=0;
	InitializeCriticalSection(&cs);

	while (!die)
	{
		switch (parseCmd ())
		{
			// пустой ввод
		case EMPTY: break;
			// выключить
		case SHUTDOWN: die = true; kill (hServerThread,hClients,args,serverThreadID,clientCount); break;
			// вывести файл
		case PRINT: print (); break;
			//	создать файл с базой
		case MAKE: make();break;
			// запустить клиентов
		case START_CLIENT: startClients (hServerThread,hClients,args,serverThreadID,clientCount,activeFile); break;
			// установить файл с базой
		case SET_PENDING: setPending (activeFile); if (!activeFile) printf("ERR:File not exist\n"); break;
			//	ошибка ввода
		case BAD_INPUT: printf ("ERR:Bad input. Use 'help' for help\n"); break;
			// помощь по командам
		case HELP: help (); break;
		}
	}

	if (activeFile) fclose (activeFile);	
	return 0;
}