#ifndef _HEADER_H_
#define _HEADER_H_

#include <string>

void cec17_test_COP(double *x, double *f, double *g,double *h, int nx, int mx,int func_num);

class de
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

    // Statement of functions
    void create_population();
    double constraint_violation(int,double*,double*);
    double randUniform();
    bool comparate_interval(double,double,double,double);
    void DE_rand_1_bin(int);
    void update_eps_level();
    void check_rank(double &);
    void update_best();
    void write_best_solution();

public:
    de(int,int,int,double,double,double,int,std::string);
    ~de();
    virtual void run();

};

#endif // !_HEADER_H_



