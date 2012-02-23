#include <iostream>
#include <windows.h>
#include <cstdio>
using namespace std;

struct MulArgs 
{
	int index;
	int size;
	double* row;
	double* vector;
	double* resultCell;
};

DWORD WINAPI mul (LPVOID arg)
{
	MulArgs args = *((MulArgs*) arg);
	double tmp = 0;
	for (int i=0;i<args.size;i++)
	{
		tmp += args.row[i]*args.vector[i];
	}
	*(args.resultCell) = tmp;
	cout<<"A_"<<args.index<<" * vector  = "<<tmp<<endl;
	Sleep(7);
	cout<<"Closing thread #"<<args.index<<endl;
	ExitThread (0);
}

int main ()
{
	FILE* file_in = fopen ("input.in","r");
	int size = 0;
	fscanf(file_in,"%d",&size);
	double** matrix = new double* [size];
	for (int i=0;i<size;i++)
	{
		matrix[i]=new double [size];
		for (int j=0;j<size;j++)
		{
			fscanf(file_in,"%lf",&matrix[i][j]);
		}
	}

	double* vector = new double [size];
	for (int i=0;i<size;i++)
	{
		fscanf(file_in,"%lf",&vector[i]);
	}

	HANDLE* hThreads = new HANDLE [size];
	MulArgs* args = new MulArgs [size];
	LPDWORD* threadsID = new LPDWORD [size];
	double* result = new double [size];
	for (int i=0;i<size;i++)
	{
		args[i].index = i;
		args[i].size = size;
		args[i].row =matrix[i];
		args[i].vector = vector;
		args[i].resultCell = &result[i];

		hThreads[i] = CreateThread (NULL,0, mul, (void*) &args[i], 0, threadsID[i]);
		if (!hThreads)
		{
			cout<<"Error on starting "<<i<<" thread...\n";
		}
	}
	WaitForMultipleObjects (size, hThreads, true, INFINITE);
	for (int i=0;i<size;i++)
	{
		printf ("%8.3lf ", result[i]);
	}
	cout<<endl;

	delete[] args;
	delete[] result;
	delete[] hThreads;
	delete[] vector;
	delete[] threadsID;
	for (int i=0;i<size;i++)
	{
		delete[] matrix[i];
	}
	delete[] matrix;
	return 0;
}
