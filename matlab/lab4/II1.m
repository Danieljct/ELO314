    fs = 5e3;
t = 0:1/fs:0.1-1/fs;
x1 = 0.5*cos(2*pi*100*t) + 1.5*cos(2*pi*500*t);
r = sqrt(2)*randn(1,length(t));
x2 = x1 + r;

plot(1000*t,x2);
f_print(20,8,"Señal de dos tonos puros y ruido", 10,'Serif',"Tiempo (ms)", "Magnitud","parte1_II_1_1",1);

figure;

f = linspace(0, fs, length(t));
hold on
stem(f, abs(fft(x1)));
stem(f, abs(fft(x2)),'x');
hold off
legend("Sin ruido", "Con ruido");
xlim([0, 1000]);
f_print(20,8,"fft de señales con y sin ruido", 10,'Serif',"Frecuencia (Hz)", "Magnitud","parte1_II_1_2",1);
%% c
t = 0:1/fs:0.1-1/fs;
f = linspace(0, fs, length(t));
figure;
hold on
plot(f, mag2db(abs(fft(x1))/375) ,'x');
plot(f, mag2db(abs(fft(x2))/375 ));
hold off
legend("Sin ruido", "Con ruido");
xlim([0, 1000]); ylim([-40, 5]);
f_print(20,8,"fft de señales con y sin ruido, normalizadas en dB", 10,'Serif',"Frecuencia (Hz)", "Magnitud (dB)","parte1_II_1_3",1);
%% c2
t = 0:1/fs:1-1/fs;
x1 = 0.5*cos(2*pi*100*t) + 1.5*cos(2*pi*500*t);
r = sqrt(2)*randn(1,length(t));
x2 = x1 + r;
f = linspace(0, fs, length(t));
figure;
hold on
plot(f, mag2db(abs(fft(x1))/3750),"x");
plot(f, mag2db(abs(fft(x2))/3750 ));
legend("Sin ruido", "Con ruido");
xlim([0, 1000]); ylim([-40, 5]);
f_print(20,8,"fft de señales con y sin ruido, normalizadas en dB, tiempo de 1 segundo", 10,'Serif',"Frecuencia (Hz)", "Magnitud (dB)","parte1_II_1_3_2",1);