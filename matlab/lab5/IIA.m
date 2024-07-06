load('test_training_signals.mat');
x = test_signal;
N = length(x);
t = (1:N)/fs;

[y,rango] = cropAudio(x,fs);
letra = rango;

%% 
load('test_training_signals.mat');
load('letras.mat');
x = training_signal;
N = length(x);
t = (1:N)/fs;
figure;
hold on
plot(t,x,'b');
% s
plot(t(s),x(s),'r')
% e
plot(t(e),x(e),'g')
% ñ
plot(t(nh),x(nh),'c')
% a
plot(t(a),x(a),'m')
% l
plot(t(l),x(l),'y')
% t
plot(t(t1),x(t1),'Color','#CC6633')
% m
plot(t(m),x(m),'Color','#336699')
% p
plot(t(p),x(p),'Color','#CCFF00')
% o
plot(t(o),x(o),'Color','#7f8c8d')
% r
plot(t(r),x(r),'Color','#154360')

%s
plot(t(s2),x(s2),'r')
plot(t(s3),x(s3),'r')
%e
plot(t(e2),x(e2),'g')
plot(t(e3),x(e3),'g')
plot(t(e4),x(e4),'g')
%a
plot(t(a2),x(a2),'m')
%l
plot(t(l2),x(l2),'y')

legend("Silencio","S","E","Ñ","A","L","T","M","P","O","R")
f_print(20,12,"Segmentos de señal para cada sonido",10,'Serif',"Tiempo (s)","Magnitud","figuraIIA1",1);

%%
load('test_training_signals.mat');
x = training_signal;
letras = load("letras.mat");
matrices = fieldnames(letras);
letras_rms = zeros(1,length(matrices));
cruces_zero = zeros(1,length(matrices));

for i = 1:length(matrices)
    varName = matrices{i};
    matrix = letras.(varName);
    letras_rms(i) = rms(x(matrix));
    cruces_zero(i) = zeroCrossings(x(matrix),fs);
end

V = [1,2,3,4,5,6,10,11,12,13,14,16];
U = [15,17,18,19,20];
S = [7,8,9];

sonidos = [V,U,S];
figure;
hold on

scatter(letras_rms(V),cruces_zero(V),'r',"filled");
scatter(letras_rms(U),cruces_zero(U),'g',"filled");
scatter(letras_rms(S),cruces_zero(S),'b',"filled");

x = 0.0094:1e-3:0.15;
y = 29.8013*(x-0.1)+4.2;
xline(0.01);
plot(x, y, '-c', 'LineWidth', 1)
legend("Con vibración","Sin vibración","Silencio","Umbral Silencio","Umbral vibración")
f_print(18,12,"Nube de puntos y clasificación",10,'Serif',"Valor RMS","Cruces por cero por ms","figuraIIA1_2",1);

hold off
%%

load('test_training_signals.mat');
x = test_signal;
N = length(x); 
t = (1:N)/fs;
ventana = .020*fs;
nuevox = [];
nuevorms = [];
soundsc(x,fs)


for i=1:ventana:N
    if i+ventana<N
        segmento = x(i:i+ventana-1);
        segmento_rms = rms(segmento);
        segmento_zc = zeroCrossings(segmento,fs);
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
    nuevox = [nuevox ones(1,length(segmento))*VUS((i-1)/ventana+1)];
    nuevorms = [nuevorms ones(1,length(segmento))*segmento_rms];
end

subplot 311
plot(t,x);
f_print(20,18,"Señal original",10,'Serif',"Tiempo (s)","Magnitud","figuraIIA2",0);
subplot 312
plot(t,nuevox);
f_print(20,12,"Clasificación VUS",10,'Serif',"Tiempo (s)","VUS","figuraIIA2",0);
ylim([-1.1 1.1])
subplot 313
hold on
plot(t,nuevorms);
yline(0.0094)
legend("Valor RMS", "Límite del silencio")
f_print(20,12,"Valor RMS",10,'Serif',"Tiempo (s)","Magnitud","figuraIIA2",1);
