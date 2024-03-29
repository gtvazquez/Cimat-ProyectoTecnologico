#include "elDE.h"
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
using namespace std;

int ng_A[28]={1,1,1,2,2,1,1,1,1,1,1,2,3,1,1,1,1,2,2,2,2,3,1,1,1,1,2,2}; // ng_A denotes the number of inequality constraints of fi
int nh_A[28]={1,1,1,1,1,6,2,2,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}; // nh_A denotes the number of equality constraints of fi
int nr_A[28]={100,100,100,10,10,20,50,100,10,100,100,100,100,100,100,100,100,100,50,100,100,100,100,100,100,100,100,50}; // nr_A denotes the range of fi

extern bool overwrite; extern bool overwrite_f;
extern bool overwrite_div; extern bool overwrite_sr; extern bool overwrite_sd; 
extern int kEval;
extern bool CR_s;

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
    maxFEs = kEval*dimension;
    iterations = maxFEs / population_size;
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

    mean_fi  = (double *) malloc(sizeof(double)*iterations);
    mean_diversity_i  = (double *) malloc(sizeof(double)*iterations);
    sd_i = (double *) malloc(sizeof(double)*iterations*dimension);
    SR = (double *) malloc(sizeof(double)*iterations);

};

elDE::~elDE()
{
    free(f); free(x); free(g); free(h); free(phi);
    free(f_offspring); free(x_offspring); free(g_offspring); free(h_offspring); free(phi_offspring);
    free(g_best_f); free(h_best_f); free(best_x);
    free(mean_fi); free(mean_diversity_i); free(SR); free(sd_i);
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
    double max_h =  0;
    double max_;

    for (int  j = 0; j < ng_A[func_num-1]; j++){
        //max_g = (max_g < g_[ng_A[func_num-1]*index + j]) ? g_[ng_A[func_num-1]*index + j] : max_g;
        if(g_[ng_A[func_num-1]*index + j] > 0)
            max_g += g_[ng_A[func_num-1]*index + j];
    }
    for (int j = 0; j < nh_A[func_num-1]; j++){
        //max_h = (max_h < std::abs(h_[nh_A[func_num-1]*index+j])) ? std::abs(h_[nh_A[func_num-1]*index+j]) : max_h;
        max_h += std::abs(h_[nh_A[func_num-1]*index+j]);
    }
    
    //max_h = (max_h <= eps ) ? 0 : max_h;

    //max_  = (max_g < max_h) ? max_h : max_g;
    max_ = max_g + max_h;

    return max_;
};


bool elDE::comparate_interval(double f_x, double f_y, double phi_x, double phi_y)
{
    if( (phi_x <= eps_level and phi_y <= eps_level) or (phi_x == phi_y) )
    {
        if (f_x <= f_y)
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
    do {r1 = (int)(randUniform()*population_size); } while(r1 == index);
    do {r2 = (int)(randUniform()*population_size); } while (r1 == r2 or r2 == index);
    do {r3 = (int)(randUniform()*population_size); } while (r3 == r1 or r3 ==r2  or r3 == index);

    for (int  i = 0; i < dimension; i++)
    {
            x_offspring[dimension*index + i] = x[dimension*r1 + i] + F_*(x[dimension*r2 +i] - x[dimension*r3 +i]);
            check_rank(x_offspring[dimension*index + i]);
            if(CR_s)
                std::cout << F_*(x[dimension*r2 +i] - x[dimension*r3 +i] ) << " ";
    }
    if(CR_s)
        std::cout << "\n";
};


double init_eps_level;
void elDE::update_eps_level()
{
    if(FEs > t_FEs)
        eps_level = 0;
    else
    {
        if (FEs == population_size  || (FEs%(100*population_size) == 0)){
            double * aux_phi = (double *)malloc(sizeof(double)*population_size);
            for (int  i = 0; i < population_size; i++)
              aux_phi [i] = phi[i];
        
            std::sort(aux_phi, aux_phi+population_size);
            init_eps_level =  eps_level = aux_phi[int(0.8*population_size)];
            std::cout << "Init ips level: " << init_eps_level<< "\n";
            free(aux_phi);
        }else
        {
            eps_level = init_eps_level *  std::pow(( 1 - double(FEs)/t_FEs), 5 );
        }
    }
};



/*

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
        eps_level = aux_phi[int(0.8*population_size)] *  std::pow(( 1 - double(FEs)/t_FEs), 1 );
        
        free(aux_phi);
    }
}
*/


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
    
};


void elDE::write()
{
    std::string name = "Result/"+method_name+"_"+std::to_string(func_num)+"_"+std::to_string(dimension)+".txt";
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
        output<<iterations<<"\n\n";
        overwrite_f = true;
    }

    for (int i = 0; i < iterations; i++)
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
        output<<iterations<<"\n\n";
        overwrite_div = true;
    }

    for (int i = 0; i < iterations; i++)
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
        output<<iterations<<"\n\n";
        overwrite_sr = true;
    }

    for (int i = 0; i < iterations; i++)
        output<<"SR"<<i<<": "<<SR[i]<<"\n";
    
    output.close();



    name = "Result/SD/" + method_name + "_"+std::to_string(func_num)+"_"+std::to_string(dimension)+".txt";
    if(overwrite_sd)
    {
        output.open(name, std::ios_base::app);
        output<<"\n";
    }
    else{
        output.open(name);
        output<<iterations<<"\n\n";
        overwrite_sd = true;
    }

    for (int i = 0; i < iterations; i++)
    {
        output<<"SD"<<i<<"\t";
        for (int j = 0; j < dimension; j++)
            output<<"x"<<j<<": "<<sd_i[dimension*i+j]<<"  ";
        output<<"\n";
    }
    output.close();

        
};



