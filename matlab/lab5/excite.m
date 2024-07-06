function X = excite(N,Np)
X = zeros(1,N);
for i=0:N-1
    if mod(i,Np) == 0
        X(i+1) = 1;
    end
end
