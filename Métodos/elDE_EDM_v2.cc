#include "elDE.h"
#include "elDE_EDM_v2.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <algorithm>
#include <fstream>
#include <string>
#include <cmath>


void cec17_test_COP(double *x, double *f, double *g,double *h, int nx, int mx,int func_num);

extern int ng_A [28]; extern int nh_A[28]; extern int nr_A[28];


elDE_EDM_v2::elDE_EDM_v2(int func_num, int dimension, int population_size, double t_FES,int seed, std::string method_name) : elDE(func_num, dimension, population_size, 0, 0, t_FES, seed, method_name)
{
    f_elite   = (double *)malloc(sizeof(double)*population_size);
    elite     = (double *)malloc(sizeof(double)*dimension*population_size);
    g_elite   = (double *)malloc(sizeof(double)*population_size*ng_A[func_num-1]);
    h_elite   = (double *)malloc(sizeof(double)*population_size*nh_A[func_num-1]);
    phi_elite = (double *)malloc(sizeof(double)*population_size);


    f_current   = (double *)malloc(sizeof(double)*population_size*3);
    current     = (double *)malloc(sizeof(double)*dimension*population_size*3);
    g_current   = (double *)malloc(sizeof(double)*population_size*ng_A[func_num-1]*3);
    h_current   = (double *)malloc(sizeof(double)*population_size*nh_A[func_num-1]*3);
    phi_current = (double *)malloc(sizeof(double)*population_size*3);
};




elDE_EDM_v2::~elDE_EDM_v2()
{
    free(f_current); free(current); free(g_current);
    free(h_current); free(phi_current);

    free(f_elite); free(elite); free(g_elite);
    free(h_elite); free(phi_elite);
};




void elDE_EDM_v2::mutation()
{
    //Generate F_i and repair its value
    do {
        F = randCauchy(0.5, 0.5*(double)FEs/maxFEs);
    } while ( F <= 0.0);

    if (F > 1) F = 1.0;

    elDE::mutation();

};


void elDE_EDM_v2::crossover()
{
    int j;
    //Generate CR_i and repair its value
     if(randUniform() > 0.5)
            CR =  randGauss(0.1, 0.1 );
          else
            CR =   randGauss(0.9, 0.1 );

          if (CR > 1) CR = 1;
          else if (CR < 0) CR = 0;

    elDE::crossover();

};














void elDE_EDM_v2::selection(bool flag)
{
    if(!flag) // Elite set Population and initialize Do
    {
        for (int i = 0; i < population_size; i++)
        {
            for (int j = 0; j < dimension; j++)
                elite[dimension*i + j] = x[dimension*i + j];
            
            f_elite [i] = f [i];
            phi_elite[i] = phi[i];

            for (int j = 0; j < ng_A[func_num-1]; j++)
                g_elite[ng_A[func_num-1] * i  + j] = g[ng_A[func_num-1] * i  + j];
            for (int j = 0; j < nh_A[func_num-1]; j++)
                h_elite[nh_A[func_num-1] * i  + j] = h[nh_A[func_num-1] * i  + j];

        }


        mean_diversity(0);
        Di = mean_diversity_i[0];
        Dt = Di;
        
    }
    else 
    {
        for (int i = 0; i < population_size; i++)
        {
            if(!comparate_interval(f_offspring[i],f_elite[i], phi_offspring[i],phi_elite[i]))
            {
                f_elite[i]   =  f_offspring[i];
                phi_elite[i] = phi_offspring[i];
                for (int  j = 0; j < dimension; j++)
                    elite[dimension*i+j] = x_offspring[dimension*i+j];
                for (int j = 0; j < ng_A[func_num-1]; j++)
                    g_elite[ng_A[func_num-1] * i  + j] = g_offspring[ng_A[func_num-1] * i  + j];
                for (int j = 0; j < nh_A[func_num-1]; j++)
                    h_elite[nh_A[func_num-1] * i  + j] = h_offspring[nh_A[func_num-1] * i  + j];       
            }
        }
    }
    
};


void elDE_EDM_v2::updateDt()
{
    double cte = std::pow(0.1/Di, 1.0/2000.0);

    if(FEs < 2000)
        Dt = Dt * cte;
    else
        Dt= Dt * std::pow(0.001,1.0/2000.0);

};


void elDE_EDM_v2::replacement()
{
    updateDt();
    for (int i = 0; i < population_size * 3; i++)
        list.push_back(i);


    int index, realIndex;
    setUnionToCurrent();

    while(selected.size() < population_size && list.size() > 0 )
    {
        index = 0;
        for (int i = 1; i < list.size(); i++)
            if (comparate_interval(f_current[list[i]], f_current[list[index]], phi_current[list[i]], phi_current[list[index]]))
                index  = i;
        selected.push_back(list[index]); realIndex = list[index];    
        list.erase(list.begin() + index);

        for (int i = 0; i < list.size(); i++)
        {
            if(distance(current, realIndex, list[i]) < Dt )
            {
                penalized.push_back(list[i]);
                list.erase(list.begin() + i );
                i--;
            }
        }
        
    }


    std::vector <double> distance_i(penalized.size(), -1);
    double indexBest, length;

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

    selected.clear(); penalized.clear(); list.clear();
    
};

void elDE_EDM_v2::setUnionToCurrent()
{
    //Copy Target
    for (int i = 0; i < population_size*dimension; i++) current[i] = x[i];
    for (int i = 0; i < population_size; i++) f_current[i] = f[i];
    for (int i = 0; i < population_size; i++) phi_current[i] = phi[i];
    for (int i = 0; i < ng_A[func_num-1]*population_size; i++) g_current[i] = g[i];
    for (int i = 0; i < nh_A[func_num-1]*population_size; i++) h_current[i] = h[i];
    
    //Copy Trial
    for (int i = 0; i < population_size*dimension; i++) current[population_size*dimension+i] = x_offspring[i];
    for (int i = 0; i < population_size; i++) f_current[population_size+i] = f_offspring[i];
    for (int i = 0; i < population_size; i++) phi_current[population_size+i] = phi_offspring[i];
    for (int i = 0; i < ng_A[func_num-1]*population_size; i++) g_current[ng_A[func_num-1]*population_size+i] = g_offspring[i];
    for (int i = 0; i < nh_A[func_num-1]*population_size; i++) h_current[nh_A[func_num-1]*population_size+i] = h_offspring[i];
    
    
    //Copy Elite
    for (int i = 0; i < population_size*dimension; i++) current[2*population_size*dimension+i] = elite[i];
    for (int i = 0; i < population_size; i++) f_current[2*population_size+i] = f_elite[i];
    for (int i = 0; i < population_size; i++) phi_current[2*population_size+i] = phi_elite[i];
    for (int i = 0; i < ng_A[func_num-1]*population_size; i++) g_current[2*ng_A[func_num-1]*population_size+i] = g_elite[i];
    for (int i = 0; i < nh_A[func_num-1]*population_size; i++) h_current[2*nh_A[func_num-1]*population_size+i] = h_elite[i];
};