#pragma once

#include <GL/glew.h>

namespace CSCI441 {

	class OpenGLUtils {
	public:
		static void printOpenGLInfo();

		static GLuint registerOpenGLTexture2D( const char *filename,
											   GLenum minFilter = GL_LINEAR,
											   GLenum magFilter = GL_LINEAR,
											   GLenum wrapS = GL_REPEAT,
											   GLenum wrapT = GL_REPEAT,
											   GLenum envMode = GL_MODULATE );

		static bool loadBMP(const char* filename, int &texWidth, int &texHeight, unsigned char* &texData, int &texChannels );
		static bool loadPPM(const char* filename, int &texWidth, int &texHeight, unsigned char* &texData, int &texChannels );
		static bool loadTGA(const char *filename, int &texWidth, int &texHeight, unsigned char* &texData, int &texChannels );
	private:
		OpenGLUtils();
	};

}
