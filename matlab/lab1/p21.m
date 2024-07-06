Ts = [1/10, 1/3, 1/2, 10/9];
 
for i = 1:4
    subplot (2,2,i);
    n = 0:1:1/Ts(i)*10;
    t = 0:0.01:1/Ts(i)*10;
    s = sin(2*pi*Ts(i)*n);
    hold on
    stem(n,s, 'color', 'B', 'LineWidth',1);
    plot(t, sin(2*pi*t*Ts(i)), '--', 'color' ,'r');
    legend("Señal muestreada","Señal continua");
    hold off
   
    xlim([0,1/Ts(i)*10]);
    ylim([-1,1]);
    xlabel('Número de muestra (n)');
    ylabel('Amplitud');
    titulo = sprintf("Período de muestreo: %.2f", Ts(i));
    title(titulo);

end