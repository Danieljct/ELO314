function plot_fft_mag(x,fs)         
% Grafica en semilog, el espectro en frecuencias de una señal 'x' muestreada
% a tasa la tasa fijan de fs[Hz], con eje de ordenadas ajustado para medir
% la amplitud de las componentes armónicas periódicas
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%Obtención del largo del vector de datos de la FFT
FF=fft(x);                          %Para esta fucnión, x debe ser una señal REAL
N=length(FF);
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%Obtención del vector de frecuencias
f=(0:fs/N:(N-1)*fs/N);
%Normalización de amplitudes de componentes espectrales para SEÑALES REALES
a=abs(FF).*(2/N);
a(1)=a(1)/2;                            %La frecuencia cero no debe cambiar
f=f(1:floor(N/2));
a=a(1:floor(N/2));
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
figure
plot(f,a), set(gca, 'YScale', 'log'), grid on
title('FFT')
xlabel('Frecuencia Hz')
ylabel('Amplitud de armónicos periódicos')
