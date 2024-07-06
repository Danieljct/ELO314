load('fft_analysis.mat')
N = round(length(modulation2));
f = linspace(-Fs/2-1, Fs/2, 10*N);

hold on
plot(f, fftshift(  mag2db(abs(fft(  modulation1.*hamming((N)) ,10*N )))));
plot(f, fftshift(  mag2db(abs(fft(  modulation2.*hamming((N))  ,10*N )))));
hold off
ylim([20, 70]);
xlim([-1250, 1250]);
legend("modulation1","modulation2");
% 25 940 central, hamming se ve
f_print(20,8,"fft de señales modulation1 y modulation2, con ventana de hamming", 10,'Serif',"Frecuencia (Hz)", "Magnitud (dB)","parte1_II_2_1",1);
%%


load('fft_analysis.mat')
N = round(length(modulation2));
f = linspace(-Fs/2-1, Fs/2, 10*N);
figure;
plot(f, fftshift(  mag2db(abs(fft(  modulation1.*hamming((N)) ,10*N )))));
ylim([20, 70]);
xlim([850, 1050]);
legend("modulation1");
% 25 940 central, hamming se ve
f_print(20,8,"fft de señal modulation1 hamming", 10,'Serif',"Frecuencia (Hz)", "Magnitud (dB)","parte1_II_2_2",1);

%%

load('fft_analysis.mat')
N = round(length(modulation2));
f = linspace(-Fs/2-1, Fs/2, 10*N);
plot(f, fftshift(  mag2db(abs(fft(  distorted.*flattopwin((N))  ,10*N )))));
ylim([-10, 90]);
xlim([-5000, 5000]);
%4700
f_print(20,8,"fft de señal distorted con ventana flattopwin", 10,'Serif',"Frecuencia (Hz)", "Magnitud (dB)","parte1_II_2_3",1);