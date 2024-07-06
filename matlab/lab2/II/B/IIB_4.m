%% 4

[data, fs] = audioread('gtr-jazz_16_48.wav');
data = data(:,1);
t = (0:length(data)-1)/fs;
data2 = resample(data,3,8);
t2 = (0:length(data2)-1)/(3*fs/8);

hold on
scatter(t,data,100,'.');
scatter(t2,data2,100,'.');
title("Comparación audio gtr-jazz\_16\_48.wav con resample")
legend("Señal de original", "Señal con resample");
xlabel("Tiempo (s)"); ylabel("Amplitud");
hold off

soundsc(data, fs);
pause(8);
soundsc(data2, 3*fs/8);