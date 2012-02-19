#include <iostream>
#include <cstdio>
using namespace std;

struct order
{
    char name[10];
    int amount;
    double price;
};

int main (int argc, char*  argv [])
{
    if (argc==1)
    {
        cout<<"ERR: no parameters(100)\n";
        return 100;
    }
    FILE* fout = fopen (argv[1],"wb");
    if (!fout)
    {
        cout<<"ERR: file \'"<<argv[1]<<"\' not created(110)\n";
        return 110;
    }
    int count =0;
    cout<<"# of records:";
    cin>>count;
    order tmp [count];
    cout<<"_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_\n";
    for (int i=0;i<count;i++)
    {
        cout<<"#"<<i+1<<"  == == == == == == ==\n";
        cout<<"Product:";
        cin>>tmp[i].name;
        cout<<"Amount:";
        cin>>tmp[i].amount;
        cout<<"Price:";
        cin>>tmp[i].price;
    }

    fwrite(&tmp,1,sizeof(order)*count,fout);
    fclose(fout);
    return 0;
}
