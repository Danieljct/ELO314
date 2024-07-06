function X = DFTdc(x)
    N = length(x);
    n = 0:N/2-1;
    pares = x(2*n+1)
    impares = x(2*n+2)
    
    PARES = DFTsum(pares);
    IMPARES = DFTsum(impares);
    w=exp(-j*2*pi*n/N);
    X = [PARES+w.*IMPARES, PARES-w.*IMPARES];
end