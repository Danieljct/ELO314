Fs_Hz = 8000;

soundsc(Datafloat, Fs_Hz)

phaser_8ksps_noise = Datafloat;
phaser_8ksps_impulses = Datafloat;
phaser_8ksps_impulses2 = Datafloat;


save('phaser_samples.mat','phaser_8ksps_impulses3','Fs_Hz', '-append')

save('phaser_samples.mat','phaser_8ksps_noise', '-append')

soundsc(phaser_8ksps_noise, Fs_Hz)

soundsc(phaser_8ksps_impulses2, Fs_Hz)

figure, spectrogram(phaser_8ksps_noise, 256, [], [], Fs_Hz, 'yaxis')
figure, spectrogram(phaser_8ksps_impulses2, 256, [], [], Fs_Hz, 'yaxis')

audiowrite('E:\repositories\repo_labdsp_elo314_utfsm\audio_files\wav\phaser_noise_16_8.wav',phaser_8ksps_noise,Fs_Hz);

audiowrite('E:\repositories\repo_labdsp_elo314_utfsm\audio_files\wav\phaser_impulses2_16_8.wav',phaser_8ksps_impulses2,Fs_Hz);

phaser_8ksps_impulses3 = resample(phaser_8ksps_impulses2, 44100, 8000);


audiowrite('E:\repositories\repo_labdsp_elo314_utfsm\audio_files\wav\phaser_8ksps_impulses_16_44p1.wav',phaser_8ksps_impulses3,44100);