#include <string>
#include <cstdlib>
#include <iostream>
#include <omp.h>

using namespace std;

int main(int argc, char const **argv)
{
    string method = argv[1];
    string dimension = argv[2];
    string F;
    string CR;
    string tFEs;
    string clusterSize;

    if(method == "1"){ F = argv[3]; CR = argv[4]; tFEs = argv[5];}
    else { tFEs = argv[3];  if (method == "5")  clusterSize = argv[4];  }

    #pragma omp parallel for
    for (int i = 1; i < 29; i++)
    {
        string command;
        if(method == "1")
            command = "./executable " + method + " "  + to_string(i) +" "+ dimension + " " + F + " " + CR + " " + tFEs;
        else{
            if (method == "5" )
                command = "./executable " + method + " "  + to_string(i) +" "+ dimension + " " + tFEs+ " "+ clusterSize;
            else
            {
                command = "./executable " + method + " "  + to_string(i) +" "+ dimension + " " + tFEs;
            }
        }
            
        bool flag = system(command.c_str());
    }
        

    return 0;
}

