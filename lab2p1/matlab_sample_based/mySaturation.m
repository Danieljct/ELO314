function y = mySaturation(x, max)
% Función SISO de ejemplo para emular procesar muestra a muestra
%--------------------------------------------------------------------------

% Santity check
if ( ~isequal(size(x), [1,1]) )
    fprintf(2,'Error: x debe ser de dimensión [1,1]\n')
    return
end
if ( ~isequal(size(max), [1,1]) )
    fprintf(2,'Error: max debe ser de dimensión [1,1]\n')
    return
end    

%--------------------------------------------------------------------------
% Función de prueba: saturación simétrica
if ( x > max )
    y = max;
else if ( x < (-max) )
        y = -max;
    else
        y = x;
    end
end
%--------------------------------------------------------------------------
    
return

