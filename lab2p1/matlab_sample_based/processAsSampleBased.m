function vectorOut = processAsSampleBased( funcIn, vectorIn, varargin )
% Santity check
if ( ~isvector( vectorIn ) )
    fprintf(2,'Error: vectorIn debe ser un vector\n')
    return
end

%--------------------------------------------------------------------------
% Ejecución de función SISO para cada muestra del vector de entrada
% Preparando vector de salida
vectorOut = zeros( size( vectorIn ) );

% Iteración muestra a muestra: ¡Este ciclo 'for' simula el paso del tiempo!
for i=1:length(vectorIn)
    %----------------------------------------------------------------------
    % Zona de código equivalente a la interrupción en el DSP
    %----------------------------------------------------------------------
    % Aquí se llama la función normalmente para procesar
    % una muestra por vez
    %fprintf ( 'Procesando muestra %d\n', i );
    vectorOut(i) = funcIn( vectorIn(i), varargin{:} );
    %----------------------------------------------------------------------
end
return