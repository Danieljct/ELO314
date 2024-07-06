function y = FIR_filter(x,fn)
th = fn*pi*2;
coeff_b = zeros(1,3);

coeff_b(1) = 1;
coeff_b(2) = -2*cos(th);
coeff_b(3) = cos(th)^2+sin(th)^2;

y = conv(x, coeff_b);
end