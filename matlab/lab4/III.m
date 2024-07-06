load('nspeech.mat');
N = length(nspeech);
f = (0:fs/N:fs-1/N);
t = (0:N-1)/fs;
%soundsc(nspeech, fs);
X=fft(nspeech);
plot(f, mag2db(abs(X)));
xlim([0 4000])

f_print(12,8,"fft de señal nspeech", 10,'Serif',"Frecuencia (Hz)", "Magnitud (dB)","parte1_III_1_1",1);

w = 2*pi*f;
fnotch = 1685.02;
theta = 2*pi*fnotch/fs;
H =  1-2*cos(theta)*exp(-j*w/fs)+exp(-2*j*w/fs);
plot(f, mag2db(abs(H)));
xlim([0 4000])
f_print(12,8,"Respuesta en frecuencia filtro", 10,'Serif',"Frecuencia (Hz)", "Magnitud (dB)","parte1_III_1_2",1);


%%
figure;
hold on
plot(f, mag2db(abs(X)));
plot(f, mag2db(abs(X.*H)));
hold off
legend("Señal original", "Señal filtrada")
f_print(20,8,"Magnitud del espectro de las señales", 10,'Serif',"Frecuencia (Hz)", "Magnitud (dB)","parte1_III_1_3",1);


%pause(N/fs);
%%
sonido = ifft(X.*H,"symmetric");
soundsc(real(sonido), fs);

figure;
hold on
plot(t,nspeech)
plot(t,(sonido))
hold off
xlim([0 t(end)])
legend("Señal original", "Señal filtrada")
f_print(20,8,"Inversa luego del filtro", 10,'Serif',"Tiempo (s)", "Magnitud","parte1_III_1_4",1);

