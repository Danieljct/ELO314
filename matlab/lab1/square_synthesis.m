%% Síntesis de señal cuadrada por suma de armónicos de baja frecuencia
close all
clear, clc

fs = 8000;
f0 = 100;
timeSpan = 1;
% 
 squareSignal1 = squareSynth(fs, f0, timeSpan, 5, 0); % Sin fase aleatoria
 squareSignal2 = squareSynth(fs, f0, timeSpan, 5, 360); % Mayor variablidad

% Graficos
subplot 211
plot(0:1/fs:(1-1/fs),squareSignal1);
xlim([0, 0.03]);
    xlabel('Tiempo (s)');
    ylabel('Amplitud');
    title("Señal sin fase aleatoria");

subplot 212
plot(0:1/fs:(1-1/fs),squareSignal2);
xlim([0, 0.03]);
    xlabel('Tiempo (s)');
    ylabel('Amplitud');
    title("Señal con fase aleatoria");
% Producción de sonido
sound(0.5*squareSignal1, fs)
pause(1);
sound(0.5*squareSignal2, fs)
%%
function squareSignal = squareSynth( fs_Hz, f0_Hz, timeSpan_s, numberOfOddHarmonics, randPhaseRange_degrees )
    % PARAMETROS
    % fs_Hz                 : > 0
    % f0_Hz                 : > 0
    % timeSpan_s            : > 0
    % numberOfOddHarmonics  : entero >= 0
    % randPhaseRange_degrees: 0 a 360 grados

    % Número de muestras resultantes
    N = floor(timeSpan_s * fs_Hz);
    % Tiempo
    t = (0:(N-1))./fs_Hz;
    % Ajuste de fase de armónicos IMPARES SOLAMENTE
    numberOfTones = numberOfOddHarmonics + 1;
    harmonics = (1:2:2*numberOfTones-1)';
    freqs = harmonics * f0_Hz;
    % Cambio de fases
    phase_degrees = randPhaseRange_degrees * rand(numberOfTones,1);
    phase_rad = (pi/180)*phase_degrees;
    % Amplitudes
    amplitudes = 1./harmonics;
    % Síntesis
    tones = amplitudes.*sin(2*pi*freqs*t + phase_rad);
    squareSignal = sum(tones,1);
end