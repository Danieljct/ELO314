close all;
N = 8;
n = 0:N-1;
k = 0:N-1;
f = k/N*2*pi;

A = genAmatrix(N);


x1 = n == 0;
x2 = n >= 0;
x3 = exp(-j*2*pi*n/N);
x4 = cos(2*pi*n/N);
x = [x1;x2;x3;x4];


for i = 1:4
    subplot(2,2,i)
    X = DFTdc(x(i,:));
    Y = fft(x(i,:));
    hold on
    stem(f, abs(X),'x');
    stem(f, abs(Y));
    text(1,max(abs(X))*0.5, "immse="+num2str(immse(X, Y)));
    legend("DFTdc", "FFT");
    hold off
    f_print(20,12,"Transformadas x_"+num2str(i)+"[n]", 10,'Serif',"Frecuencia normalizada (rad/muestra)", "Magnitud","parte1_VI_1_1",~mod(i,4));

end

%% tiempos de procesamiento
N = 100:100:5000;
Fs = 5e3;
tiempo = zeros(2,length(N));
for i = 1:length(N)

t = linspace(0,0.1,N(i));
x5 = cos(2*pi*t*100/Fs);


tic
X = DFTsum(x5);
tiempo(1,i) = toc

tic
Y = DFTdc(x5);
tiempo(2,i) = toc
end
figure;
hold on;
plot(N,tiempo(1,:));
plot(N,tiempo(2,:));
hold off;
legend("sum DFT", "dc DFT")
f_print(20,8,"Comparación tiempos x_5[n]", 10,'Serif',"Cantidad de datos (N)","Tiempo (s)", "parte1_VI_1_2",1);
%% 2)

N = 8;
n = 0:N-1;
k = 0:N-1;
f = k/N*2*pi;

x1 = n == 0;
x2 = n >= 0;
x3 = exp(-j*2*pi*n/N);

Fs = 5e3;
x5 = cos(2*pi*n*100/Fs);



x = [x1;x2;x3;x5];

figure;
for i = 1:4
    X = FFT8(x(i,:));
    Y = fft(x(i,:));
    subplot(2,2,i);
    hold on
    stem(f, abs(X),'x');
    stem(f, abs(Y));
    text(1,max(abs(X))*0.5, "immse="+num2str(immse(X, Y)));
    legend("FFT8", "fft");
    f_print(20,12,"Transformadas x_"+num2str(i)+"[n]", 10,'Serif',"Frecuencia normalizada (rad/muestra)", "Magnitud","parte1_VI_2_1",~mod(i,4));

    hold off
end

%% 3)
n1 = 10:19;
N = 2.^n1;

Fs = 5e3;
tiempo = zeros(2,length(N));

for i = 1:length(N)
t = linspace(0,0.1,N(i));
x5 = cos(2*pi*t*100/Fs);

tic
X = fft(x5);
tiempo(1,i) = toc;

tic
Y = fft_stage(x5);
tiempo(2,i) = toc;

if(N(i)==4096)
    immse(X, Y)
end

end
figure;
subplot 121
plot(N,tiempo(1,:));
f_print(12,8,"Tiempo de procesamiento fft matlab", 10,'Serif',"Cantidad de puntos (N)", "Tiempo (s)","parte1_VI_3_1",0);
set(gca, 'XScale', 'log')
subplot 122
plot(N,tiempo(2,:));
set(gca, 'XScale', 'log')
f_print(20,8,"Tiempo de procesamiento fft\_stage", 10,'Serif',"Cantidad de puntos (N)", "Tiempo (s)","parte1_VI_3_1",1);





