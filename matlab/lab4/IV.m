N = 8;
n = 0:N-1;
k = 0:N-1;
f = k/N*2*pi;
x1 = n == 0;
x2 = n >= 0;
x3 = exp(-j*2*pi*n/N);
x4 = cos(2*pi*n/N);


x = [x1;x2;x3;x4];

for i = 1:4
    subplot(2,2,i)
    X = DFTsum(x(i,:));
    Y = fft(x(i,:));
    hold on
    stem(f, abs(X),'x');
    stem(f, abs(Y));
    hold off
    text(2,max(abs(X))*0.5, "immse="+num2str(immse(X, Y)));
    legend("DFT", "FFT");
    f_print(20,12,"Transformadas x_"+num2str(i)+"[n]", 10,'Serif',"Frecuencia normalizada (rad/muestra)", "Magnitud","parte1_IV_1_1",~mod(i,4));


end

%% DTFT
fw = 1e3;
w = -pi:1/fw:pi;
f = -pi:2*pi/N:pi-2*pi/N;
figure;
for i = 1:4
    subplot(2,2,i)
    X = DTFTsum(x(i,:),fw);
    Y = fftshift(DFTsum(x(i,:)));
    hold on
    plot(w, abs(X));
    stem(f, abs(Y));xlim([-pi,pi])
    hold off
    legend("DTFT", "DFT");
    f_print(20,12,"Transformadas x_"+num2str(i)+"[n]", 10,'Serif',"Frecuencia normalizada (rad/muestra)", "Magnitud","parte1_IV_2_1",~mod(i,4));

end

%% 3)
Fs = 5e3;
t = 0:1/Fs:.1-1/Fs;
x5 = cos(2*pi*t*100/Fs);
%plot(t,x5)
N = length(x5);
f = linspace(0,2*pi,N);

X = DFTsum(x5);
Y = fft(x5);

figure;
%hold on
%stem(f, abs(X),'x');
%stem(f, abs(Y));
%hold off
plot(f, abs(X)-abs(Y));
text(1,max(abs(X)-abs(Y))*0.5, "immse="+num2str(immse(X, Y)));
xlim([0 2*pi])

f_print(12,8,"Error entre DFT y fft", 10,'Serif',"Frecuencia normalizada (rad/muestra)", "Magnitud","parte1_IV_3_1",1);
