#ifndef _HEADER_H_3
#define _HEADER_H_3
#include "elDE.h"
#include <vector>

class elDE_EDM : public elDE
{
protected:
    
    double Di;
    double Dt;


    double *current;
    double *f_current;
    double *g_current;
    double *h_current;
    double *phi_current;


    double *elite;
    double *f_elite;
    double *g_elite;
    double *h_elite;
    double *phi_elite;

    std::vector <int> list;
    std::vector <int> selected;
    std::vector <int> penalized;


    void mutation();
    void crossover();
    void selection(bool);
    void replacement();
    void updateDt();
    void setUnionToCurrent();
    
public:
    elDE_EDM(int,int,int,double,int,std::string);
    ~elDE_EDM();
};

#endif

