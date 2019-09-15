#include "elDE.h"
#include "elDE_EDM_v2.h"
#include "elDE_EDM_C.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <algorithm>
#include <fstream>
#include <string>
#include <cmath>
#include <iostream>

void cec17_test_COP(double *x, double *f, double *g,double *h, int nx, int mx,int func_num);

extern int ng_A [28]; extern int nh_A[28]; extern int nr_A[28];
extern double cte;
extern int kEval;

elDE_EDM_C::elDE_EDM_C(int func_num, int dimension, int population_size, int clusterSize ,double t_FES,int seed, std::string method_name) : elDE_EDM_v2(func_num, dimension, population_size, t_FES, seed, method_name)
{
    this->clusterSize = clusterSize;
};


void elDE_EDM_C::mutation()
{
    elDE_EDM_v2::mutation();
};

void elDE_EDM_C::crossover()
{
    elDE_EDM_v2::crossover();
};

void elDE_EDM_C::selection(bool flag)
{
    elDE_EDM_v2::selection(flag);
};

void elDE_EDM_C::replacement()
{
    bool flag;
    elDE_EDM_v2::updateDt();
    for (int i = 0; i < population_size * 3; i++)
        list.push_back(i);

    int index, realIndex;
    elDE_EDM_v2::setUnionToCurrent();

    int size_selected = 0;

    while(size_selected < population_size && list.size() > 0 )
    {
        flag = true;
        index = 0;

        for (int i = 1; i < list.size(); i++)
            if (comparate_interval(f_current[list[i]], f_current[list[index]], phi_current[list[i]], phi_current[list[index]]))
                index  = i;
        

        realIndex = list[index];  /* std::cout << "Index : " << realIndex << "\n\n"; */
        list.erase(list.begin() + index);

        // We penalize minors who Dt/10 //
        for (int i = 0; i < list.size(); i++)
        {
            if(distance(current, realIndex, list[i]) < Dt/10.0 )
            {
                penalized.push_back(list[i]);
                /* std::cout << "Dt/10  Penalizado: " << list[i] << std::endl; */
                list.erase(list.begin() + i );
                i--;
            }
        }



        // Insert the selected one with its respective cluster //
        for (int u = 0 ; u < selected_.size(); u++ )
        {
            for(int v = 0; v < selected_[u].size() && selected_[u].size() < clusterSize; v++)
            {
                if (distance(current, realIndex, selected_[u][v] ) < Dt  )
                {
                    selected_[u].push_back(realIndex); flag = false; // Insert //
                    
                    if(selected_[u].size() == clusterSize) // we penalize the remaining ones in cluster //
                    {
                        for (int t = 0; t < list.size(); t++)
                        {
                            if(distance(current, realIndex, list[t]) < Dt )
                            {
                                penalized.push_back(list[t]);
                                /* std::cout << "Dt  Penalizado: " << list[t] << std::endl; */
                                list.erase(list.begin() + t);
                                t --;
                            }
                        }
                    }
                    v = clusterSize;
                    u = selected_.size();
                }
            }
        }

        if(flag)
        {
            std:: vector <int> new_cluster; new_cluster.push_back(realIndex);
            selected_.push_back (new_cluster);
        }
        /* std::cout  << std::endl; */

        size_selected ++;
        /*
        for (int u = 0; u < selected_.size(); u++)
        {
            for (int v = 0; v < selected_[u].size(); v++)
            {
                std::cout << selected_[u][v]<< " ";
            }std::cout << "\n";
        }std::cout << "\n\n\n"; getchar();
        */
    }
    
    

    //std::cout<< "4 "<< selected_.size() << "\n"; getchar();

    for (int  i = 0; i < selected_.size(); i++)
    {
        for (int j = 0; j < selected_[i].size(); j++)
            selected.push_back(selected_[i][j]);
    }
    selected_.clear();  // Imprimir el tamaño de los cluster
    
    //std::cout<< "5 "<< selected.size() << "\n"; getchar();


    std::vector <double> distance_i(penalized.size(), -1);
    double indexBest, length;

    /* std::cout<< "6 "<< selected.size() << "\n"; */
    while (selected.size() < population_size)
    {

        for (int i = 0; i < penalized.size(); i++)
        {
            distance_i [i] = distance(current, penalized[i], selected[0]); 
            for (int j = 1; j < selected.size(); j++)
                if(distance(current, penalized[i], selected[j]) <  distance_i[i])
                    distance_i [i] = distance(current, penalized[i], selected[j]);
        }


        index = 0;
        length = penalized.size();

        for (int i = 0; i < length; i++)
            if(distance_i[i] > distance_i[index])
                index = i;
        
        selected.push_back(penalized[index]);
        penalized.erase(penalized.begin() + index);

    }

    // Copy Selected to Xg+1
    for (int i = 0; i < population_size; i++)
    {
        f[i] = f_current[selected[i]];
        phi[i] = phi_current[selected[i]];

        for (int j = 0; j < dimension; j++)
            x[dimension*i + j] = current[dimension*selected[i] + j];
        for (int j = 0; j < ng_A[func_num-1]; j++)
            g[ng_A[func_num-1] * i  + j] = g_current[ng_A[func_num-1] * selected[i] + j];
        for (int j = 0; j < nh_A[func_num-1]; j++)
            h[nh_A[func_num-1] * i  + j] = h_current[nh_A[func_num-1] * selected[i] + j];
    }
    //printf ("7\n");
    selected.clear(); penalized.clear(); list.clear();

};