function [X] = DTFTsum(x, fw)
N = length(x);
w = -pi:1/fw:pi;
X = zeros(1, length(w));
for  n = 0:N-1
    X = X + x(n+1)*exp(-j*w*n);
end
end