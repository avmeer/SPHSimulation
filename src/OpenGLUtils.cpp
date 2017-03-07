#include "../include/OpenGLUtils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void CSCI441::OpenGLUtils::printOpenGLInfo() {
	GLint maxVertexAttribs = 0;
	glGetIntegerv( GL_MAX_VERTEX_ATTRIBS, &maxVertexAttribs );

	GLint maxVertexUniformBlocks = 0;
	glGetIntegerv( GL_MAX_VERTEX_UNIFORM_BLOCKS, &maxVertexUniformBlocks );

	GLint maxGeometryUniformBlocks = 0;
	glGetIntegerv( GL_MAX_GEOMETRY_UNIFORM_BLOCKS, &maxGeometryUniformBlocks );

	GLint maxFragmentUniformBlocks = 0;
	glGetIntegerv( GL_MAX_FRAGMENT_UNIFORM_BLOCKS, &maxFragmentUniformBlocks );

	GLint maxUniformBlockSize = 0;
	glGetIntegerv( GL_MAX_UNIFORM_BLOCK_SIZE, &maxUniformBlockSize );

	GLint maxLights = 0;
	glGetIntegerv( GL_MAX_LIGHTS, &maxLights );

	GLint maxTextures = 0;
	glGetIntegerv( GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextures );

	fprintf( stdout, "\n[INFO]: /--------------------------------------------------------\\\n");
	fprintf( stdout, "[INFO]: | OpenGL Information                                     |\n");
	fprintf( stdout, "[INFO]: |--------------------------------------------------------|\n");
	fprintf( stdout, "[INFO]: |   OpenGL Version:  %35s |\n", glGetString(GL_VERSION) );
	fprintf( stdout, "[INFO]: |   OpenGL Renderer: %35s |\n", glGetString(GL_RENDERER) );
	fprintf( stdout, "[INFO]: |   OpenGL Vendor:   %35s |\n", glGetString(GL_VENDOR) );
	fprintf( stdout, "[INFO]: |   Shading Version: %35s |\n", glGetString(GL_SHADING_LANGUAGE_VERSION) );
	fprintf( stdout, "[INFO]: >--------------------------------------------------------<\n");
	fprintf( stdout, "[INFO]: |   Max # Vertex Attributes:  %26d |\n", maxVertexAttribs );
	fprintf( stdout, "[INFO]: |   Max # Vertex Uniform Blocks:  %22d |\n", maxVertexUniformBlocks );
	fprintf( stdout, "[INFO]: |   Max # Geometry Uniform Blocks:  %20d |\n", maxGeometryUniformBlocks );
	fprintf( stdout, "[INFO]: |   Max # Fragment Uniform Blocks:  %20d |\n", maxFragmentUniformBlocks );
	fprintf( stdout, "[INFO]: |   Max Uniform Blocks Size:  %20d bytes |\n", maxUniformBlockSize );
	fprintf( stdout, "[INFO]: |   Max # Lights:    %35d |\n", maxLights );
	fprintf( stdout, "[INFO]: |   Max # Textures:  %35d |\n", maxTextures );
	fprintf( stdout, "[INFO]: \\--------------------------------------------------------/\n\n");
}

GLuint CSCI441::OpenGLUtils::registerOpenGLTexture2D( const char *filename, GLenum minFilter, GLenum magFilter, GLenum wrapS, GLenum wrapT, GLenum envMode ) {
	glEnable( GL_TEXTURE_2D );
	GLuint textureHandle;
	glGenTextures(1, &textureHandle);

	int texWidth, texHeight, texChannels;
	unsigned char *texData;
	if( strstr( filename, ".bmp" ) ) {
		CSCI441::OpenGLUtils::loadBMP( filename, texWidth, texHeight, texData, texChannels );
	} else if( strstr( filename, ".ppm" ) ) {
		CSCI441::OpenGLUtils::loadPPM( filename, texWidth, texHeight, texData, texChannels );
	} else if( strstr( filename, ".tga" ) ) {
		CSCI441::OpenGLUtils::loadTGA( filename, texWidth, texHeight, texData, texChannels );
	} else {
		return 0;
	}

	glBindTexture(GL_TEXTURE_2D, textureHandle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, envMode );

	//actually transfer the texture to the GPU!
	glTexImage2D(GL_TEXTURE_2D,                 //still working with 2D textures, obv.
					0,                          //not using mipmapping, so this is the highest level.
					texChannels == 4 ? GL_RGBA : GL_RGB,  //we're going to provide OpenGL with RGB and maybe A components...
					texWidth,                   //...of this width...
					texHeight,                  //...and this height.
					0,                          //give it a border of 0 pixels (none)
					texChannels == 4 ? GL_RGBA : GL_RGB,  //and store it, internally, as RGB/A (OpenGL will convert to floats between 0.0 and 1.0f)
					GL_UNSIGNED_BYTE,           //this is the format our data is in, and finally,
					texData);                 //there's the data!

	return textureHandle;
}

