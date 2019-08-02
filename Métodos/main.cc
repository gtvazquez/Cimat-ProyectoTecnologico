#include "elDE.h"
#include "elDE_SHADE.h"
#include "elDE_EDM.h"
#include "elDE_EDM_v2.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <unistd.h>

#define runs 25

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
double prime [runs] = {2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97};
static datum data[runs];


// Statement of functions
void statistics(int,int,int);
void graphs(int,int,int);
std::string get_name(int);
bool compareInterval_1(datum, datum); // Sort feasible solutions in front of infeasible solutions;
bool compareInterval_2(datum, datum); // Sort feasible solutions according to their function values f(x*)


int main(int argc, char const **argv)
{
    overwrite = false;      
    //Ejecutar el método runs veces
    for (int i = 0; i < runs; i++)
    {
        switch (atoi(argv[1])) 
        {
            case 1: // elDE (epsilon level Diferential Evolution)
            {
                //Argumentos : Número de la función, Dimensión, F, CR, t_FEs
                elDE * object = new elDE(atoi(argv[2]),atoi(argv[3]),atoi(argv[3])*5,atof(argv[4]),atof(argv[5]),atof(argv[6]),prime[i], get_name(atoi(argv[1])));
                object->run();
                delete object;
                overwrite = true;
            }break;

            case 2:
            {
                //Argumentos : Número de la función, Dimensión, F, CR, t_FEs
                elDE * object = new elDE_SHADE(atoi(argv[2]),atoi(argv[3]),atoi(argv[3])*5,atof(argv[4]),prime[i], get_name(atoi(argv[1])));
                object->run();
                delete object;
                overwrite = true;
            }break;


            case 3:
            {
                //Argumentos : Número de la función, Dimensión, t_FEs
                elDE * object = new elDE_EDM(atoi(argv[2]),atoi(argv[3]),atoi(argv[3])*5,atof(argv[4]),prime[i], get_name(atoi(argv[1])));
                object->run();
                delete object;
                overwrite = true;
            }break;

            case 4:
                //Argumentos : Número de la función, Dimensión, t_FEs
                elDE * object = new elDE_EDM_v2(atoi(argv[2]),atoi(argv[3]),atoi(argv[3])*5,atof(argv[4]),prime[i], get_name(atoi(argv[1])));
                object->run();
                delete object;
                overwrite = true;
        }
    }
    //Obtenemos los estadísticos con los cuales se comparan
    statistics(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]));
    graphs(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]));

    return 0;
}



void statistics(int method, int func_num, int dimension)
{
    ifstream input;
    string name;
    string sub_name;

    sub_name = get_name(method);

    name = "Result/"+sub_name+"_"+to_string(func_num)+"_"+to_string(dimension)+".txt";
    input.open(name);
    int len_g, len_h;
    double val;
    string trash;

    input >> len_g >> len_h;
    
    for (int  i = 0; i < runs; i++)
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
    for (int  i = 0; i < runs; i++)
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
    for (int i = 0; i < runs; i++)
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


    sort(data,data+runs, compareInterval_1);
    int index = 0;
    while(index < runs and data[index].v == 0)
        index++;
    sort(data,data+index, compareInterval_2);

    double mean = 0;
    double worst;
    double best;
    double SR = (double(index)/runs) * 100;
    double vio = 0;  
    double sd = 0;
 
    // Get mean and vio
    for (int i = 0; i < runs; i++)
    {
        mean += data[i].f;
        vio  += data[i].v;
    }
    
    mean /= runs;
    vio  /= runs;


    // Get best and worst
    worst = data[runs-1].f;
    best  = data[0].f;

    // Get SD
    for (int i = 0; i < runs; i++)
        sd += pow(data[i].f - mean , 2);
    sd = sqrt(sd/runs);

    ofstream output;
    string name_file = "Result/Table/" + sub_name + "_" + to_string(func_num) + "_" + to_string(dimension)+".txt";
    output.open(name_file);


    output<<"Best:   "<<best<<"\n";
    output<<"Median: "<<data[runs/2].f<<"\n";
    output<<"c:      ["<<data[runs/2].c[0]<<","<<data[runs/2].c[1]<<","<<data[runs/2].c[2]<<"]\n";
    output<<"v:      "<<data[runs/2].v<<"\n";
    output<<"Mean:   "<<mean<<"\n";
    output<<"Worst:  "<<worst<<"\n";
    output<<"STD:    "<<sd<<"\n";\
    output<<"SR:     "<<SR<<"\n";
    output<<"vio:    "<<vio<<"\n";     

    output.close();

    name = "rm -f "+ name;
    bool erase = system(name.c_str());
};



