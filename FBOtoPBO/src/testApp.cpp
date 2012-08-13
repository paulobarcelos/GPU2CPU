#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofSetVerticalSync(true);
	
	cam.initGrabber(640, 480);
	fbo.allocate(WIDTH, HEIGHT, GL_RGBA);
	image.allocate(WIDTH, HEIGHT, OF_IMAGE_COLOR_ALPHA);
	
	glGenBuffersARB(2, pboIds);
	glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, pboIds[0]);
	glBufferDataARB(GL_PIXEL_PACK_BUFFER_ARB, WIDTH * HEIGHT * 4, NULL, GL_STREAM_READ_ARB);
	glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, pboIds[1]);
	glBufferDataARB(GL_PIXEL_PACK_BUFFER_ARB, WIDTH * HEIGHT * 4, NULL, GL_STREAM_READ_ARB);	
	glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, 0);
	
	index = 0;
	nextIndex = 0;
	
	TIME_SAMPLE_SET_FRAMERATE(60);
}

//--------------------------------------------------------------
void testApp::update(){
	cam.update();
	
	fbo.begin();
	ofClear(255, 0, 0);
	ofSetColor(255);
	cam.draw(0, 0);	
	ofRect(0, 0, 100, 100);
	fbo.end();
	
	ofClear(255, 0, 0);
	ofSetColor(255);
	fbo.draw(0, ofGetHeight()-HEIGHT);
	
	// increment current index first then get the next index
    // "index" is used to read pixels from a framebuffer to a PBO
    // "nextIndex" is used to process pixels in the other PBO
    index = (index + 1) % 2;
    nextIndex = (index + 1) % 2;
	
	//ofSleepMillis(50);
	
		
	// copy pixels from framebuffer to PBO
	// Use offset instead of ponter.
	// OpenGL should perform asynch DMA transfer, so glReadPixels() will return immediately.
	glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, pboIds[index]);
	
	// Bind FBO
	//fbo.bind();
	TIME_SAMPLE_START("FBO to PBO");
	glReadPixels(0, 0, WIDTH, HEIGHT, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
	TIME_SAMPLE_STOP("FBO to PBO");		
	// Unbind FBO
	//fbo.unbind();
	
	ofSleepMillis(15);
	
	
	// map the PBO that contain framebuffer pixels before processing it
	glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, pboIds[nextIndex]);
	TIME_SAMPLE_START("PBO to CPU");
	GLubyte* src = (GLubyte*)glMapBufferARB(GL_PIXEL_PACK_BUFFER_ARB, GL_READ_ONLY_ARB);
	TIME_SAMPLE_STOP("PBO to CPU");
	if(src){
		TIME_SAMPLE_START("MEMCPY");
		// copy the data to the image
		memcpy(image.getPixels(), src, WIDTH * HEIGHT * 4 * sizeof(unsigned char));
		TIME_SAMPLE_STOP("MEMCPY");
		
		TIME_SAMPLE_START("IMAGE UPDATE");
		image.update();
		TIME_SAMPLE_STOP("IMAGE UPDATE");
		
		// release pointer to the mapped buffer
		glUnmapBufferARB(GL_PIXEL_PACK_BUFFER_ARB);
	}
	// unbind PBO
	glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, 0);
	
}

//--------------------------------------------------------------
void testApp::draw(){
		
	image.draw(0, 0);
	
	TIME_SAMPLE_DRAW(200, 20);
	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}