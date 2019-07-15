#include "elDE.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <algorithm>
#include <fstream>
#include <string>
#include <cmath>

void cec17_test_COP(double *x, double *f, double *g,double *h, int nx, int mx,int func_num);

int ng_A[28]={1,1,1,2,2,1,1,1,1,1,1,2,3,1,1,1,1,2,2,2,2,3,1,1,1,1,2,2}; // ng_A denotes the number of inequality constraints of fi
int nh_A[28]={1,1,1,1,1,6,2,2,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}; // nh_A denotes the number of equality constraints of fi
int nr_A[28]={100,100,100,10,10,20,50,100,10,100,100,100,100,100,100,100,100,100,50,100,100,100,100,100,100,100,100,50}; // nr_A denotes the range of fi

extern bool overwrite; extern bool overwrite_f; extern 
bool overwrite_div; extern bool overwrite_sr;

elDE::elDE(int func_num, int dimension, int population_size, double F, double CR, double t_FEs, int seed, std::string method_name)
{
    this->method_name = method_name;
    this->func_num = func_num;
    this->dimension = dimension;
    this->population_size = population_size;
    this->F = F;
    this->CR = CR;
    eps = 0.0001;

    f   = (double *)malloc(sizeof(double)*population_size);
    x   = (double *)malloc(sizeof(double)*dimension*population_size);
    g   = (double *)malloc(sizeof(double)*population_size*ng_A[func_num-1]);
    h   = (double *)malloc(sizeof(double)*population_size*nh_A[func_num-1]);
    phi = (double *)malloc(sizeof(double)*population_size);

    FEs = 0;
    maxFEs = 20000*dimension;
    this->t_FEs  = int( double(maxFEs)* t_FEs );

    srand (seed);

    f_offspring   = (double *)malloc(sizeof(double)*population_size);
    x_offspring   = (double *)malloc(sizeof(double)*dimension*population_size);
    g_offspring   = (double *)malloc(sizeof(double)*population_size*ng_A[func_num-1]);
    h_offspring   = (double *)malloc(sizeof(double)*population_size*nh_A[func_num-1]);
    phi_offspring = (double *)malloc(sizeof(double)*population_size);

    g_best_f = (double *) malloc(sizeof(double)*ng_A[func_num-1]);
    h_best_f = (double *) malloc(sizeof(double)*nh_A[func_num-1]);
    best_x   = (double *) malloc(sizeof(double)*dimension);

    mean_fi  = (double *) malloc(sizeof(double)*4000);
    mean_diversity_i  = (double *) malloc(sizeof(double)*4000);
    SR = (double *) malloc(sizeof(double)*4000);

};

elDE::~elDE()
{
    free(f); free(x); free(g); free(h); free(phi);
    free(f_offspring); free(x_offspring); free(g_offspring); free(h_offspring); free(phi_offspring);
    free(g_best_f); free(h_best_f); free(best_x);
    free(mean_fi); free(mean_diversity_i); free(SR);
};

void elDE::create_population()
{
    for (int  i = 0; i < dimension*population_size; i++)
    {
        x[i] = randUniform()*nr_A[func_num-1];
        if(randUniform() < 0.5)
            x[i] *= -1;
    }
    cec17_test_COP(x,f,g,h,dimension,population_size,func_num);

    for (int  i = 0; i < population_size; i++)
        phi[i] = constraint_violation(i,g,h);

    FEs += population_size;
};

double elDE::constraint_violation(int index, double *g_, double *h_)
{
    double max_g =  0;
    double max_h = -1;
    double max_;

    for (int  j = 0; j < ng_A[func_num-1]; j++)
        max_g = (max_g < g_[ng_A[func_num-1]*index + j]) ? g_[ng_A[func_num-1]*index + j] : max_g;
    for (int j = 0; j < nh_A[func_num-1]; j++)
        max_h = (max_h < std::abs(h_[nh_A[func_num-1]*index+j])) ? std::abs(h_[nh_A[func_num-1]*index+j]) : max_h;
    
    max_h = (max_h <= eps ) ? 0 : max_h;

    max_  = (max_g < max_h) ? max_h : max_g;

    return max_;
};


bool elDE::comparate_interval(double f_x, double f_y, double phi_x, double phi_y)
{
    if( (phi_x <= eps_level and phi_y <= eps_level) or (phi_x == phi_y) )
    {
        if (f_x < f_y)
            return true;
        else
            return false;
    }else
    {
        if(phi_x < phi_y)
            return true;
        else
            return false;
    }
};


