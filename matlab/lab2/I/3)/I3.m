[data, fs] = audioread('gtr-jazz_16_48.wav');
cropAudio(data(:,1), fs, 'Lab2p1_arpegio.wav');