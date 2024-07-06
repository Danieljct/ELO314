function X = DFTmatrix(x)
    N = length(x);
    A = genAmatrix(N);
    X=A*x;
end