void elDE::DE_rand_1_bin(int index, double F_)
{
    int r1, r2, r3, j;
    r1 = (int)(randUniform()*population_size);
    do {r2 = (int)(randUniform()*population_size); } while (r1 ==r2);
    do {r3 = (int)(randUniform()*population_size); } while (r3 ==r1 and r3 ==r2);

    for (int  i = 0; i < dimension; i++)
    {
            x_offspring[dimension*index + i] = x[dimension*r1 + i] + F_*(x[dimension*r2 +i] - x[dimension*r3 +i] );
            check_rank(x_offspring[dimension*index + i]);
    }
};



void elDE::update_eps_level()
{
    if(FEs >= t_FEs)
        eps_level = 0;
    else
    {
        double * aux_phi = (double *)malloc(sizeof(double)*population_size);
        for (int  i = 0; i < population_size; i++)
            aux_phi [i] = phi[i];
        
        std::sort(aux_phi, aux_phi+population_size);
        eps_level = aux_phi[int(0.8*population_size)] * ( 1 - double(FEs)/t_FEs);
        
        free(aux_phi);
    }
};



double elDE::randUniform()
{
    double random  = (double)rand() / (double) RAND_MAX;
    if(random == 1)
        return 0;
    else
        return random;
};

double elDE::randCauchy(double mu, double gamma)
{
    return mu + gamma * tan(M_PI*(randUniform() - 0.5));
};

double elDE::randGauss(double mu, double sigma)
{
    return mu + sigma * sqrt(-2.0 * log(randUniform())) * sin(2.0 * M_PI * randUniform());
};


void elDE::check_rank(double & val)
{
    if(val > nr_A[func_num-1])
        val = nr_A[func_num-1];
    if(val < -nr_A[func_num-1])
        val = -nr_A[func_num-1];
};



void elDE::update_best()
{
    double aux_best_f = best_f;

    for (int  i = 0; i < population_size; i++)
    {
        if(comparate_interval(f[i], best_f, phi[i], phi_))
        {

            best_f = f[i];
            phi_   = phi[i];
            for (int j = 0; j < ng_A[func_num-1]; j++)
                g_best_f[j] = g[ng_A[func_num-1] * i  + j];
            for (int j = 0; j < nh_A[func_num-1]; j++)
                h_best_f[j] = h[nh_A[func_num-1] * i  + j];
            for (int j = 0; j < dimension; j++)
                best_x[j] = x[dimension*i + j];
            
        }
    }

    //if(best_f != aux_best_f)
        //printf("f(x) : %.2f     phi:%.2f     FEs:%d\n", best_f,phi_,FEs);
    
};


void elDE::write()
{
    std::string name = "Result/"+method_name+std::to_string(func_num)+"_"+std::to_string(dimension)+".txt";
    std::ofstream output;
    if(overwrite)
    {
        output.open(name, std::ios_base::app);
        output<<"\n";
    }
    else{
        output.open(name);
        output<<ng_A[func_num-1]<<" "<<nh_A[func_num-1]<<"\n\n";
    }
    
    output<<("f: ");
    output<<(std::to_string(best_f));
    output<<("\ng: ");
    for (int j = 0; j < ng_A[func_num-1]; j++)
        output<<(g_best_f[j])<<" ";
    output<<("\nh: ");
    for (int j = 0; j < nh_A[func_num-1]; j++)
        output<<(h_best_f[j])<<" ";
    output<<("\n\n");    

    output.close();



    name = "Result/Mean f/"+method_name+"_"+std::to_string(func_num)+"_"+std::to_string(dimension)+".txt";

    if(overwrite_f)
    {
        output.open(name, std::ios_base::app);
        output<<"\n";
    }
    else{
        output.open(name);
        output<<4000<<"\n\n";
        overwrite_f = true;
    }

    for (int i = 0; i < 4000; i++)
        output<<"Mean_f"<<i<<": "<<mean_fi[i]<<"\n";
    
    output.close();



    name = "Result/Mean Diversity/"+method_name+"_"+std::to_string(func_num)+"_"+std::to_string(dimension)+".txt";
    
    if(overwrite_div)
    {
        output.open(name, std::ios_base::app);
        output<<"\n";
    }   
    else{
        output.open(name);
        output<<4000<<"\n\n";
        overwrite_div = true;
    }

    for (int i = 0; i < 4000; i++)
        output<<"Mean_Div"<<i<<": "<<mean_diversity_i[i]<<"\n";
    
    output.close();





    name = "Result/SR/"+method_name+"_"+std::to_string(func_num)+"_"+std::to_string(dimension)+".txt";
    
    if(overwrite_sr)
    {
        output.open(name, std::ios_base::app);
        output<<"\n";
    }
    else{
        output.open(name);
        output<<4000<<"\n\n";
        overwrite_sr = true;
    }

    for (int i = 0; i < 4000; i++)
        output<<"SR"<<i<<": "<<SR[i]<<"\n";
    
    output.close();
    
};


