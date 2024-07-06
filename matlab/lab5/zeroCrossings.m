function y = zeroCrossings(x,fs)
    N = length(x);
    cruces = 0;
    for i = 2:N
        if (x(i)*x(i-1)<0)
            cruces = cruces + 1; 
        elseif (x(i)*x(i-1)==0)
            cruces = cruces + 1/2;
        end
    end
    y = cruces/(N*1000/fs);
end