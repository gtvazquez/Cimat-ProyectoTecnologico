#include <cstddef>

double *OShift=NULL,*M=NULL,*M1=NULL,*M2=NULL,*y=NULL,*z=NULL,*z1=NULL,*z2=NULL;

int ini_flag=0,n_flag,func_flag,f5_flag;
int kEval = 20000;
double cte = 0.7;

bool overwrite = false; bool overwrite_f= false;
bool overwrite_div = false; bool overwrite_sr = false;
bool overwrite_sd = false;
bool CR_s;