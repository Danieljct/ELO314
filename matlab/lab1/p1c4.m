f = 500;
tmax = 1;
fs = 60000; 
t = 0:1/fs:tmax-1/fs;
A = 1;
signal = A*sin(2*pi*(f)*t);
signalr = 2*rand(1, fs) - 1;

signalf = signal + signalr;
subplot 311
plot(t*1000,signalr)
xlim([0, 10]);
    xlabel('Tiempo (ms)');
    ylabel('Amplitud');
    title("Señal aleatoria");

subplot 312
plot(t*1000,signal)
xlim([0, 10]);
    xlabel('Tiempo (ms)');
    ylabel('Amplitud');
    title("Señal sin ruido");

subplot 313
plot(t*1000,signalf)
xlim([0, 10]);
    xlabel('Tiempo (ms)');
    ylabel('Amplitud');
    title("Suma de ambas señales");

soundsc(signal)
%% aumentar la frecuencia de muestreo

f = 500;
tmax = 1;
f1 = 8000; f2 = 20000; f3  = 40000;
t1 = 0:1/f1:tmax-1/f1;
t2 = 0:1/f2:tmax-1/f2;
t3 = 0:1/f3:tmax-1/f3;
A = 1;

signalf1 = A*sin(2*pi*(f)*t1) + 2*rand(1, f1) - 1;
signalf2 = A*sin(2*pi*(f)*t2) + 2*rand(1, f2) - 1;
signalf3 = A*sin(2*pi*(f)*t3) + 2*rand(1, f3) - 1;

subplot 311
plot(t1*1000,signalf1)
xlim([0, 10]);
xlabel('Tiempo (ms)');
ylabel('Amplitud');
title("Frecuencia de muestreo de 8.000 sps");

subplot 312
plot(t2*1000,signalf2)
xlim([0, 10]);
xlabel('Tiempo (ms)');
ylabel('Amplitud');
title("Frecuencia de muestreo de 20.000 sps");
subplot 313
plot(t3*1000,signalf3)
xlim([0, 10]);
xlabel('Tiempo (ms)');
ylabel('Amplitud');
title("Frecuencia de muestreo de 40.000 sps");
soundsc(signalf1, f1);
pause(1)
soundsc(signalf2, f2);
pause(1)
soundsc(signalf3, f3);
