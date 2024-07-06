function A = genAmatrix(N)
A = zeros(N,N);
for n = 0:N-1
    for k = 0:N-1
        A(k+1,n+1) = exp(-j*2*pi*k*n/N);       
    end
end
end