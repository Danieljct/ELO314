function y = IIR_filter(x, coeff_b, coeff_a)

y = zeros(1,length(x));
for i = 1:length(x)
    if(i==1)
        y(i) = coeff_b(1)*x(i);
    elseif (i==2)
        y(i) = coeff_b(1)*x(i) - coeff_a(2)*y(i-1);
    else
        y(i) = coeff_b(1)*x(i) - coeff_a(2)*y(i-1) - coeff_a(3)*y(i-2);
    end
end
end