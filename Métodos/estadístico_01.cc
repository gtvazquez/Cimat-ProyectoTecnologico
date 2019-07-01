#include "de.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;

struct datum
{
    double f;
    vector<double> g;
    vector<double> h;
    vector<int> c;
    double v;
};

extern bool overwrite;
double prime [25] = {2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97};
datum data[25];


// Statement of functions
void statistics(int,int,int);
bool compareInterval_1(datum, datum); // Sort feasible solutions in front of infeasible solutions;
bool compareInterval_2(datum, datum); // Sort feasible solutions according to their function values f(x*)


int main(int argc, char const **argv)
{
    overwrite = false;      
    //Ejecutar el método 25 veces
    for (int i = 0; i < 25; i++)
    {
        switch (atoi(argv[1])) 
        {
            case 1: // elDE (epsilon level Diferential Evolution)
            {
                //Argumentos : Número de la función, Dimensión, F, CR, t_FEs
                de * object = new de(atoi(argv[2]),atoi(argv[3]),atoi(argv[3])*5,atof(argv[4]),atof(argv[5]),atof(argv[6]),prime[i], "elDE_");
                object->run();
                delete object;
                overwrite = true;
            }break;

            case 2:
            {

            }break;
        }
    }

    //Obtenemos los estadísticos con los cuales se comparan
    statistics(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]));


    return 0;
}



void statistics(int method, int func_num, int dimension)
{
    ifstream input;
    string name;
    string sub_name;

    switch (method)
    {
    case 1:
        sub_name = "elDE";
        break;
    
    default:
        break;
    }

    name = "Result/25Runs/"+sub_name+"_"+to_string(func_num)+"_"+to_string(dimension);
    input.open(name);

    int len_g, len_h;
    double val;
    string trash;

    input >> len_g >> len_h;

    for (int  i = 0; i < 25; i++)
    {
        datum object;

        input >> trash;
        //input >> object.f;
        input >> data[i].f;

        input >> trash;
        for (int k = 0; k < len_g; k++)
        {
            input >> val;
            data[i].g.push_back(val);
        }

        input >> trash;
        for (int k = 0; k < len_h; k++)
        {
            input >> val;
            data[i].h.push_back(val);
        }

        //data.push_back(object);
    }
    
    input.close();

    // Get c foreach run
    for (int  i = 0; i < 25; i++)
    {
        data[i].c.resize(3);
        for (int k = 0; k < len_g; k++)
        {
            if (data[i].g[k] > 1)
                data[i].c[0] += 1;
            
            if (data[i].g[k] > 0.01 and data[i].g[k] <= 1)
                data[i].c[1] += 1;
            
            if (data[i].g[k] > 0.0001 and data[i].g[k] <= 0.01)
                data[i].c[2] += 1;
            
        }
        for (int k = 0; k < len_h; k++)
        {
            if (abs(data[i].h[k]) > 1)
                data[i].c[0] += 1;
            
            if (abs(data[i].h[k]) > 0.01 and abs(data[i].h[k]) <= 1)
                data[i].c[1] += 1;
            
            if (abs(data[i].h[k]) > 0.0001 and abs(data[i].h[k]) <= 0.01)
                data[i].c[2] += 1;
        }
        
        
    }
    
    //Get v for each run
    for (int i = 0; i < 25; i++)
    {
        data[i].v = 0.0;
        
        for (int k = 0; k < len_g; k++)
            if(data[i].g[k] > 0 )
                data[i].v += data[i].g[k];

        for (int k = 0; k < len_h; k++)
            if(abs(data[i].h[k]) - 0.0001 > 0 )
                data[i].v += abs(data[i].h[k]);
        
        data[i].v /= (len_g+len_h);
    }


    sort(data,data+25, compareInterval_1);
    int index = 0;
    while(index < 25 and data[index].v == 0)
        index++;
    sort(data,data+index, compareInterval_2);

    double mean = 0;
    double worst;
    double best;
    double SR = (index/24) * 100;
    double vio = 0;  
    double sd = 0;
 

    // Get mean and vio
    for (int i = 0; i < 25; i++)
    {
        mean += data[i].f;
        vio  += data[i].v;
    }
    
    mean /= 25;
    vio  /= 25;


    // Get best and worst
    worst = data[24].f;
    best  = data[0].f;

    // Get SD
    for (int i = 0; i < 25; i++)
        sd += pow(data[i].f - mean , 2);
    sd = sqrt(sd/25);


    ofstream output;
    string name_file = "Result/25Runs/Table/" + sub_name + "_" + to_string(func_num) + "_" + to_string(dimension);
    output.open(name_file);


    output<<"Best:   "<<best<<"\n";
    output<<"Median: "<<data[12].f<<"\n";
    output<<"c:      ["<<data[12].c[0]<<","<<data[12].c[1]<<","<<data[12].c[2]<<"]\n";
    output<<"v:      "<<data[12].v<<"\n";
    output<<"Mean:   "<<mean<<"\n";
    output<<"Worst:  "<<worst<<"\n";
    output<<"STD:    "<<sd<<"\n";\
    output<<"SR:     "<<SR<<"\n";
    output<<"vio:    "<<vio<<"\n";     

    output.close();

};


bool compareInterval_1(datum i1, datum i2)
{
    return(i1.v < i2.v);
};

bool compareInterval_2(datum i1, datum i2)
{
    return(i1.f < i2.f);
};