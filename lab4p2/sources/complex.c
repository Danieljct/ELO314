#include <math.h>
#include "complex.h"

void c_add(Complex* result, Complex* a, Complex* b)
{
    result->real = a->real + b->real;
    result->img  = a->img  + b->img;
}
void c_sub(Complex* result, Complex* a, Complex* b)
{
    result->real = a->real - b->real;
    result->img  = a->img  - b->img;
}
void c_prod(Complex* result, Complex* a, Complex* b)
{
    result->real = a->real*b->real - a->img*b->img;
    result->img  = a->real*b->img  + a->img*b->real;
}
void c_div(Complex* result, Complex* a, Complex* b)
{
    float den    =  b->real*b->real + b->img*b->img;
    result->real = (a->real*b->real + a->img*b->img )/den;
    result->img  = (a->img*b->real  - a->real*b->img)/den;
}
float c_mag(Complex* a)
{
	return (float) sqrt(a->real*a->real + a->img*a->img);
}
