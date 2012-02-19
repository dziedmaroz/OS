#include <iostream>
#include <windows.h>
#include <string>
using namespace std;

int main (int argc, char* argv[])
{
    STARTUPINFO creatorStartupInfo, reporterStartupInfo;
    PROCESS_INFORMATION creatorProcessInfo, reporterProcessInfo;

    ZeroMemory (&creatorStartupInfo, sizeof(STARTUPINFO));
    ZeroMemory (&reporterStartupInfo, sizeof(STARTUPINFO));
    cout<<argv[0]<<endl;
    string creatorConsole = string ("Z:\\home\\lucian\\University\\labOS\\lab1\\lab1-build-desktop\\creator.exe");
    //string creatorConsole=string ("creator.exe");
    cout<<"Binary filename for CREATOR output: ";
    string params=string();

    //
    //AllocConsole();
    cin>>params;
    cout<<"Trying to start CREATOR process ("<<creatorConsole<<")\n";
    if (!CreateProcess(creatorConsole.c_str(), params.c_str(), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &creatorStartupInfo, &creatorProcessInfo))
    {
        cout<<"ERR: The new process (CREATOR) is not created\n";
        return 100;
    }
    else
    {
        cout<<"lalala\n";
    }
    return 0;
}
