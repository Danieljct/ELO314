function y = sinc(arg)
    N = length(arg);
    y = sin(arg*pi)./(arg*pi);
    if arg == 0
        y = 1;
    end
end