void elDE::run()
{
    int k = 0;
    create_population();
    selection(0);
    update_eps_level();
    mean_f(k); mean_diversity(k); sr(k); k++;

    best_f = f[0]; phi_ = phi[0]; 
    update_best();

    while(FEs + population_size <= maxFEs)
    {
        mutation();
        crossover();
        replacement();
        selection(1);
        update_eps_level();
        mean_f(k); mean_diversity(k);sr(k); k++;
        FEs += population_size;
    }
    
    update_best();
    write();
};



void elDE::mutation()
{
    for (int i = 0; i < population_size; i++)
        DE_rand_1_bin(i,F);

    cec17_test_COP(x_offspring,f_offspring,g_offspring,h_offspring,dimension,population_size,func_num);
        for (int  i = 0; i < population_size; i++)
            phi_offspring[i] = constraint_violation(i,g_offspring,h_offspring);
};

void elDE::crossover()
{
    int j;
    for (int  index = 0; index < population_size; index++)
    {
        j = randUniform()*dimension;
        for (int i = 0; i < dimension; i++)
        {
            if(randUniform() > CR and i != j )
                x_offspring[dimension*index + i] = x[dimension*index + i];
        }   
    }    

    cec17_test_COP(x_offspring,f_offspring,g_offspring,h_offspring,dimension,population_size,func_num);
        for (int  i = 0; i < population_size; i++)
            phi_offspring[i] = constraint_violation(i,g_offspring,h_offspring);
};



void elDE::replacement()
{
    for (int i = 0; i < population_size; i++)
    {
        if(!comparate_interval(f[i],f_offspring[i], phi[i],phi_offspring[i]))
        {
            f[i]   =  f_offspring[i];
            phi[i] = phi_offspring[i];
            for (int  j = 0; j < dimension; j++)
                x[dimension*i+j] = x_offspring[dimension*i+j];
            for (int j = 0; j < ng_A[func_num-1]; j++)
                g[ng_A[func_num-1] * i  + j] = g_offspring[ng_A[func_num-1] * i  + j];
            for (int j = 0; j < nh_A[func_num-1]; j++)
                h[nh_A[func_num-1] * i  + j] = h_offspring[nh_A[func_num-1] * i  + j];       
        }
    }
};


void elDE::selection(bool flag) {};


double elDE::distance(double * x_, int A, int B)
{
   double distance = 0.0;
   double term;
   for(int i = 0; i < dimension; i++)
   {
	    term = ( x_[dimension*A + i] - x_[dimension*B + i])/(nr_A[func_num-1] + nr_A[func_num-1]);
        
	    distance += term*term;
   }
   return sqrt(distance/double(dimension));
};


void elDE::mean_f(int k)
{
    double mean = 0;
    for (int i = 0; i < population_size; i++)
        mean += f[i];
    
    mean /= population_size;
    mean_fi[k] = mean;
};

void elDE::mean_diversity(int k)
{
    int index = 0;

    for (int i = 1; i < population_size; i++)
        if (comparate_interval(f[i],f[index],phi[i],phi[index]))
            index = i;
    
    double mean = 0;
    
    for (int i = 0; i < population_size; i++)
    {
        mean += distance(x, index, i);
    }    

    mean /= population_size;

    mean_diversity_i[k] = mean;
    
};


void elDE::sr(int k)
{ 
    double count = 0;
    
    for (int i = 0; i < population_size; i++)
        if(phi[i] <= eps)
        {
            count += 1.0;
        }
    
    count /= population_size;

    SR[k] = count*100;
};