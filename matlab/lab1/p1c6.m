f = 500;
tmax = 1;
fs = 8000; % pqsi
t = 0:1/fs:tmax-1/fs;
A = 1;
signal = A*sin(2*pi*(f)*t);
fase = pi*rand(1, fs) - pi/2;

signalf = A*sin(2*pi*(f)*t + fase);
subplot 211
plot(t*1000,signal)
xlim([0, 10]);
    xlabel('Tiempo (ms)');
    ylabel('Amplitud');
    title("Señal original");

subplot 212
plot(t*1000,signalf)
xlim([0, 10]);
    xlabel('Tiempo (ms)');
    ylabel('Amplitud');
    title("Señal con ruido en fase");

soundsc(signalf)
