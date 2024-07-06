function [X] = DFTsum(x)
N = length(x);
k = 0:N-1;
X = zeros(1, N);
for  n = 0:N-1
    X = X + x(n+1)*exp(-j*2*pi*k*n/N);
end
end