#pragma once

#include <GL/glew.h>

namespace CSCI441 {

	class ShaderUtils {
	public:
		static void printLog( GLuint handle );
		static void printShaderInfo( GLuint handle );
		static void readTextFromFile( const char *filename, char* &output );
		static GLuint compileShader( const char *filename, GLenum shaderType );
		static const char* GL_type_to_string(GLenum type);
		static const char* GL_shader_type_to_string(GLint shaderType);
	private:
		ShaderUtils();
	};

}
