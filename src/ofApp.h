#pragma once

#include "ofMain.h"
#include "ofxMidi.h"
#include <bitset>

#define X 16
#define Y 16

#define N_INSTRUMENTS 8

class ofApp : public ofBaseApp {

	public:
		void setup();
		void update();
		void draw();
		void exit();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
	

private:
	ofxMidiOut midiOut;

	float tempo = 150;
	float p = 60. / tempo;
	float mp = X * p;

	bitset<N_INSTRUMENTS> notes[X][Y] = {{0}};
	float lmt; // measure time of last update

	char instruments[N_INSTRUMENTS] = {
		11,
		46,
		38,
		52,
		79,
		88,
		116,
		55
	};
	ofColor instrumentColors[N_INSTRUMENTS];
	char currentInstrument = 0;

	// Remember last note on times so we can send note offs correctly
	float lastNoteOn[Y][N_INSTRUMENTS] = {{FLT_MAX}};

	bitset<N_INSTRUMENTS> mutes = 0;

	bool sustain = false;

	int w, h, dim;

	ofPoint getDot(int x, int y);
	ofPoint getDotPosition(int x, int y);
};
