function X = fft_stage(x)
    if (length(x) == 2)
        X = FFT2(x);
    else
        N = length(x);
        n = 0:N/2-1;
        x_par = x(2*n+1);
        x_impar = x(2*n+2);
        w = exp(-j*2*pi*n/N);
    
        X_par = fft_stage(x_par);
        X_impar = fft_stage(x_impar);
        X = zeros(1, N);
        X(1:N/2) = X_par + w .* X_impar;
        X(N/2+1:N) = X_par - w .* X_impar;
        %X = [fft_stage(x11) + w.*fft_stage(x12), fft_stage(x11) - w.*fft_stage(x12)];
    end
end