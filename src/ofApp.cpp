#include "ofApp.h"
#include <cctype>

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetVerticalSync(true);
	ofBackground(0);
	ofSetLogLevel(OF_LOG_VERBOSE);

	// print the available output ports to the console
	midiOut.listPorts(); // via instance
	//ofxMidiOut::listPorts(); // via static too

	// connect
	midiOut.openPort(1); // by number
	//midiOut.openPort("IAC Driver Pure Data In"); // by name
	//midiOut.openVirtualPort("ofxMidiOut"); // open a virtual port

	printf("BPM: %f\n", (double)tempo);
	printf("Seconds per note: %f\n", p);

	for (int i = 0; i < N_INSTRUMENTS; i++) {

		// Set instrument for each channel
		midiOut.sendProgramChange(i+1, instruments[i]);

		// Set color for each instrument
		instrumentColors[i] = ofColor::fromHsb(i * 255. / N_INSTRUMENTS, 255, 255);
	}

	ofPoint windowSize = ofGetWindowSize();
	windowResized(windowSize.x, windowSize.y);
}

// Returns notes for a pentatonic scale.
int getMidiBase(int base) {
	switch(base) {
		case 0: return 49;
		case 1: return 51;
		case 2: return 54;
		case 3: return 56;
		case 4: return 58;
	}
	return -1;
}

int getNote(int y) {
	y = Y - 1 - y; // map 0-15 --> 15-0
	int octave = y / 5;
	int base = y % 5;
	return getMidiBase(base) + 12 * octave;
}

//--------------------------------------------------------------
void ofApp::update() {
	float t = ofGetElapsedTimef();
	float mt = fmod(t, mp);
	if (floor(mt / p) != floor(lmt / p)) {
		int x = floor(mt / p);
		for (int y = 0; y < Y; y++) {
			for (int i = 0; i < N_INSTRUMENTS; i++) {
				if (lastNoteOn[y][i] < t - p && !sustain)
					midiOut.sendNoteOff(i+1, getNote(y), 0);
				if (notes[x][y].test(i) && !mutes.test(i)) {
					midiOut.sendNoteOn(i+1, getNote(y), 127);
					lastNoteOn[y][i] = t;
				}
			}
		}
	}
	lmt = mt;
}

ofPoint ofApp::getDotPosition(int x, int y) {
	float xpos = dim * (x - (X-1.)/2) / X + w/2;
	float ypos = dim * (y - (Y-1.)/2) / Y + h/2;
	return ofPoint(xpos, ypos);
}

//--------------------------------------------------------------
void ofApp::draw() {
	float t = ofGetElapsedTimef();
	float mt = fmod(t, mp);

	float baseRadius = dim / (5*X);
	float tf = 0.2 * sin(2 * M_PI * mt / (8 * p));

	ofFill();
	// Draw hover indicator
	ofPoint hoverDot = getDot(mouseX, mouseY);
	int hoverX = round(hoverDot.x), hoverY = round(hoverDot.y);
	if (hoverX >= 0 && hoverX < X &&
		hoverY >= 0 && hoverY < Y) {
		ofPoint hoverDotPos = getDotPosition(hoverX, hoverY);
		ofSetColor(63,63,63);
		ofCircle(hoverDotPos.x, hoverDotPos.y, baseRadius*1.2);
	}

	// Draw dots
	for (int x = 0; x < X; x++) {

		float age = fmod(mt - x * p, mp);
		if (age < 0) age += mp;

		float f = max(0., 1. - age/p);

		for (int y = 0; y < Y; y++) {
			ofPoint dotPos = getDotPosition(x, y);

			float angleBegin = 360 * mt / mp;
			float angle = 360. / notes[x][y].count();

			float radius = baseRadius * (1 + tf) * (1 + 0.6 * f) * sqrt(notes[x][y].count());

			for (int i = 0; i < N_INSTRUMENTS; i++) {
				if (notes[x][y].test(i)) {
					ofColor color = instrumentColors[i] * (1 - tf) * (0.6 + 0.5 * f);
					ofPath sector;
					sector.setFillColor(color);
					sector.arc(dotPos.x, dotPos.y, radius, radius, angleBegin, angleBegin + angle);
					sector.draw();
					angleBegin += angle;
				}
			}

			if (notes[x][y].none()) {
				ofSetColor(63,63,63);
				ofCircle(dotPos.x, dotPos.y, baseRadius * 0.8);
			}
		}
	}

	// Draw channel activity status
	for (int i = 0; i < N_INSTRUMENTS; i++) {
		ofColor color = instrumentColors[i];
		ofSetColor(color);
		float baseRadius = dim / (4*X);
		float radius = baseRadius;
		if (mutes.test(i)) {
			ofNoFill();
			radius = baseRadius * 0.8;
		}
		else {
			ofFill();
			radius = baseRadius * (1 + tf / 2);
		}
		ofCircle((w - dim) / 2 - 4 * baseRadius,
				 (h - dim) / 2 + dim * (i+1.f) / (N_INSTRUMENTS + 1),
				 radius);
	}

	// Draw current channel
	ofNoFill();
	ofColor currentColor = instrumentColors[currentInstrument];
	ofSetColor(currentColor);
	ofCircle((w - dim) / 2 - dim / X,
			 (h - dim) / 2 + dim * (currentInstrument+1.f) / (N_INSTRUMENTS + 1),
			 dim / (2*X));
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch (key) {
		case 'a':
			currentInstrument = 0;
			break;
		case 's':
			currentInstrument = 1;
			break;
		case 'd':
			currentInstrument = 2;
			break;
		case 'f':
			currentInstrument = 3;
			break;
		case 'g':
			currentInstrument = 4;
			break;
		case 'h':
			currentInstrument = 5;
			break;
		case 'j':
			currentInstrument = 6;
			break;
		case 'k':
			currentInstrument = 7;
			break;
		case 'A':
			mutes.flip(0);
			break;
		case 'S':
			mutes.flip(1);
			break;
		case 'D':
			mutes.flip(2);
			break;
		case 'F':
			mutes.flip(3);
			break;
		case 'G':
			mutes.flip(4);
			break;
		case 'H':
			mutes.flip(5);
			break;
		case 'J':
			mutes.flip(6);
			break;
		case 'K':
			mutes.flip(7);
			break;
		case ' ':
			sustain = true;
			break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
	switch (key) {
		case ' ':
			sustain = false;
			break;
	}
}

ofPoint ofApp::getDot(int x, int y) {
	float cf = ofMap(x, (w - dim) / 2, (w + dim) / 2, -0.5, X-0.5);
	float rf = ofMap(y, (h - dim) / 2, (h + dim) / 2, -0.5, Y-0.5);
	return ofPoint(cf, rf);
} 

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {	
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	ofPoint dot = getDot(x, y);
	notes[(int)round(dot.x)][(int)round(dot.y)].flip(currentInstrument);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
	this->w = w;
	this->h = h;
	this->dim = min(w, h);
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}

//--------------------------------------------------------------
void ofApp::exit() {

	// Send noteOffs for every note
	for (int y = 0; y < Y; y++)
		for (int i = 0; i < N_INSTRUMENTS; i++)
			midiOut.sendNoteOff(i+1, getNote(y), 0);

	// clean up
	midiOut.closePort();
}
