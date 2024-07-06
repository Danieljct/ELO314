a = 0.1; b = 0.05; Gi = 1; Go = 1;
i2 = -.5:1e-3:.5;
yt = zeros(1,length(i2));

for i = 1:length(i2)
    x = i2(i);
    x_ = Gi * x;
  
    if (abs(x_) >= a)
        y_ = b*x_+sign(x_)*(1 - b)*a;
    else
        y_ = x;
    end
    y = Go*y_;
    yt(1,i) = y;
end

plot(i2,yt);