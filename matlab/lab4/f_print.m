function f_print(width, height, titulo, fontsize, fuente, ejex, ejey, nombrepdf, export)
set(gcf, 'Units', 'centimeters', 'Position', [1, 1, width, height]);
set(gca, 'FontSize', fontsize, 'FontName', fuente);

title(titulo,'FontSize', fontsize,'FontName', fuente);
xlabel(ejex,'FontSize', fontsize,'FontName', fuente); ylabel(ejey,'FontSize', fontsize,'FontName', fuente);
if (export)
    exportgraphics(gcf, nombrepdf+'.pdf', 'ContentType', 'vector');
end
end