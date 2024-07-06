function X = FFT8(x)
    x11 = x([1,3,5,7]);
    x12 = x([2,4,6,8]);
    w = exp(-j*2*pi*(0:3)/8);
    X = [FFT4(x11) + w.*FFT4(x12), FFT4(x11) - w.*FFT4(x12)];
end