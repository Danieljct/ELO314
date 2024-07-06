[data, fs] = audioread('besh_16_20.wav');
t = 0:1/fs:(length(data)-1)/fs;
t1 = .065*fs:.18*fs;
t2 = .22*fs:.3*fs;
hold on
plot(t, data, 'color', 'b');
plot(t(t1), data(t1), 'color', 'r');
plot(t(t2), data(t2),'color', 'black');
title("Audio 'besh' separado en sonidos");
legend('original','E','sh');
xlabel("Tiempo (s)");
ylabel("Amplitud");
hold off
soundsc(data,fs);

audiowrite('Lab2p1_vocal.wav',data(t1),fs);