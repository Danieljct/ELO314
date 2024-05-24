/***************************************************************************//**
* \file     levinson.c
*
* \brief    Resuelve la ecuación matricial para encontrar el vector 'alpha'
*           R * alpha = r
*
* \authors  ???, Gonzalo Carrasco
*******************************************************************************/

/***************************************************************************//**
**      HEADER FILES
*******************************************************************************/
#include <stdlib.h>
#include "levinson.h"
/******************************************************************************
**      MACRO DEFINITIONS
*******************************************************************************/

/***************************************************************************//**
**      GLOBAL VARIABLES
*******************************************************************************/
float* e;
float* k;
float* alpha_prev;
/***************************************************************************//**
**      INTERNAL FUNCTION DEFINITIONS
*******************************************************************************/

/***************************************************************************//**
*   \brief  Inicialización de variables para función levinson()
*
*   \param order : Tamaño de los vectores
*
*   \return Void.
*******************************************************************************/
/* Inicialización de variables */
void levinson_init(short order){
    e          = (float *)malloc((order+1)*sizeof(float));
    k          = (float *)malloc((order+1)*sizeof(float));
    alpha_prev = (float *)malloc((order+1)*sizeof(float));
}

/***************************************************************************//**
*   \brief  Algoritmo de Levinson-Durbin
*
*   \param alpha [output]: Vector de parámetros de salida identificados
*   \param r      [input]: Vector de autocorrelaciones desde r(0) a r(p)
*   \param length [input]: Orden p del polinomio de denominador identificado
*
*   \return Void.
*******************************************************************************/
void levinson(float* alpha, float* r, short order){
    int   i=1,j=1;
    float sum=0.0;
    short loop;

    // Initialising the Array variables
    for (loop=0;loop<=order;loop++)
    {
        e[loop]=0;
        k[loop]=0;
        alpha[loop]=0;
        alpha_prev[loop]=0;
    }
    // End initalising
    e[0]=r[0];
    // Iterate with i=1 to length as iteration variable
    i=1;
    for(i=1;i<=order;i++)
    {
        //-----------------------------------
        // start step 1
        sum=0;
        for (j=1;j<=i-1;j++)                    // i: 1..length
            sum+=alpha_prev[j]*r[i-j];          // para i=1 y j=1, se usa r(0). Para i=length y j=1, entonces se usa hasta r(length-1)

        k[i]=(r[i]-sum)/e[i-1];                 // i= [1 .. length] entonces [r(1) .. r(length)]
        // End step 1
        //-----------------------------------   // r debe ser de largo (length+1), [r(0) .. r[length)]
        //start step 2
        alpha[i]=k[i];
        for(j=1;j<=i-1;j++)
            alpha[j]=alpha_prev[j]- ( k[i]*alpha_prev[i-j] ) ;
        //end step 2
        //-----------------------------------
        //start step 3
        e[i]=e[i-1]*(1-(k[i]*k[i]));
        //end step 3
        for( loop=0;loop<=order;loop++)
            alpha_prev[loop]=alpha[loop];
    }// End For levinson
    /*----------------------------------------------------------------------*/
    // Reformateo de constantes alpha = [1 -alpha(1) -alpha(2) ... -alpha(length-1)]
    for( loop=0;loop<=order;loop++)
        alpha[loop]=-1*alpha[loop];

    alpha[0]=1;

}
