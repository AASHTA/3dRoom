/*
 * texture.h
 *
 *  Created on: Mar 8, 2015
 *      Author: Ioana
 */

#ifndef TEXTURE_H_
#define TEXTURE_H_

class Texture {
private:
	// Data read from the header of the BMP file
	unsigned char header[54]; // Each BMP file begins by a 54-bytes header
	unsigned int dataPos;     // Position in the file where the actual data begins
	unsigned int width, height;
	unsigned int imageSize;   // = width*height*3
	// Actual RGB data
	unsigned char * data;
	GLuint textureId;

public:
	void loadTexture(char *imagepath);
	void setTextureId(GLuint id);
	GLuint getTextureId();
};


#endif /* TEXTURE_H_ */
