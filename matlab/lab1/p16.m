t = 0:2:59;
y = sin(t/6);
subplot 311

stem(t,y, 'color', 'B', 'LineWidth',1)
xlabel('Tiempo (s)')
ylabel('Amplitud')
title('Stem')
grid on
subplot 312
stairs(t,y,'color','r','LineWidth',2)
xlabel('Tiempo (s)')
ylabel('Amplitud')
title('Stairs')
grid on

subplot 313
hold on
plot(t,y,'color', 'g', 'LineWidth',1.5)
xlabel('Tiempo (s)')
ylabel('Amplitud')
title('Plot')
hold off