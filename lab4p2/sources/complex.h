#ifndef complex_h_
#define complex_h_

typedef struct Complex_body {
     float real;
     float img;
 } Complex;

void c_add(Complex* result, Complex* a, Complex* b);

void c_sub(Complex* result, Complex* a, Complex* b);

void c_prod(Complex* result, Complex* a, Complex* b);

void c_div(Complex* result, Complex* a, Complex* b);

float c_mag(Complex* a);

#endif
