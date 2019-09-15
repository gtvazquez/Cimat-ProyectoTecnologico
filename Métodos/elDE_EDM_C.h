#ifndef _HEADER_H_5
#define _HEADER_H_5
#include "elDE_EDM_v2.h"


class elDE_EDM_C : public elDE_EDM_v2
{
protected:
    
    int clusterSize;

    std::vector <std::vector<int>> selected_;

    void mutation();
    void crossover();
    void selection(bool);
    void replacement();
    
public:
    elDE_EDM_C(int,int,int,int, double,int,std::string);
    ~elDE_EDM_C();
};

#endif