fs = 5e3; 
t = 0:1/fs:0.100-1/fs;
w = 2*pi*100;
x1 = sin(w*t); x2 = cos(w*t);
%a
N = 4096; %bins de frecuencia
f = linspace(-fs/2, fs/2, N);
stem(f, fftshift(abs(fft(x1,N))),'.');
grid;
f_print(20,8,"Magnitud de la fft de señal x_1", 10,'Serif',"frecuencia (Hz)", "Amplitud","parte1_1_a",1);
figure
stem(f, fftshift(abs(fft(x2,N))),'.'); grid; ylim([0 250]);
f_print(20,8,"Magnitud de la fft de señal x_2", 10,'Serif',"frecuencia (Hz)", "Amplitud","parte1_1_a_2",1);

%% b
f = linspace(0, fs, N);
stem(f, (abs(fft(x1,N))),'.');grid;
f_print(20,8,"Magnitud de la fft de señal x_1", 10,'Serif',"frecuencia (Hz)", "Amplitud","parte1_1_b",1);
figure
stem(f, (abs(fft(x2,N))),'.');grid;ylim([0 250]);
f_print(20,8,"Magnitud de la fft de señal x_2", 10,'Serif',"frecuencia (Hz)", "Amplitud","parte1_1_b_2",1);
%% 2a- 
fs = 5e3; 
t = 0:1/fs:0.100-1/fs;
w = 2*pi*100;
x1 = sin(w*t); x2 = cos(w*t);
%a
N = [256 500 2048]; %bins de frecuencia
for i = 1:3
    f = linspace(-fs/2, fs/2, N(i));
    f = f/(fs)*2*pi;
    subplot(2, 3, 2*i-1)
    stem(f, fftshift(abs(fft(x1,N(i)))),'.'); xlim([-.75 .75]);
    titulo = sprintf("FFT de seno con N = %d", N(i));
    f_print(20,12,titulo, 10,'Serif',"Frecuencia normalizada (rad/muestra)", "Magnitud","parte1_2_a_1",0);
    subplot(2, 3, 2*i)
    stem(f, fftshift(abs(fft(x2,N(i)))),'.');xlim([-.75 .75]);
    titulo = sprintf("FFT de coseno con N = %d", N(i));
    f_print(20,12,titulo, 10,'Serif',"Frecuencia normalizada (rad/muestra)", "Magnitud","parte1_2_a_1",~mod(i,3));
end

%% 2b- 
fs = 5e3; 
t = 0:1/fs:0.100-1/fs;
w = 2*pi*100;
x1 = sin(w*t); x2 = cos(w*t);

N = [256 500 2048]; %bins de frecuencia
for i = 1:3
    f = linspace(-fs/2, fs/2, N(i));
    f = f/(fs)*2*pi;
    subplot(2, 3, 2*i-1)
    stem(f, fftshift(real(fft(x1,N(i)))),'.');xlim([-.75 .75]);ylim([-250,250])
    titulo = sprintf("FFT de seno con N = %d", N(i));
    f_print(20,12,titulo, 10,'Serif',"Frecuencia normalizada (rad/muestra)", "Parte real","parte1_2_a_1",0);
    subplot(2, 3, 2*i)
    stem(f, fftshift(real(fft(x2,N(i)))),'.');xlim([-.75 .75]);ylim([-250,250])
    titulo = sprintf("FFT de coseno con N = %d", N(i));
    f_print(20,12,titulo, 10,'Serif',"Frecuencia normalizada (rad/muestra)", "Parte real","parte1_2_b_1",~mod(i,3));
end
figure
for i = 1:3
    f = linspace(-fs/2, fs/2, N(i));
    f = f/(fs)*2*pi;
    subplot(2, 3, 2*i-1)
    stem(f, fftshift(imag(fft(x1,N(i)))),'.');xlim([-.75 .75]);ylim([-250,250])
    titulo = sprintf("FFT de seno con N = %d", N(i));
    f_print(20,12,titulo, 10,'Serif',"Frecuencia normalizada (rad/muestra)", "Parte imaginaria","parte1_2_a_1",0);
    subplot(2, 3, 2*i)
    stem(f, fftshift(imag(fft(x2,N(i)))),'.');xlim([-.75 .75]);ylim([-250,250])
    titulo = sprintf("FFT de coseno con N = %d", N(i));
    f_print(20,12,titulo, 10,'Serif',"Frecuencia normalizada (rad/muestra)", "Parte imaginaria","parte1_2_b_2",~mod(i,3));

end

