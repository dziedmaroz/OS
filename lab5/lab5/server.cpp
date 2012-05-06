#include <windows.h>
#include "common_header.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;
enum States {MAKE,START_CLIENT,PRINT, SET_PENDING,BAD_INPUT,EMPTY, SHUTDOWN, HELP};
struct Arguments 
{
	HANDLE hWrite;
	HANDLE hRead;
	char* filename;
};


bool die = false;
CRITICAL_SECTION cs;

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
void kill()
{

}


// поток обслуживающий клиентов
DWORD WINAPI serverThread (LPVOID arg)
{
	while (!die)
	{
		printf ("some\n");
		Sleep (500);
	}
	ExitThread (0);
}
//запустить клиенты
void startClients (HANDLE* hServerThread)
{
	*hServerThread = CreateThread (NULL,NULL,serverThread,NULL,NULL,NULL);
	if (!hServerThread)
	{
		printf ("ERR: Can't start server thread\n");
		GetLastError();
		return;
	}
}

// выбираем какой файл обрабатывать
void setPending (char* &filename)
{
	printf ("Set filename: ");
	scanf ("%s",&filename);
}

// вывести активный файл
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
int main ()
{
	char* activeFilename = new char [100];
	HANDLE hServerThread;
	InitializeCriticalSection(&cs);
	
	while (!die)
	{
		switch (parseCmd ())
		{
			case EMPTY: break;
			case SHUTDOWN: die = true; kill (); break;
			case PRINT: print (); break;
			case MAKE: make();break;
			case START_CLIENT: startClients (&hServerThread); break;
			case SET_PENDING: setPending (activeFilename); break;
			case BAD_INPUT: printf ("ERR:Bad input. Use 'help' for help\n"); break;
			case HELP: help (); break;
		}

	}
	delete [] activeFilename;
	return 0;
}