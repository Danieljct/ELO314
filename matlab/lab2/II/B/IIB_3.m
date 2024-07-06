[data, fs] = audioread('alternate_tones_16_16.wav');
data = data(:,1);
%data = [1,zeros(1,length(data)-1)];
t = (0:length(data)-1)/fs;
y = zeros(1,length(data));
N = 10; M = .25*fs;
buffer = zeros(1,N*M);


for n = 1:length(data)
    for i = length(buffer)-1:-1:1
        buffer(i+1) = buffer(i);
    end
    buffer(1) = data(n);

    for k = 1:N 
        y(n) = y(n) + 0.35^k * buffer(k*M);
    end
    
end
hold on
plot(t,data, 'color', 'black');
plot(t,y, 'color', 'b');
legend("Señal de entrada", "Señal de salida");
xlabel("Tiempo (s)"); ylabel("Amplitud");
hold off

soundsc(data, fs);
pause(8);
soundsc(y,fs);
