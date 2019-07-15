#include "elDE.h"
#include "elDE_SHADE.h"
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


elDE_SHADE::elDE_SHADE(int func_num, int dimension, int population_size, double t_FES,int seed, std::string method_name) : elDE(func_num, dimension, population_size, 0, 0, t_FES, seed, method_name) {};


void elDE_SHADE::mutation()
{
    //Generate F_i and repair its value
    do {
        F = randCauchy(0.5, 0.5*(double)FEs/maxFEs );
    } while ( F <= 0.0);

    if (F > 1) F = 1.0;

    elDE::mutation();

};


void elDE_SHADE::crossover()
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


void elDE_SHADE::replacement()
{
    elDE::replacement();
};


void elDE_SHADE::selection(bool flag)
{
    elDE::selection(flag);
};