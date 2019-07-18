#ifndef _HEADER_H_
#define _HEADER_H_

#include <string>

class elDE
{
protected:
    // Statement of variables
    std::string method_name;
    int func_num;
    int dimension;
    int population_size;
    double F;
    double CR;
    double eps;
    double eps_level;
    int FEs;
    int maxFEs;
    int t_FEs;
    double phi_theta;
    double *f;
    double *x;
    double *g;
    double *h;
    double *phi;
    double *f_offspring;
    double *x_offspring;
    double *g_offspring;
    double *h_offspring;
    double *phi_offspring;
    double best_f;
    double *g_best_f;
    double *h_best_f;
    double *best_x;
    double phi_;
    double *mean_fi;
    double *mean_diversity_i;
    double *SR;
    double *sd_i;

    // Statement of functions
    void create_population();
    double constraint_violation(int,double*,double*);
    double randUniform();
    double randCauchy(double, double);
    double randGauss(double, double);
    bool comparate_interval(double,double,double,double);
    void DE_rand_1_bin(int,double);
    void update_eps_level();
    void check_rank(double &);
    void update_best();
    void write();
    double distance(double*,int, int);
    void mean_f(int);
    void mean_diversity(int);
    void sr(int);
    void sd(int);

    virtual void mutation();
    virtual void crossover();
    virtual void selection(bool);
    virtual void replacement();



public:
    elDE(int,int,int,double,double,double,int,std::string);
    ~elDE();
    void run();

};

#endif // !_HEADER_H_



