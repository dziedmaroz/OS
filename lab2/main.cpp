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
	cout<<"Reading file input.in...\n";
	FILE* file_in = fopen ("input.in","r");
	if (!file_in)
	{
		cout<<"ERR:Something wrong with input file\n";
		return 100;
	}
	int size = 0;
	fscanf(file_in,"%d",&size);
	cout<<"Matrix size: "<<size<<endl;
	double** matrix = new double* [size];
	cout<<"Matrix: \n";
	for (int i=0;i<size;i++)
	{
		matrix[i]=new double [size];
		for (int j=0;j<size;j++)
		{
			fscanf(file_in,"%lf",&matrix[i][j]);
			printf ("%3.3lf ",matrix[i][i]);
		}
		printf ("\n");
	}
	cout<<"Reading vector...\n";
	double* vector = new double [size];
	for (int i=0;i<size;i++)
	{
		fscanf(file_in,"%lf",&vector[i]);
		printf ("%3.3lf ",vector[i]);
	}
	printf ("\n");

	HANDLE* hThreads = new HANDLE [size];
	MulArgs* args = new MulArgs [size];
	DWORD* threadsID = new DWORD [size];
	double* result = new double [size];
	for (int i=0;i<size;i++)
	{
		args[i].index = i;
		args[i].size = size;
		args[i].row =matrix[i];
		args[i].vector = vector;
		args[i].resultCell = &result[i];

		cout<<"Trying to start "<< i<<" thread\n";

		hThreads[i] = (HANDLE) CreateThread (NULL,0, mul, (void*) &args[i], 0, &threadsID[i]);
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
