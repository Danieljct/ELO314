%% 3a)
th = pi/3;
r = .99; %0.99, 0.9, 0.7
p1 = r*cos(th)+j*r*sin(th);
p2 = r*cos(th)-j*r*sin(th);

coeff_a = [1 -(p1+p2) p1*p2];
coeff_b = [1 0 0];

freqz(coeff_b, coeff_a);
%% 3b)
 impulse(tf(coeff_b, coeff_a, -1));

%% 4)
load('pcm.mat');
x = pcm;
th = 3146*2*pi/fs;
r = .99;
p1 = r*cos(th)+j*r*sin(th);
p2 = r*cos(th)-j*r*sin(th);

coeff_a = [1 -(p1+p2) p1*p2];
coeff_b = [1 0 0];

y = IIR_filter(x,coeff_b,coeff_a);

hold on
pspectrum(x, fs)
soundsc(x,fs)
pause(3);
pspectrum(y, fs)
soundsc(y,fs)
legend("Espectro original", "Espectro con Filtro IIR")
hold off
t = (0:length(x)-1)/fs;
figure

subplot 121
plot(t,x);
title("Señal pcm original");
xlabel("Tiempo (s)"); ylabel("Amplitud");
subplot 122
plot(t,y);
title("Señal pcm filtrada");
xlabel("Tiempo (s)"); ylabel("Amplitud");