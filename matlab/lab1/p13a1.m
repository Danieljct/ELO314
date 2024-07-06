%% A1
%f1 = 50; f2 = 500;
%f1 = 200; f2 = 300;
f1 = 880; f2 = 1100; f3 = 1320;
tmax = 1;
fs = 10000; % pqsi
t = 0:1/fs:tmax;
A = 1; phi = 0;
signal = A*sin(2*pi*(f1)*t + phi);
signal2 = A*sin(2*pi*(f2)*t + phi);
signal3 = A*sin(2*pi*(f3)*t + phi);

subplot 211
plot(t,signal);
xlabel("Tiempo (s)");
ylabel("Amplitud (V)");
title("Seno de frecuencia 50 Hz");
xlim([0 0.5]);
subplot 212
plot(t,signal2);
xlabel("Tiempo (s)");
ylabel("Amplitud (V)");
title("Seno de frecuencia 500 Hz");
xlim([0 0.05]);

soundsc(signal)
pause(1);
soundsc(signal2)

%% A2
signal4 = signal + signal2 + signal3;
plot(t*1000,signal3);
xlabel("Tiempo (ms)");
ylabel("Amplitud (V)");
title("Señal suma de senos de frecuencias 50 y 500 Hz");
xlim([0 50]);



soundsc(signal4)
%a2
%signal4 = signal + signal2 + signal3;
%plot(t,signal3)
%xlim([0, 50e-3]);

%soundsc(signal2)

% el mcd de 880, 1100 y 1320 es 220 LA