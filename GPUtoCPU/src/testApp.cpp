#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofSetBackgroundAuto(false);
	
	cam.initGrabber(640, 480);
	
	fbo128.allocate(640, 480, GL_RGBA);
	image128.allocate(640, 480, OF_IMAGE_COLOR_ALPHA);
	
	fbo256.allocate(256, 256, GL_RGBA);
	image256.allocate(256, 256, OF_IMAGE_COLOR_ALPHA);
	
	fbo512.allocate(512, 512, GL_RGBA);
	image512.allocate(512, 512, OF_IMAGE_COLOR_ALPHA);
	
	fbo1024.allocate(1024, 1024, GL_RGBA);
	image1024.allocate(1024, 1024, OF_IMAGE_COLOR_ALPHA);
	
	fbo2048.allocate(2048, 2048, GL_RGBA);
	image2048.allocate(2048, 2048, OF_IMAGE_COLOR_ALPHA);
	
	fbo4096.allocate(4096, 4096, GL_RGBA);
	image4096.allocate(4096, 4096, OF_IMAGE_COLOR_ALPHA);
	
	shader.setupShaderFromSource(GL_FRAGMENT_SHADER, "uniform sampler2DRect tex; uniform vec2 texSize; uniform vec2 size; void main(void){ vec2 st = gl_TexCoord[0].st / size; st *= texSize; gl_FragColor = texture2DRect(tex, st); }");
	shader.linkProgram();
}

//--------------------------------------------------------------

void testApp::update(){
	

}

//--------------------------------------------------------------
void testApp::draw(){
	
	
	cam.update();
	
	if(cam.isFrameNew()){
		ofClear(255);
		ofSetColor(255);
				
		process(fbo128, image128, "aa 128");
		process(fbo256, image256, "ab 256");
		process(fbo512, image512, "ac 512");
		process(fbo1024, image1024, "ad 1024");
		process(fbo2048, image2048, "ae 2048");
		process(fbo4096, image4096, "af 4096");

		ofSetColor(0);
		TIME_SAMPLE_DRAW(20, 20);
	}
}

void testApp::process(ofFbo & fbo, ofImage & image, string name){
	fbo.begin();
	ofClear(0);
	shader.begin();
	shader.setUniformTexture("tex", cam.getTextureReference(), 0 );
	shader.setUniform2f("texSize", cam.getWidth(), cam.getHeight());
	shader.setUniform2f("size", fbo.getWidth(), fbo.getHeight());
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(0, 0, 0);
	glTexCoord2f(fbo.getWidth(), 0);
	glVertex3f(fbo.getWidth(), 0, 0);
	glTexCoord2f(fbo.getWidth(), fbo.getHeight());
	glVertex3f(fbo.getWidth(), fbo.getHeight(), 0);
	glTexCoord2f(0,fbo.getHeight()); 
	glVertex3f(0,fbo.getHeight(), 0);
	glEnd();
	shader.end();
	
	fbo.end();
	
	TIME_SAMPLE_START(name);
	fbo.readToPixels(image.getPixelsRef());
	//image.update();
	TIME_SAMPLE_STOP(name);
	
	//image.draw(0, 0);
}