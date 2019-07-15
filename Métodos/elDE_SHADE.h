#ifndef _HEADER_H_2
#define _HEADER_H_2
#include "elDE.h"


class elDE_SHADE : public elDE
{
protected:

    void mutation();
    void crossover();
    void selection(bool);
    void replacement();

public:
    elDE_SHADE(int,int,int,double,int,std::string);
};
#endif