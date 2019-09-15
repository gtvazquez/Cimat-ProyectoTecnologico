#ifndef _HEADER_H_4
#define _HEADER_H_4

#include "elDE.h"
#include <vector>

class elDE_EDM_v2 : public elDE
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


    virtual void mutation();
    virtual void crossover();
    virtual void selection(bool);
    virtual void replacement();
    void updateDt();
    void setUnionToCurrent();
    
public:
    elDE_EDM_v2(int,int,int,double,int,std::string);
    ~elDE_EDM_v2();
};

#endif