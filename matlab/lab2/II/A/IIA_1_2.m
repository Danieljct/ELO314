%% 1-2
[data, fs] = audioread('gtr-jazz_16_48.wav');

a = 0.2; b = 0.05; Gi = 1; Go = 1;

data = data(:,1);

yt = zeros(1,length(data));

for i = 1:length(data)
    x = data(i);
    x_ = Gi * x;  
    if (abs(x_) >= a)
        y_ = b*x_+sign(x_)*(1 - b)*a;
    else
        y_ = x;
    end
    y = Go*y_;
    yt(1,i) = y;
end

t = (0:length(data)-1)/fs;
hold on
plot(t, data, 'color', 'black');
plot(t, yt, 'color', 'r');
legend("Señal original", "Señal con distorisión")
hold off;

soundsc(data,fs);
pause(8);
soundsc(yt,fs);

