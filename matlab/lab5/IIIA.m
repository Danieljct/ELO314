load('test_training_signals.mat');
x = test_signal;
N = length(x); 
t = (1:N)/fs;
ventana = .020*fs;
nuevox = [];
nuevorms = [];
%soundsc(x,fs)


for i=1:ventana:N
    if i+ventana<N
        segmento = x(i:i+ventana-1);
        segmento_rms = rms(segmento);
        segmento_zc = zeroCrossings(segmento,fs);
        h((i-1)/ventana+1,:) = mylpc(segmento,15);
        if segmento_rms < .01
            VUS((i-1)/ventana+1) = 0;
        elseif segmento_zc > 29.8013*(segmento_rms-0.1)+4.2
            VUS((i-1)/ventana+1) = -1;
        else
            VUS((i-1)/ventana+1) = 1;
        end  
    else
        segmento = x(i:end);
        segmento_rms = rms(segmento);
        segmento_zc = zeroCrossings(segmento,fs);
        if segmento_rms < .01
            VUS((i-1)/ventana+1) = 0;
        elseif segmento_zc > 29.8013*(segmento_rms-0.1)+4.2
            VUS((i-1)/ventana+1) = -1;
        else
            VUS((i-1)/ventana+1) = 1;
        end
    end
    nuevorms = [nuevorms segmento_rms];
end

sintetizada = [];

for i=1:ventana:N
    if i+ventana<N
        if VUS((i-1)/ventana+1) == 1
            X = excite(ventana, fs/100);
            y = filter(1,h((i-1)/ventana+1,:),X)*nuevorms((i-1)/ventana+1);
    
        elseif VUS((i-1)/ventana+1) == -1
            X = rand(1,ventana)-0.5;
            y = filter(1,h((i-1)/ventana+1,:),X)*nuevorms((i-1)/ventana+1);
        else
            y = zeros(1,ventana);
    
        end
    else
        tamanho = length(x(i:end));
        if VUS((i-1)/ventana+1) == 1
            X = excite(tamanho, fs/100);
            y = filter(1,h((i-1)/ventana+1,:),X)*nuevorms((i-1)/ventana+1);
    
        elseif VUS((i-1)/ventana+1) == -1
            X = 2*rand(1,tamanho)-1;
            y = filter(1,h((i-1)/ventana+1,:),X)*nuevorms((i-1)/ventana+1);
        else
            y = zeros(1,tamanho);
    
        end
    end
    sintetizada = [sintetizada, y];

end
soundsc(sintetizada, fs)
subplot 211
plot(t,x)
f_print(20,12,"Señal original",10,'Serif',"Tiempo (s)","Amplitud","figuraIIIA1",0);
subplot 212
plot(t,sintetizada*5)
f_print(20,12,"Señal sintetizada",10,'Serif',"Tiempo (s)","Amplitud","figuraIIIA1",1);
audiowrite("my_test_signal.wav",sintetizada,fs);
