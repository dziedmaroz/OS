#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <cstdio>

using namespace std;

struct order
{
    char name[10];
    int amount;
    double price;
};

int main (int argc, char* argv[])
{
    if (argc!=5)
    {
        cout<<"ERR: wrong parameters list(100)\n";
        return 100;
    }

    char* fbinName = argv[1];
    char* frepName = argv[2];
    int minAmount = atoi(argv[3]);
    int minPrice = atof(argv[4]);
    FILE* fbin = fopen (fbinName,"rb");
    FILE* frep = fopen (frepName,"w");

    if (!fbin)
    {
        cout<<"ERR: file \'"<<fbinName<<"\' not created(110)\n";
        return 110;
    }
    if (!frep)
    {
        cout<<"ERR: file \'"<<frepName<<"\' not created(110)\n";
        return 110;
    }

   fseek (fbin,0,SEEK_END);
   int count = ftell (fbin)/sizeof(order);

   order records [count];
   rewind (fbin);
   fread (records,sizeof(order),count,fbin);
   fclose (fbin);
   fprintf (frep,"REPORT FILE \'%s\'\n\n",fbinName);

   for (int i=0;i<count;i++)
   {
       if (records[i].amount<minAmount &&  records[i].price*records[i].amount<minPrice)
       {
           fprintf (frep,"Product: %s\n",records[i].name);
           fprintf (frep,"Amount: %d\n",records[i].amount);
           fprintf (frep,"Total price: %8.3lf\n",records[i].price*records[i].amount);
           fprintf (frep,"\n\n");
       }
   }
   fclose (frep);

   return 0;
}
