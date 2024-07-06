function cropAudio(audioSignal, Fs, croppedAudioFilename)
    t = 0:1/Fs:(length(audioSignal)-1)/Fs;
    plot(t, audioSignal, 'color', 'b');
    lim = ginput(2);
    if(lim(2,1)<lim(1,1))
        lim = [lim(2,:);lim(1,:)];
    end
    rango = int64(lim(1,1)*Fs):int64(lim(2,1)*Fs);
    plot(t(rango),audioSignal(rango));
    audiowrite(croppedAudioFilename,audioSignal(rango),Fs);
end