clear;
fs = 8000;
fo = 100;
Np = fs/fo;
X = excite(fs,Np);
%soundsc(X,fs);
f = 0:fs-1;
x = fft(X,fs);
plot(f,abs(x));
xlim([0,fs/2]);
ylim([0,105]);

%% 4)
load('vowels.mat')
vocales = ["a","e","i","o","u"];

for i = 1:5
    h(i,:) = mylpc(eval("vowel_"+vocales(i)),15);
    h2(i,:) = lpc(eval("vowel_"+vocales(i)),15);
    H(i,:) = freqz(1,h(i,:),fs/2);  
    subplot(3,2,i);
    plot(0:fs/2-1,mag2db(abs(H(i,:))));
    titulo = "Magnitud del espectro del filtro mylpc()";
    f_print(20,22,titulo,10,'Serif',"Frecuencia (Hz)","Magnitud","figuraIB4",~mod(i,5));
   
end

immse(h2,h)

%% 
close all;
for i = 1:5
    y(i,:) = filter(1,h(i,:),X);
    soundsc(y(i,:),fs);
    pause(2);
    audiowrite("mylpc_vowel_"+vocales(i)+".wav", y(i,:),fs);
    Y(i,:) = fft(y(i,:));
    figure;
    plot(0:fs-1,mag2db(abs(Y(i,:)))); xlabel("Frecuencia (Hz)");xlim([0,fs/2])
end
close all;