bool CSCI441::OpenGLUtils::loadBMP(const char* filename, int &texWidth, int &texHeight, unsigned char* &texData, int &texChannels ) {

	FILE *file;
	unsigned long size;                 // size of the image in bytes.
	size_t i;							// standard counter.
	unsigned short int planes;          // number of planes in image (must be 1)
	unsigned short int bpp;             // number of bits per pixel (must be 24)
	char temp;                          // used to convert bgr to rgb color.

	// make sure the file is there.
	if ((file = fopen(filename, "rb"))==NULL) {
		printf("[ERROR]: File Not Found: %s\n",filename);
		return false;
	}

	// seek through the bmp header, up to the width/height:
	fseek(file, 18, SEEK_CUR);

	// read the width
	if ((i = fread(&texWidth, 4, 1, file)) != 1) {
		printf("[ERROR]: reading width from %s.\n", filename);
		return false;
	}
	//printf("Width of %s: %lu\n", filename, image->sizeX);

	// read the height
	if ((i = fread(&texHeight, 4, 1, file)) != 1) {
		printf("[ERROR]: reading height from %s.\n", filename);
		return false;
	}
	//printf("Height of %s: %lu\n", filename, image->sizeY);

	// calculate the size (assuming 24 bits or 3 bytes per pixel).
	size = texWidth * texHeight * 3;

	// read the planes
	if ((fread(&planes, 2, 1, file)) != 1) {
		printf("[ERROR]: reading planes from %s.\n", filename);
		return false;
	}
	if (planes != 1) {
		printf("[ERROR]: Planes from %s is not 1: %u\n", filename, planes);
		return false;
	}

	// read the bpp
	if ((i = fread(&bpp, 2, 1, file)) != 1) {
		printf("[ERROR]: reading bpp from %s.\n", filename);
		return false;
	}
	if (bpp != 24) {
		printf("[ERROR]: Bpp from %s is not 24: %u\n", filename, bpp);
		return false;
	}

	// seek past the rest of the bitmap header.
	fseek(file, 24, SEEK_CUR);

	// read the data.
	texData = (unsigned char *) malloc(size);
	if (texData == NULL) {
		printf("[ERROR]: allocating memory for color-corrected image data");
		return false;
	}

	if ((i = fread(texData, size, 1, file)) != 1) {
		printf("[ERROR]: reading image data from %s.\n", filename);
		return false;
	}

	for (i=0;i<size;i+=3) { // reverse all of the colors. (bgr -> rgb)
		temp = texData[i];
		texData[i] = texData[i+2];
		texData[i+2] = temp;
	}

	texChannels = 3;
	return true;
}

bool CSCI441::OpenGLUtils::loadPPM(const char* filename, int &texWidth, int &texHeight, unsigned char* &texData, int &texChannels ) {
	FILE *fp;

	// make sure the file is there.
	if ((fp = fopen(filename, "rb"))==NULL) {
		printf("[ERROR]: File Not Found: %s\n",filename);
		return false;
	}

	int temp, maxValue;
	fscanf(fp, "P%d", &temp);
	if(temp != 3) {
		fprintf(stderr, "[ERROR]: PPM file is not of correct format! (Must be P3, is P%d.)\n", temp);
		fclose(fp);
		return false;
	}

	//got the file header right...
	fscanf(fp, "%d", &texWidth);
	fscanf(fp, "%d", &texHeight);
	fscanf(fp, "%d", &maxValue);

	//now that we know how big it is, allocate the buffer...
	texData = new unsigned char[texWidth*texHeight*3];
	if(!texData) {
		fprintf(stderr, "[ERROR]: couldn't allocate image memory. Dimensions: %d x %d.\n", texWidth, texHeight);
		fclose(fp);
		return false;
	}

	//and read the data in.
	for(int j = 0; j < texHeight; j++) {
		for(int i = 0; i < texWidth; i++) {
			//read the data into integers (4 bytes) before casting them to unsigned characters
			//and storing them in the unsigned char array.
			int r, g, b;
			fscanf(fp, "%d", &r);
			fscanf(fp, "%d", &g);
			fscanf(fp, "%d", &b);

			texData[(j*texWidth+i)*3+0] = (unsigned char)r;
			texData[(j*texWidth+i)*3+1] = (unsigned char)g;
			texData[(j*texWidth+i)*3+2] = (unsigned char)b;
		}
	}

	fclose(fp);

	texChannels = 3;
	return true;

}

