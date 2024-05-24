% Ejemplo de cómo emular en matlab el comportamiento del DSP operando
% muestra a muestra.
close all
clear, clc
%% Selección del audio
audio_path = '..\..\audio_files\wav\sonidos_voz_4kBw_16_16.wav';
[x, Fs] = audioread(audio_path);
soundsc(x, Fs)
pause(3)
%% 'Trampa' en Matlab usando operaciones vectoriales
maxSatVal = 0.3;

y = min( max(x, -maxSatVal), maxSatVal);

figure(1)
plot(x)
grid on
hold on
plot(y)
hold off
xlabel('Muestras')
ylabel('Amplitudes de entrada y salida')
title('Función vectorial en Matlab')

%% Emulación muestra a muestra
inputVector = x;

outputVector = processAsSampleBased( @mySaturation , inputVector, maxSatVal);
%outputVector = processAsSampleBased( @myMa4 , inputVector);

%% Gráficos
figure(2)
plot(inputVector)
grid on
hold on
plot(outputVector)
hold off
xlabel('Muestras')
ylabel('Amplitudes de entrada y salida')
title('Emulación de operación muestra a muestra en Matlab')

soundsc(outputVector,Fs)