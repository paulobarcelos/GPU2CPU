#pragma once

#include "ofMain.h"
#include "ofxTimeMeasurements.h"

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void process(ofFbo & fbo, ofImage & image, string name);
	
		
		
	ofVideoGrabber cam;
	ofShader shader;
	ofFbo fbo128,fbo256,fbo512,fbo1024,fbo2048,fbo4096;
	ofImage image128,image256,image512,image1024,image2048,image4096;

};
