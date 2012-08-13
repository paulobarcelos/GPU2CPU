#pragma once

#include "ofMain.h"

//from http://www.songho.ca/opengl/gl_pbo.html

class ofxPBOTexture : public ofTexture{

    public:

        ofxPBOTexture(){
            pboID = NULL;
        }

        ~ofxPBOTexture(){
            ofxPBOTexture::clear();
        }

        //----------------------------------------------------------
        void clear(){
            ofTexture::clear();
            if( pboID != NULL){
                glDeleteBuffersARB(1, pboID);
                delete [] pboID;
                pboID = NULL;
            }
        }

        //----------------------------------------------------------
        void allocate(int w, int h, int internalGlDataType){
            ofxPBOTexture::allocate(w, h, internalGlDataType, ofGetUsingArbTex());
        }

        //----------------------------------------------------------
        void allocate(int w, int h, int internalGlDataType, bool bUseARBExtention){
            ofxPBOTexture::clear();

            if( pboID == NULL ){
                pboID = new GLuint[1];
            }

            ofTexture::allocate(w, h, internalGlDataType, bUseARBExtention);

            GLuint dataSize    = 0;

            if( internalGlDataType == GL_RGBA || internalGlDataType == GL_BGRA ){
                dataSize = w * h * 4;
            }if( internalGlDataType == GL_RGB || internalGlDataType == GL_BGR ){
                dataSize = w * h * 3;
            }else if(internalGlDataType == GL_LUMINANCE){
                dataSize = w * h;
            }

            bool pboSupported = true;

            if(pboSupported){
                // create 2 pixel buffer objects, you need to delete them when program exits.
                // glBufferDataARB with NULL pointer reserves only memory space.
                glGenBuffersARB(1, pboID);
                glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, pboID[0]);
                glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, dataSize, 0, GL_STREAM_DRAW_ARB);
                glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
            }

        }


        //----------------------------------------------------------
        void loadData(unsigned char * data, int w, int h, int glDataType){

            if ( w > texData.tex_w || h > texData.tex_h) {
                ofLog(OF_LOG_ERROR,"image data too big for allocated texture. not uploading...");
                return;
            }

            //update our size with the new dimensions - this should be the same size or smaller than the allocated texture size
            texData.width 	= w;
            texData.height 	= h;
            texData.glType  = glDataType;

            GLuint dataSize    = 0;

            if( glDataType == GL_RGBA || glDataType == GL_BGRA ){
                dataSize = w * h * 4;
            }if( glDataType == GL_RGB || glDataType == GL_BGR ){
                dataSize = w * h * 3;
            }else if(glDataType == GL_LUMINANCE){
                dataSize = w * h;
            }

            //compute new tex co-ords based on the ratio of data's w, h to texture w,h;
            #ifndef TARGET_OF_IPHONE
                if (texData.textureTarget == GL_TEXTURE_RECTANGLE_ARB){
                    texData.tex_t = w;
                    texData.tex_u = h;
                } else
            #endif
            {
                texData.tex_t = (float)(w) / (float)texData.tex_w;
                texData.tex_u = (float)(h) / (float)texData.tex_h;
            }

            // 	ok this is an ultra annoying bug :
            // 	opengl texels and linear filtering -
            // 	when we have a sub-image, and we scale it
            // 	we can clamp the border pixels to the border,
            //  but the borders of the sub image get mixed with
            //  neighboring pixels...
            //  grr...
            //
            //  the best solution would be to pad out the image
            // 	being uploaded with 2 pixels on all sides, and
            //  recompute tex_t coordinates..
            //  another option is a gl_arb non pow 2 textures...
            //  the current hack is to alter the tex_t, tex_u calcs, but
            //  that makes the image slightly off...
            //  this is currently being done in draw...
            //
            // 	we need a good solution for this..
            //
            //  http://www.opengl.org/discussion_boards/ubb/ultimatebb.php?ubb=get_topic;f=3;t=014770#000001
            //  http://www.opengl.org/discussion_boards/ubb/ultimatebb.php?ubb=get_topic;f=3;t=014770#000001

            //------------------------ likely, we are uploading continuous data

            GLint prevAlignment;
            glGetIntegerv(GL_UNPACK_ALIGNMENT, &prevAlignment);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);


            // update the texture image:
            glEnable(texData.textureTarget);

            // bind the texture and PBO
            glBindTexture(texData.textureTarget, (GLuint)texData.textureName[0]);

             // start to modify pixel values ///////////////////

            // bind PBO to update pixel values
            glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, pboID[0]);
            glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, dataSize, 0, GL_STREAM_DRAW_ARB);

            GLubyte* ptr = (GLubyte*)glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY_ARB);
            if(ptr){

                //this is the advantage of PBO - we can update a percentage of the total data
                //we don't have to do the whole thing.
                if( dataSize > 0 ){
                    memcpy(ptr, data, dataSize * sizeof(char));
                }

                glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB); // release pointer to mapping buffer
            }

            // copy pixels from PBO to texture object
            // Use offset instead of pointer.
            glTexSubImage2D(texData.textureTarget, 0, 0, 0, w, h, texData.glType, GL_UNSIGNED_BYTE, 0);

            // it is good idea to release PBOs with ID 0 after use.
            // Once bound with 0, all pixel operations behave normal ways.
            glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

            glDisable(texData.textureTarget);

            //------------------------ back to normal.
            glPixelStorei(GL_UNPACK_ALIGNMENT, prevAlignment);


            #ifdef TARGET_OF_IPHONE
                texData.bFlipTexture = true; // textures need to be flipped for the iphone
            #else
                texData.bFlipTexture = false;
            #endif

        }


    GLuint * pboID;

};