void graphs(int method, int func_num, int dimension)
{
    ifstream input;
    string name;
    string sub_name;
    bool erase;

    sub_name = get_name(method);
    

    // Mean Mean f
    name = "Result/Mean f/"+sub_name+"_"+to_string(func_num)+"_"+to_string(dimension)+".txt";
    input.open(name);

    int length; input >> length;
    double *mean_f = (double *)malloc(sizeof(double)*length); for (int i = 0; i < length; i++) mean_f[i] = 0;
    double value; std::string trash;

    for (int i = 0; i < runs; i++)
    {
        for (int j = 0; j < length; j++)
        {
            input >> trash >>value; 
            mean_f[j] += value;
        }   
    }




    for (int i = 0; i < length; i++)
        mean_f[i] /= (runs*1.0);
    
    input.close();

    name = "Result/Mean f/Resumen_"+sub_name+"_"+to_string(func_num)+"_"+to_string(dimension)+".txt";
    ofstream output;
    output.open(name);
    
    for (int  i = 0; i < length; i++)
        output<<"Mean_f"<<i<<": "<<mean_f[i]<<"\n";

    output.close();
    free(mean_f);


    name = "rm -f Result/Mean\\ f/"+sub_name+"_"+to_string(func_num)+"_"+to_string(dimension)+".txt";
    erase = system(name.c_str());

    // Mean Mean Diversity
    name = "Result/Mean Diversity/"+sub_name+"_"+to_string(func_num)+"_"+to_string(dimension)+".txt";
    input.open(name);

    input >> length;
    double *mean_div = (double *)malloc(sizeof(double)*length); for (int i = 0; i < length; i++) mean_div[i] = 0;

    for (int i = 0; i < runs; i++)
    {
        for (int j = 0; j < length; j++)
        {
            input >> trash >>value; 
            mean_div[j] += value;
        }   
    }

    for (int i = 0; i < length; i++)
        mean_div[i] /= (runs*1.0);
    
    input.close();

    name = "Result/Mean Diversity/Resumen_"+sub_name+"_"+to_string(func_num)+"_"+to_string(dimension)+".txt";
    output.open(name);
    
    for (int  i = 0; i < length; i++)
        output<<"Mean_Div"<<i<<": "<<mean_div[i]<<"\n";

    output.close();
    free(mean_div);

    name = "rm -f Result/Mean\\ Diversity/"+sub_name+"_"+to_string(func_num)+"_"+to_string(dimension)+".txt";
    erase = system(name.c_str());




    // Mean SR
    name = "Result/SR/"+sub_name+"_"+to_string(func_num)+"_"+to_string(dimension)+".txt";
    input.open(name);

    input >> length;
    double *mean_sr = (double *)malloc(sizeof(double)*length); for (int i = 0; i < length; i++) mean_sr[i] = 0;

    for (int i = 0; i < runs; i++)
    {
        for (int j = 0; j < length; j++)
        {
            input >> trash >>value; 
            mean_sr[j] += value;
        }   
    }

    for (int i = 0; i < length; i++)
        mean_sr[i] /= (runs*1.0);

    input.close();

    name = "Result/SR/Resumen_"+sub_name+"_"+to_string(func_num)+"_"+to_string(dimension)+".txt";
    output.open(name);

    for (int  i = 0; i < length; i++)
        output<<"Mean_SR"<<i<<": "<<mean_sr[i]<<"\n";

    output.close();
    free(mean_sr);

    name = "rm -f Result/SR/"+sub_name+"_"+to_string(func_num)+"_"+to_string(dimension)+".txt";
    erase = system(name.c_str());




    // Mean SD
    name = "Result/SD/"+sub_name+"_"+to_string(func_num)+"_"+to_string(dimension)+".txt";
    input.open(name);

    input >> length;
    double *mean_sd = (double *)malloc(sizeof(double)*length*dimension); for (int i = 0; i < length*dimension; i++) mean_sd[i] = 0;

    for (int i = 0; i < runs; i++)
    {
        for (int j = 0; j < length; j++)
        {
            input >> trash;
            for (int k = 0; k < dimension; k++)
            {
                input >> trash >> value;
                mean_sd[dimension*j+k] += value;
            }   
        }
    }

    for (int i = 0; i < length*dimension; i++)
        mean_sd [i] /= runs;
    
    input.close();
    

    name = "Result/SD/Resumen_"+sub_name+"_"+to_string(func_num)+"_"+to_string(dimension)+".txt";
    output.open(name);

    for (int  i = 0; i < length; i++)
    {
        output<<"Mean_SD"<<i<<"   ";
        for (int j = 0; j < dimension; j++)
            output<<"x"<<j<<":  "<<mean_sd[dimension*i+j]<<"  ";    
        output<<"\n";
    }

    output.close();
    free(mean_sd);

    name = "rm -f Result/SD/"+sub_name+"_"+to_string(func_num)+"_"+to_string(dimension)+".txt";
    erase = system(name.c_str());


};

std::string get_name(int method)
{
    string list[] = {"elDE","elDE_SHADE","elDE_EDM", "elDE_EDM_v2" };
    return list[method -1];
};



bool compareInterval_1(datum i1, datum i2)
{
    return(i1.v < i2.v);
};

bool compareInterval_2(datum i1, datum i2)
{
    return(i1.f < i2.f);
};