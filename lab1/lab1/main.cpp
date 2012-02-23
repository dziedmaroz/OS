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
    char* paramsCH;
    cin>>params;
    paramsCH = params.c_str();
    cout<<"Trying to start CREATOR process ("<<creatorConsole<<")\n";
    if (!CreateProcess(creatorConsole.c_str(), paramsCH, NULL, NULL, FALSE, NULL, NULL, NULL, &creatorStartupInfo, &creatorProcessInfo))
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
