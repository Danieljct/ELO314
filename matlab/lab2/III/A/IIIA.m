%% 1) a
th = pi/2; % variar pi/6, pi/3, pi/2
coeff_b = zeros(1,3);

coeff_b(1) = 1;
coeff_b(2) = -2*cos(th);
coeff_b(3) = cos(th)^2+sin(th)^2;
coeff_a = [1 0 0];
freqz(coeff_b, coeff_a);

%

%% b
tf_ = tf(coeff_b, coeff_a, -1)
h = impulse(tf_);

scatter(1:length(h),h);
title("Respuesta impulso")
xlabel("Muetras (n)"); ylabel("Amplitud");

%% 2)
load('nspeech.mat', 'fs', 'nspeech');

x = nspeech;
Fn = 1685; %frecuencia no
Fs = fs; %frecuencia de sampleo

y = FIR_filter(x, Fn/Fs);
pspectrum(x, Fs);
pspectrum(y, Fs);





