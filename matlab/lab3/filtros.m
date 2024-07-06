B = fir1(200, .4);
freqz(B, 1);

%% IIB3
B = fir2(150, [0 0.1 0.15 .35 .4 .65 .75 1],  [1 1 0 0 0.5 0.5 0 0]);
[H, w] = freqz(B, 1);
plot(w/pi, abs(H));
ylim([ -0.2 1.2])
grid;
f_print (14,6, "Filtro FIR de orden 150", 10, 'Serif','Frecuencia Normalizada','Amplitud',"filtrob3150",1);
%%
B = firpm(100,[0 .3 .305 1],[1 1 0 0]);
[H, w] = freqz(B, 1);
plot(w/pi, abs(H));
grid;
f_print (14,6, "Filtro pm de orden 100", 10, 'Serif','Frecuencia Normalizada','Amplitud',"filtroequir",1);
%% III 1) a)
fs = 16000;
wn = 2000/(fs/2);
[B, A] = ellip(2, 3, 1000,wn);
freqz(B, A);

%% b)
fs = 16000;
wn = 4000/(fs/2);
[B, A] = ellip(2, 3, 1000,wn,"high");
freqz(B, A);
%% c)
fs = 16000;
wn1 = 2000/(fs/2);
wn2 = 4000/(fs/2);
[B, A] = ellip(2, 3, 100,[wn1, wn2]);
freqz(B, A);
%% d)
fs = 16000;
wn1 = 2000/(fs/2);
wn2 = 4000/(fs/2);
[B, A] = ellip(2, 3, 100,[wn2, wn1], 'stop'); 
freqz(B, A);

%% III 2) 
fs = 16000;
wn1 = 2000/(fs/2);
wn2 = 4000/(fs/2);
[B, A] = cheby1(2, 1.5,[wn2, wn1]); 
freqz(B, A);
%% cheby 2
fs = 16000;
wn1 = 2000/(fs/2);
wn2 = 4000/(fs/2);
[B, A] = cheby2(2, 20,[wn2, wn1]); 
freqz(B, A);
%% III 3)

fs = 16000;
wn1 = 800/(fs/2);
wn2 = 1600/(fs/2);
[B, A] = butter(2,[wn2, wn1]);
[z,p,k] = butter(2,[wn2, wn1]);
%freqz(B, A);
% a medida que crece el orden, loscoeficientes del A van creciendo y el B
% se achican
subplot 131
zplane(z,p);
xlim([-2 2]); ylim([-2 2]);
subplot 132
[z,p,k] = butter(4,[wn2, wn1]);
zplane(z,p);
xlim([-2 2]); ylim([-2 2]);
subplot 133
[z,p,k] = butter(8,[wn2, wn1]);
zplane(z,p);
xlim([-2 2]); ylim([-2 2]);
%% IV 1) butter
fs = 16000;
wp = 3200/(fs/2);
wsp = 4000/(fs/2);
Rp = 0.5;
Rs = 40;
[n, wn] = buttord(wp, wsp, Rp, Rs)
[n, wn] = cheb1ord(wp, wsp, Rp, Rs)
[n, wn] = ellipord(wp, wsp, Rp, Rs)
[n,fo,mo,w] = firpmord( [3200 4000], [1 0], [Rp/10 Rp/50], fs)

% b = firpm(n,fo,mo,w);
% freqz(b, 1, 1024, fs);

%[B, A] = firpm(4,fo,mo,w);freqz(B, A);