void elDE::run()
{
    int k = 0;
    create_population();
    selection(0);
    update_eps_level();
    mean_f(k); mean_diversity(k); sr(k);sd(k); k++;

    best_f = f[0]; phi_ = phi[0]; 
    update_best();
    while(FEs + population_size <= maxFEs)
    {
        FEs += population_size;
        mutation();
        crossover();
        replacement();
        selection(1);
        update_eps_level();
        mean_f(k); mean_diversity(k);sr(k); sd(k); k++;
        //if(FEs > maxFEs*0.995)
        //    printPopulation();
        //std:: cout << eps_level << "\n";
    }
    update_best();
    cout<< "\n";
    for (int i = 0; i < dimension; i++)
       cout<< best_x[i]<< " ";
    cout<< "    f: "<< best_f<< "   phi:"<< phi_<< "\n";
    cout<< "\n\n";
    
    write();
    //getchar();
};



void elDE::mutation()
{
    for (int i = 0; i < population_size; i++)
    {
        //Generate F_i and repair its value
        do {
             F = randCauchy(0.5, 0.5*(double)FEs/maxFEs);
        } while ( F <= 0.0);

        if (F > 1) F = 1.0;
        DE_rand_1_bin(i,F);
    }
};


void elDE::crossover()
{
    int j;


    for (int  index = 0; index < population_size; index++)
    {
            //Generate CR_i and repair its value
            if(randUniform() > 0.5)
                  CR =  randGauss(0.1, 0.1 );
             else
                  CR =   randGauss(0.9, 0.1 );

            if (CR > 1){ CR = 1;  CR_s = true;}
                else if (CR < 0) CR = 0;

        j = randUniform()*dimension;
        for (int i = 0; i < dimension; i++)
        {
            if(randUniform() > CR and i != j )
                x_offspring[dimension*index + i] = polynomial_mutation(x[dimension*index + i]); // x[dimension*index + i];
            
            else // Mutación Polinomial
            {
                x_offspring[dimension*index + i] = polynomial_mutation (x_offspring[dimension*index + i] );
            }
            //    if( abs(x_offspring[dimension*index + i] - x[dimension*index + i]) < 1e-12 )
            //       x_offspring[dimension*index + i] = polynomial_mutation(x_offspring[dimension*index + i]);
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




double elDE::polynomial_mutation(double value)
{
    double eta_m = 10000000.0, rnd, delta1, delta2, mut_pow, deltaq;
    double y, yl, yu, val, xy;
    double temp;   

    y = value;
    temp = y;
    yl = -nr_A[func_num-1];
    yu = nr_A[func_num-1];
    delta1 = (y-yl)/(yu-yl);
    delta2 = (yu-y)/(yu-yl);
    rnd = randUniform();
    mut_pow = 1.0/(eta_m+1.0);
    if (rnd <= 0.5)
    {
        xy = 1.0-delta1;
        val = 2.0*rnd+(1.0-2.0*rnd)*(pow(xy,(eta_m+1.0)));
        deltaq =  pow(val,mut_pow) - 1.0;
    }
    else
    {
        xy = 1.0-delta2;
        val = 2.0*(1.0-rnd)+2.0*(rnd-0.5)*(pow(xy,(eta_m+1.0)));
        deltaq = 1.0 - (pow(val,mut_pow));
    }
    y = y + deltaq*(yu-yl);
    if (y<yl)
        y = yl;
    if (y>yu)
        y = yu;

    //cout<< abs(temp - y) << "\n";

    return y;
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
    double * aux_diversity = (double *)malloc(sizeof(double)*population_size);
    double   prev_diversity;
    for (int i = 0; i < population_size; i++)
        aux_diversity [i] = 1;

    for (int i = 0; i < population_size; i++)
    {
        for (int j = i+1; j < population_size; j++)
        {
            prev_diversity = distance(x, i, j);
            if(prev_diversity < aux_diversity[i])
            {
                aux_diversity [i] = aux_diversity [j] = prev_diversity;
            }
        }    
    }
       
    double mean = 0;
    for (int i = 0; i < population_size; i++)
        mean += aux_diversity[i];
        
    mean /= population_size;

    mean_diversity_i[k] = mean;
    free(aux_diversity);
};


void elDE::sr(int k)
{ 
    double count = 0;
    bool flag = false;

    for (int i = 0; i < population_size && !flag; i++)
        if(phi[i] <= eps)
            flag = true;
    
    //count += 1.0;
    //count /= population_size;

    SR[k] = flag; //count*100;
};



void elDE::sd(int k)
{
    double sum;

    for (int i = 0; i < dimension; i++)
    {
        sd_i [k*dimension+i] = 0;
        for (int j = 0; j < population_size; j++)
            sd_i [k*dimension+i] += x[dimension*j+i];
    }

    for (int i = 0; i < dimension; i++)
        if(sd_i[k*dimension+i] != 0)
            sd_i[k*dimension+i] /= population_size; 


    for (int i = 0; i < dimension; i++)
    {
        sum = 0;
        for (int j = 0; j < population_size; j++)
            sum +=  std::pow(sd_i[k*dimension+i]-x[dimension*j+i], 2);
        if(sd_i[k*dimension+i] != 0)
            sd_i[k*dimension + i] = std::sqrt(sum /population_size);
    }
    
};



void elDE::printPopulation()
{
    for (int i = 0; i < population_size; i++)
    {
        for (int j = 0; j < dimension; j++)
            std::cout << "x" <<j<<":  "<<x[dimension*i+j]<<"  ";
        std::cout<<"   f:"<<f[i]<<"    Phi:"<<phi[i]<<std::endl;
    }std::cout  << std::endl;
};