bool CSCI441::OpenGLUtils::loadTGA(const char *filename, int &texWidth, int &texHeight, unsigned char* &texData, int &texChannels ) {
	FILE *fp = fopen(filename, "rb");
	if(!fp) {
		fprintf(stderr, "Error: could not open TGA file: %s.", filename);
		texData = NULL;
		return false;
	}

	//bunch of data fields in the file header that we read in en masse
	unsigned char idLength, colorMapType, imageType;
	unsigned short idxOfFirstColorMapEntry, countOfColorMapEntries;
	unsigned char numBitsPerColorMapEntry;
	unsigned short xCoordOfLowerLeft, yCoordOfLowerLeft;
	unsigned short width, height;
	unsigned char bytesPerPixel;
	unsigned char imageAttributeFlags;

	fread(&idLength, sizeof(unsigned char), 1, fp);
	fread(&colorMapType, sizeof(unsigned char), 1, fp);
	fread(&imageType, sizeof(unsigned char), 1, fp);
	fread(&idxOfFirstColorMapEntry, sizeof(unsigned short), 1, fp);
	fread(&countOfColorMapEntries, sizeof(unsigned short), 1, fp);
	fread(&numBitsPerColorMapEntry, sizeof(unsigned char), 1, fp);
	fread(&xCoordOfLowerLeft, sizeof(unsigned short), 1, fp);
	fread(&yCoordOfLowerLeft, sizeof(unsigned short), 1, fp);
	fread(&width, sizeof(unsigned short), 1, fp);
	fread(&height, sizeof(unsigned short), 1, fp);
	fread(&bytesPerPixel, sizeof(unsigned char), 1, fp);
	fread(&imageAttributeFlags, sizeof(unsigned char), 1, fp);

	//now check to make sure that we actually have the capability to read this file.
	if(colorMapType != 0) {
		fprintf(stderr, "Error: TGA file (%s) uses colormap instead of RGB/RGBA data; this is unsupported.\n", filename);
		texData = NULL;
		return false;
	}

	if(imageType != 2 && imageType != 10) {
		fprintf(stderr, "Error: unspecified TGA type: %d. Only supports 2 (uncompressed RGB/A) and 10 (RLE, RGB/A).\n", imageType);
		texData = NULL;
		return false;
	}

	if(bytesPerPixel != 24 && bytesPerPixel != 32) {
		fprintf(stderr, "Error: unsupported image depth (%d bits per pixel). Only supports 24bpp and 32bpp.\n", bytesPerPixel);
		texData = NULL;
		return false;
	}


	//set some helpful variables based on the header information:
	bool usingRLE = (imageType == 10);              //whether the file uses run-length encoding (compression)
	texChannels = (bytesPerPixel == 32 ? 4 : 3);                //whether the file is RGB or RGBA (RGBA = 32bpp)
	bool topLeft = (imageAttributeFlags & 32);      //whether the origin is at the top-left or bottom-left


	//this should never happen, since we should never have a color map,
	//but just in case the data is setting around in there anyway... skip it.
	if(idLength != 0) {
		fseek(fp, idLength, SEEK_CUR);
	}


	//allocate our image data before we get started.
	texWidth = width;
	texHeight = height;
	texData = new unsigned char[texWidth*texHeight*texChannels];

	//ok so we can assume at this point that there's no colormap, and
	//consequently that the next part of the image is the actual image data.
	if(usingRLE) {
		//ok... the data comes in in packets, but we don't know how many of these there'll be.
		int numberOfPixelsRead = 0;
		unsigned char dummy;
		while(numberOfPixelsRead < texWidth*texHeight) {
			//alright let's read the packet header.
			fread(&dummy, sizeof(unsigned char), 1, fp);
			bool isRLEPacket = (dummy & 0x80);

			unsigned char theOtherBitsYesThatWasAPun = (dummy & 0x7F);
			if(isRLEPacket) {
				//the other bits (+1) gives the number of times we need to
				//repeat the next real set of color values (RGB/A).
				unsigned char repeatedR, repeatedG, repeatedB, repeatedA;
				fread(&repeatedR, sizeof(unsigned char), 1, fp);
				fread(&repeatedG, sizeof(unsigned char), 1, fp);
				fread(&repeatedB, sizeof(unsigned char), 1, fp);
				if(texChannels==4) fread(&repeatedA, sizeof(unsigned char), 1, fp);

				//and go ahead and copy those into the new image. repetitively.
				for(int i = 0; i < ((int)theOtherBitsYesThatWasAPun+1); i++) {
					int idx = numberOfPixelsRead * texChannels;
					texData[idx+2] = repeatedR;
					texData[idx+1] = repeatedG;
					texData[idx+0] = repeatedB;
					if(texChannels==4) texData[idx+3] = repeatedA;

					numberOfPixelsRead++;
				}
			} else {
				//the other bits (+1) gives the number of consecutive
				//pixels we get to read in from the stream willy nilly.
				for(int i = 0; i < ((int)theOtherBitsYesThatWasAPun+1); i++) {
					int idx = numberOfPixelsRead * texChannels;
					fread(&texData[idx+2], sizeof(unsigned char), 1, fp);
					fread(&texData[idx+1], sizeof(unsigned char), 1, fp);
					fread(&texData[idx+0], sizeof(unsigned char), 1, fp);
					if(texChannels==4) fread(&texData[idx+3], sizeof(unsigned char), 1, fp);

					numberOfPixelsRead++;
				}
			}
		}


		//and you know what? we're not going to have worried about flipping the image before
		//if its origin was in the bottom left or top left or whatever. flip it afterwards here if need be.
		if(!topLeft) {
			unsigned char *tempCopy = new unsigned char[texWidth*texHeight*texChannels];
			for(int i = 0; i < texHeight; i++) {
				for(int j = 0; j < texWidth; j++) {
					int copyIdx = (i*texWidth + j)*texChannels;
					int pullIdx = ((texHeight - i - 1)*texWidth + j)*texChannels;
					tempCopy[copyIdx+0] = texData[pullIdx+0];
					tempCopy[copyIdx+1] = texData[pullIdx+1];
					tempCopy[copyIdx+2] = texData[pullIdx+2];
					if(texChannels==4) tempCopy[copyIdx+3] = texData[pullIdx+3];
				}
			}

			delete texData;
			texData = tempCopy;
		}

	} else {
		//uh well if we're not using run-length encoding, i guess we'll
		//just try reading bytes in straight like a normal binary file.
		unsigned char byte1, byte2, byte3, maybeEvenByte4;
		for(int i = 0; i < texHeight; i++) {
			for(int j = 0; j < texWidth; j++) {
				int multiplierThing = texChannels;

				//read in the data from file...
				fread(&byte1, sizeof(unsigned char), 1, fp);
				fread(&byte2, sizeof(unsigned char), 1, fp);
				fread(&byte3, sizeof(unsigned char), 1, fp);
				if(texChannels==4) fread(&maybeEvenByte4, sizeof(unsigned char), 1, fp);

				//flip the vertical index if the origin is in the bottom-left.
				int wutHeight = topLeft ? i : (texHeight - 1 - i);
				int idx = (wutHeight*texWidth+j)*multiplierThing;

				//and load that image into file. seems to be BGR instead of RGB...
				texData[idx+2] = byte1;
				texData[idx+1] = byte2;
				texData[idx+0] = byte3;
				if(texChannels==4) texData[idx+3] = maybeEvenByte4;
			}
		}
	}

	fclose(fp);
	return true;
}

CSCI441::OpenGLUtils::OpenGLUtils() {}
