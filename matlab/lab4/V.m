N = [2,4,8];
for i = 1:3
A = genAmatrix(N(i));
A2 = dftmtx(N(i));
disp("Error cuadrático medio para N="+N(i)+" = "+immse(A, A2))
end

%% 2
N = [8,64];
for i = 1:2
    figure 
A = genAmatrix(N(i));
subplot (1,2,1)
imagesc(real(A))
f_print(20,8,"Parte real", 10,'Serif', "Indice de frecuencia k","Indice temporal n","parte1_V_2_"+num2str(i),0);
subplot (1,2,2)
imagesc(imag(A))
f_print(20,8,"Parte imaginaria", 10,'Serif', "Indice de frecuencia k","Indice temporal n", "parte1_V_2_"+num2str(i),1);
end

%% 3 

N = 8;
n = 0:N-1;
k = 0:N-1;
f = k/N*2*pi;
A2 = dftmtx(N);
x1 = n == 0;
x2 = n >= 0;
x3 = exp(-j*2*pi*n/N);
x4 = cos(2*pi*n/N);
x = [x1;x2;x3;x4];


for i = 1:4
    subplot(2,2,i)
    X = DFTmatrix(x(i,:).');
    Y = fft(x(i,:));
    hold on
    stem(f, abs(X),'x');
    stem(f, abs(Y));
    text(1,max(abs(X))*0.5, "immse="+num2str(immse(X', Y)));
    legend("DFT", "FFT");
    hold off
    f_print(20,12,"Transformadas x_"+num2str(i)+"[n]", 10,'Serif',"Frecuencia normalizada (rad/muestra)", "Magnitud","parte1_V_3_1",~mod(i,4));

end

%% 3)


Fs = 5e3;
t = (0:1/Fs:1-1/Fs)';
x5 = cos(2*pi*t*100/Fs);
N = length(x5);
f = linspace(0,2*pi,N);
X = DFTmatrix(x5);
Y = fft(x5);

subplot 121
hold on
stem(f, abs(X),'x');
stem(f, abs(Y));
text(pi,max(abs(X))*0.5, "immse="+num2str(immse(X, Y)));
legend("DFT", "FFT"); xlim([0 2*pi])
f_print(12,8,"Transformadas x_5[n]", 10,'Serif',"Frecuencia normalizada (rad/muestra)", "Magnitud","parte1_V_3_2",0);
hold off
subplot 122
hold on
stem(f, abs(X),'x');
stem(f, abs(Y));
text(pi,max(abs(X))*0.5, "immse="+num2str(immse(X, Y)));
legend("DFT", "FFT"); xlim([0 2*pi]);ylim([0 7])
f_print(20,8,"Transformadas x_5[n]", 10,'Serif',"Frecuencia normalizada (rad/muestra)", "Magnitud","parte1_V_3_2",1);
%% 4)


N = 100:100:5000;
Fs = 5e3;
tiempo = zeros(3,length(N));
for i = 1:length(N)

t = linspace(0,0.1,N(i));

x5 = cos(2*pi*t*100/Fs)';

DFTM = @() DFTmatrix(x5);
DFTS = @() DFTsum(x5);
DFFT = @() fft(x5);
 
tiempo(1,i) = timeit(DFTM)
tiempo(2,i) = timeit(DFTS)
tiempo(3,i) = timeit(DFFT)
end
hold on;
plot(N,tiempo(2,:));
plot(N,tiempo(1,:));
plot(N,tiempo(3,:));
legend("sum DFT", "Matrix DFT", "FFT")
f_print(20,8,"Comparación tiempos x_5[n]", 10,'Serif',"Cantidad de datos (N)","Tiempo (s)", "parte1_V_4_1",1);
