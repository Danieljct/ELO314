clear, clc, close all

% Observando detalle de interpolación
Fs = 1000;
numOfSamples = 20;

signal = 2;

switch signal
    case 1
        % Delta Kroneker
        x = zeros(numOfSamples,1); 
        x(1) = 1;        
    case 2
        x = sin(2*pi*(1/3)*(0:numOfSamples-1) + 1/8) ;  
    case 3
        % Señal aleatoria
        x = rand(numOfSamples,1);        
    otherwise
        warning('Opción inexistente')
end    


% Interpolación
p = 5;
[yUpSampled, y, gd] = myInterpolation(x,p);

% Muestras iniciales del retardo de grupo son eliminadas
y2 = y;

% Etiquetado con los instantes en tiempo absoluto a cada muestra
t_x = (0:1/Fs:(1/Fs)*(length(x)-1));
t_y = (0:1/(p*Fs):(1/(p*Fs))*(length(y)-1));
t_y2 = (0:1/(p*Fs):(1/(p*Fs))*(length(y2)-1));

xmax = (length(x)+1)/Fs;
%--------------------------------------------------------------------------
figure('Units','normalized','Position',[0.5 0.05 0.6 0.8])
subplot 311
plot(t_x,x,'ro','Linewidth',2,'MarkerSize',5), grid on
hold on
stem(t_y,yUpSampled,'b.','MarkerSize',10)
hold off
xlim([0 xmax]);
%
str_title = sprintf('Upsampling por factor p = %d, Fs = %.1f Hz',p,Fs);
title(str_title)
xlabel('Tiempo /s')
ylabel('Amplitud')
%--------------------------------
subplot 312
stem(t_y,y,'b.','MarkerSize',10), grid on
xlim([0 xmax]);
%
str_title = sprintf('Interpolación por factor p = %d',p);
title(str_title)
xlabel('Tiempo /s')
ylabel('Amplitud')
%--------------------------------
subplot 313
plot(t_x,x,'ro','Linewidth',2,'MarkerSize',5), grid on
hold on
plot(t_y2-gd/Fs/p,y2,'b.','MarkerSize',10)
hold off
xlim([0 xmax]);
%
str_title = sprintf('Superposición de señales original e interpolación');
title(str_title)
xlabel('Tiempo /s')
ylabel('Amplitud')
