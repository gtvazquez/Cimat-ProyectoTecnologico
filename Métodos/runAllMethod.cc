#include <string>
#include <cstdlib>
#include <iostream>
#include <omp.h>

using namespace std;

int main(int argc, char const **argv)
{
    string command;
    string method = argv[1];
    string dimension = argv[2];
    string F;
    string CR;
    string tFEs;

    if(method == "1"){ F = argv[3]; CR = argv[4]; tFEs = argv[5];}
    else { tFEs = argv[3]; }

    #pragma omp parallel for
    for (int i = 1; i < 29; i++)
    {
        if(method == "1")
            command = "./a.out " + method + " "  + to_string(i) +" "+ dimension + " " + F + " " + CR + " " + tFEs;
        else
            command = "./a.out " + method + " "  + to_string(i) +" "+ dimension + " " + tFEs;
        system(command.c_str());
    }
        

    return 0;
}

