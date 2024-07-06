function [upsampling, y, gd] = myInterpolation (x,p)
% Sanity check
p = round(p);
% Muestras de entrada
n = length(x);
% Muestras de salida
N = n*p;
% Inicialización del vector de salida
y = zeros(N,1);
% Interpolación
y(1:p:end) = x;
upsampling = y;

% Filtrado FIR 
%%%%%%%%%% Completar %%%%%%%%%%
w_norm = 1/p;     % Frecuencia de corte normalisada vista desde nueva tasa de muestreo alta
M = 40;         % Orden del filtro
b = fir1(M, w_norm);    % Coeficientes numerador del filtro FIR
% Además, ¿qué falta retornar como gd?
gd = M/2;
%%%%%%%%%% Hasta aquí %%%%%%%%%%

% Filtrado debe ajustar ganancia por factor 'p'
y = filter(p*b,1,y);

