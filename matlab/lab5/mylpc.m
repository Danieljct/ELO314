function a = mylpc(x,p)
    rx = xcorr(x);
    n = length(x);
    rx = rx(n:end);
    Rx = toeplitz(rx(1:p));
    a = Rx\(rx(2:p+1));
    a = [1;-a];
end