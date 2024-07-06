N = 1001;
h = zeros(1,N);
wc = 2*pi/3; 
for i=0:N-1
    h(i+1) = wc/pi*sinc(wc/pi*(i-(N-1)/2));
end

%plot(-(N-1)/2:(N-1)/2,h)
M = 1024;
[X,w] = DTFT(h,M);
subplot 121 
plot(w,abs(X));
xlim([-pi pi])
f_print (18,8, "|H(\omega)|", 10, 'Serif',"Frecuencia (rad/sample)","Magnitud","ventanarecN21",0);
subplot 122
plot(w,angle(X));
xlim([-pi pi])
%f_print (18,6, "\angle H(\omega)", 10, 'Serif',"Frecuencia normalizada (rad/sample)","Ángulo (rad)","ventanarecN1001",1);
%% 2)
fs = 1e3; 
t = 0:1/fs:.1;
hold on
plot(1000*t,rectwin(length(t)));
plot(1000*t,hann(length(t)));
plot(1000*t,hamming(length(t)), "Color",'black');
plot(1000*t,blackman(length(t)),"Color", [0x2d 0x57 0x2c]);
plot(1000*t,bartlett(length(t)), 'm');
legend("rectwin", "hanning", "hamming", "blackman", "bartlett");
hold off
grid on
%f_print (16,6, "Ventanas en tiempo", 10, 'Serif',"Tiempo (ms)","Magnitud","ventanastemp",1);
%%
M = 2048;
hold on
[X, w] = DTFT(rectwin(length(t)),M);
plot(w,20*log(abs(X)));
[X, w] = DTFT(hann(length(t)),M);
plot(w,20*log(abs(X)));
[X, w] = DTFT(hamming(length(t)),M);
plot(w,20*log(abs(X)), 'black');
[X, w] = DTFT(blackman(length(t)),M);
plot(w,20*log(abs(X)));
[X, w] = DTFT(bartlett(length(t)),M);
plot(w,20*log(abs(X)));
ylim([-120, 100]); xlim([-0.3, 0.3])
legend("rectwin", "hanning", "hamming", "blackman", "bartlett");
grid on;
hold off
%f_print (20,9, "Ventanas en tiempo", 10, 'Serif',"Frecuencia (rad/s)","Magnitud (dB)","ventanasfrec",1);
