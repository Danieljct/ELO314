t = 0:2:59;
y = sin(t/6);
subplot 311

stem(t,y, 'color', 'B', 'LineWidth',1)
xlabel('Tiempo (s)')
ylabel('Amplitud')
title('Frecuencia de muestreo 0.5, stem')
grid on
subplot 312
plot(t,y,'color','r','LineWidth',2)
xlabel('Tiempo (s)')
ylabel('Amplitud')
title('Frecuencia de muestreo 0.5, plot')
grid on

tt = 0:10:59;
yy = sin(tt/6);
subplot 313
hold on
plot(tt,yy,'color', 'g', 'LineWidth',1.5)
stem(tt,yy,'color', [1,0,1], 'LineWidth', 2)
xlabel('Tiempo (s)')
ylabel('Amplitud')
title('Frecuencia de muestreo 0.1')
legend('Plot', 'Stem')