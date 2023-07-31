#pragma once

#include "ofMain.h"
#include "ofxJSONElement.h"
#include "ofVectorMath.h"

class ofApp : public ofBaseApp {

public:
	uint64_t time;
	void setup();
	void update();
	void draw();

	void checkInput();
	void phase();
	bool isVisited();

	void keyPressed(int key);

	ofxJSONElement json;//stores data from API
	string APIKey = "&key=bf3055cabf5340438f9123314221201";
	int wait; //0: waiting for user input 1: cooldown 3: cutscene/etc(controlled elsehwere)
	string weather, city, msgBox, userInp;
	string dayDis = "Currently in\n\n\n\n\n     Day 1";
	string prompt = "";
	int diagState = 0;
	int water = 50;
	int sun = 50;
	int day = 1;
	vector<string> visited;
	bool userType;

	ofImage bg, plant, fade;
	ofTrueTypeFont headFont, msgFont;

};