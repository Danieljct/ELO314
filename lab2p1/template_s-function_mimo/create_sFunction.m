% Instanciación del objeto
def = legacy_code('initialize');
% Nombre de la s-function
def.SFunctionName = 'mimo_template';
% Protiotipo de la función
% Notar es información para Matlab, y el solo acepta nombres de entrada u1,
% u2, etc, y salida y1
% Usar el nombre de la función en C que implementó
def.OutputFcnSpec = 'void multi(double u1, double u2, double y1[1], double y2[1])';
% Archivo de cabecera
def.HeaderFiles = {'template_m.h'};
% Archivo fuente en C
def.SourceFiles = {'template_m.c'};
% Compilación para la s-function
legacy_code('sfcn_cmex_generate', def)
legacy_code('compile', def)
