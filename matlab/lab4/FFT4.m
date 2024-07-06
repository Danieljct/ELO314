function X = FFT4(x)
    x11 = x([1,3]);
    x12 = x([2,4]);
    w = exp(-j*2*pi*(0:1)/4);
    X = [FFT2(x11) + w.*FFT2(x12), FFT2(x11) - w.*FFT2(x12)];
end