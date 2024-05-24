% Instanciación del objeto
def = legacy_code('initialize');
% Nombre de la s-function
def.SFunctionName = 'decodeDtmf';
% Protiotipo de la función
% Notar es información para Matlab, y el solo acepta nombres de entrada u1,
% u2, etc, y salida y1
% Usar el nombre de la función en C que implementó
def.OutputFcnSpec = 'void decodeDtmf(double u1, int32 y1[1] )';
% Archivo de cabecera
def.HeaderFiles = {'dtmf.h'};
% Archivo fuente en C
def.SourceFiles = {'dtmf.c'};
% Compilación para la s-function
legacy_code('sfcn_cmex_generate', def)
legacy_code('compile', def)
