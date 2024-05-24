% Instanciación del objeto
def = legacy_code('initialize');
% Nombre de la s-function
def.SFunctionName = 'goertzelSFunction';

def.StartFcnSpec = 'initialization()';
% Protiotipo de la función
% Notar es información para Matlab, y el solo acepta nombres de entrada u1,
% u2, etc, y salida y1
% Usar el nombre de la función en C que implementó
def.OutputFcnSpec = 'void goertzelFunction(double u1, double y1[1], double  y2[1], double  y3[1], double  y4[1], double  y5[1], double  y6[1], double  y7[1])'; % 
% Archivo de cabecera
def.HeaderFiles = {'goertzel.h'};
% Archivo fuente en C
def.SourceFiles = {'goertzel.c'};
% Compilación para la s-function
legacy_code('sfcn_cmex_generate', def)
legacy_code('compile', def)
