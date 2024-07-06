function [x,rango] = cropAudio(audioSignal, Fs)
    t = 0:1/Fs:(length(audioSignal)-1)/Fs;
    plot(t, audioSignal, 'color', 'b');
    set(gcf, 'Position', [1, 1, 1920, 1080]);
    lim = ginput(2);
    if(lim(2,1)<lim(1,1))
        lim = [lim(2,:);lim(1,:)];
    end
    rango = int64(lim(1,1)*Fs):int64(lim(2,1)*Fs);
    plot(t(rango),audioSignal(rango));
    soundsc(audioSignal(rango),Fs);
    x = audioSignal(rango);
end