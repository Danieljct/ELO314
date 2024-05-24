function y = myMa4(x)
% Declaraciones de variables peristentes (equivalente a 'static' en
% lenguaje C), que la función recordará en siguientes llamados.
persistent oldSamples
if isempty(oldSamples)
    oldSamples = [0,0,0];
end
% Promedio movil de las 4 muestras más recientes
y = (x + oldSamples(1) + oldSamples(2) + oldSamples(3)) / 4;
% Actualiza buffer lineal
oldSamples(3) = oldSamples(2);
oldSamples(2) = oldSamples(1);
oldSamples(1) = x;

return