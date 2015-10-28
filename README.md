# SoundSquare

## Install
1. Install a midi server. I used [fluidsynth](http://www.fluidsynth.org/).
2. Install [openFrameworks](http://www.openframeworks.cc/) and the ofxMidi extension.
3. Point hw3 to your openFrameworks installation in `config.make`

## Run
1. Run your midi server. For me, I run fluidsynth and point it to the correct sound card with: `fluidsynth --server --audio-driver=alsa -o audio.alsa.device=hw:1,0 /usr/share/sounds/sf2/FluidR3_GM.sf2`
2. Run `./bin/hw3`
