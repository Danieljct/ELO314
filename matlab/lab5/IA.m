fs = 8000;
fo = 100;
Np = fs/fo;
X = excite(fs,Np);
%soundsc(X,fs);
f = 0:fs-1;
x = fft(X,fs);
stem(f,abs(x),'x');
xlim([0,fs/2]);
ylim([0,105]);
f_print(20,8,"Magnitud del espectro de tren de impulsos de frecuencia 100 Hz",10,'Serif',"Frecuencia (Hz)","Magnitud","figuraIA1",1);
%% 2)
load('vowels.mat')
vocales = ["a","e","i","o","u"];

for i = 1:5
    h(i,:) = lpc(eval("vowel_"+vocales(i)),15);
    H(i,:) = freqz(1,h(i,:),fs/2);  
    subplot(3,2,i);
    plot(0:fs/2-1,mag2db(abs(H(i,:))));
    titulo = "Magnitud del espectro de filtro LPC para letra "+vocales(i);
    f_print(20,22,titulo,10,'Serif',"Frecuencia (Hz)","Magnitud","figuraIA2",~mod(i,5));
end

%% 3)
close all;
for i = 1:5
    y(i,:) = filter(1,h(i,:),X);
    soundsc(y(i,:),fs);
    pause(2);
    %audiowrite("matlab_vowel_"+vocales(i)+".wav", y(i,:),fs);
    Y(i,:) = fft(y(i,:));
    subplot(3,2,i);
    plot(0:fs-1,mag2db(abs(Y(i,:))));xlim([0,fs/2])
    titulo = "Espectro de letra "+vocales(i)+" sintetizada";
    f_print(20,22,titulo,10,'Serif',"Frecuencia (Hz)","Magnitud","figuraIA3",~mod(i,5));
end
close all;

