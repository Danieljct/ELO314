function Fc = triangle(xFc, maxf, minf, Fw,Fs)
periodo = (maxf-minf)/(Fw/2)*Fs;
    if(mod(xFc,periodo)<periodo/2)
        Fc = Fw*mod(xFc,periodo)/Fs + minf;
    else
        Fc = -Fw*(mod(xFc,periodo)-periodo/2)/Fs + maxf;
    end
end