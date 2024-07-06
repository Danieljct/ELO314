%% 1-2
tic;
%[data, fs] = audioread('gtr-jazz_16_48.wav');
[data, fs] = audioread('alternate_tones_16_16.wav');
data = data(:,1);

y = zeros(1,length(data));

M = fs*.125; N = 4; b = 0.35;
buffer = zeros(1,N*M);

for n = 1:length(data)
    for i = length(buffer)-1:-1:1
        buffer(i+1) = buffer(i);
    end
    buffer(1) = data(n);

    for k = 1:N 
        y(n) = y(n) + b * buffer(k*M);
    end
end

hold on
t = (0:length(data)-1)/fs;
plot(t,data, 'color', 'black');
plot(t,y, 'color', 'b');
legend("Señal de entrada", "Señal de salida");
xlabel("Tiempo (s)"); ylabel("Amplitud");
hold off
toc
soundsc(data, fs);
pause(8);
soundsc(y,fs);

