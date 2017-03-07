#include "../include/ShaderUtils.h"

#include <stdio.h>

#include <fstream>
#include <string>
#include <cstring>
#include <string.h>

// readTextFromFile() //////////////////////////////////////////////////////////////
//
//  Reads in a text file as a single string. Used to aid in shader loading.
//
////////////////////////////////////////////////////////////////////////////////
void CSCI441::ShaderUtils::readTextFromFile(const char *filename, char* &output){
    std::string buf = std::string("");
    std::string line;

    std::ifstream in(filename);
	if(in.is_open()){
		while( std::getline(in, line) ) {
			buf += line + "\n";
		}
	}
	else{
		printf("\n[ERROR]: | Error Opening shader file %s\n",filename);
	}
    output = new char[buf.length()+1];
    strncpy(output, buf.c_str(), buf.length());
    output[buf.length()] = '\0';

    in.close();
}

const char* CSCI441::ShaderUtils::GL_type_to_string(GLenum type) {
  switch(type) {
    case GL_BOOL: return "bool";
    case GL_INT: return "int";
    case GL_FLOAT: return "float";
    case GL_FLOAT_VEC2: return "vec2";
    case GL_FLOAT_VEC3: return "vec3";
    case GL_FLOAT_VEC4: return "vec4";
    case GL_FLOAT_MAT2: return "mat2";
    case GL_FLOAT_MAT3: return "mat3";
    case GL_FLOAT_MAT4: return "mat4";
    case GL_SAMPLER_2D: return "sampler2D";
    case GL_SAMPLER_3D: return "sampler3D";
    case GL_SAMPLER_CUBE: return "samplerCube";
    case GL_SAMPLER_2D_SHADOW: return "sampler2DShadow";
    default: break;
  }
  return "other";
}

const char* CSCI441::ShaderUtils::GL_shader_type_to_string(GLint type) {
  switch(type) {
    case GL_VERTEX_SHADER: return "Vertex Shader";
    case GL_GEOMETRY_SHADER: return "Geometry Shader";
    case GL_FRAGMENT_SHADER: return "Fragment Shader";
    default: break;
  }
  return "other";
}

// printLog() //////////////////////////////////////////////////////////////////
//
//  Check for errors from compiling or linking a vertex/fragment/shader program
//      Prints to terminal
//
////////////////////////////////////////////////////////////////////////////////
void CSCI441::ShaderUtils::printLog( GLuint handle ) {
	int status;
    int infologLength = 0;
    int maxLength;
    bool isShader;

    /* check if the handle is to a vertex/fragment shader */
    if( glIsShader( handle ) ) {
        glGetShaderiv(  handle, GL_INFO_LOG_LENGTH, &maxLength );

        isShader = true;
    }
    /* check if the handle is to a shader program */
    else {
        glGetProgramiv( handle, GL_INFO_LOG_LENGTH, &maxLength );

        isShader = false;
    }

    /* create a buffer of designated length */
    char infoLog[maxLength];

    if( isShader ) {
    	glGetShaderiv( handle, GL_COMPILE_STATUS, &status );
    	printf( "[INFO]: |   Shader  Handle %2d: Compile%-26s |\n", handle, (status == 1 ? "d Successfully" : "r Error") );

        /* get the info log for the vertex/fragment shader */
        glGetShaderInfoLog(  handle, maxLength, &infologLength, infoLog );

        if( infologLength > 0 ) {
			/* print info to terminal */
			printf( "[INFO]: |   %s Handle %d: %s\n", (isShader ? "Shader" : "Program"), handle, infoLog );
        }
    } else {
    	glGetProgramiv( handle, GL_LINK_STATUS, &status );
    	printf("[INFO]: |   Program Handle %2d: Linke%-28s |\n", handle, (status == 1 ? "d Successfully" : "r Error") );

        /* get the info log for the shader program */
        glGetProgramInfoLog( handle, maxLength, &infologLength, infoLog );

        if( infologLength > 0 ) {
			/* print info to terminal */
			printf( "[INFO]: |   %s Handle %d: %s\n", (isShader ? "Shader" : "Program"), handle, infoLog );
        }
    }
}

void printSubroutineInfo( GLuint handle, GLenum shaderStage ) {
	int params, params2;
	int *params3;
	
	glGetProgramStageiv(handle, shaderStage, GL_ACTIVE_SUBROUTINE_UNIFORMS, &params);
	printf("[INFO]: | GL_ACTIVE_SUBROUTINE_UNIFORMS (%-15s): %5i |\n", CSCI441::ShaderUtils::GL_shader_type_to_string(shaderStage), params);
	for(int i = 0; i < params; i++ ) {
		char name[64];
		int max_length = 64;
		int actual_length = 0;
		
		glGetActiveSubroutineUniformName( handle, shaderStage, i, max_length, &actual_length, name );
		glGetActiveSubroutineUniformiv( handle, shaderStage, i, GL_NUM_COMPATIBLE_SUBROUTINES, &params2 );
		glGetActiveSubroutineUniformiv( handle, shaderStage, i, GL_COMPATIBLE_SUBROUTINES, params3 );
		GLint loc = glGetSubroutineUniformLocation( handle, shaderStage, name );
		 
		printf("[INFO]: |   %i) name: %-15s #subRoutines: %-5i loc: %2i |\n", i, name, params2, loc );
		
		for(int j = 0; j < params2; j++ ) {
			GLint idx = params3[j];
		
			char name2[64];
			int max_length2 = 64;
			int actual_length2 = 0;
			glGetActiveSubroutineName( handle, shaderStage, idx, max_length2, &actual_length2, name2 );
				
			printf("[INFO]: |     %i) subroutine: %-25s index: %2i |\n", j, name2, idx );
		}
	}
}

void CSCI441::ShaderUtils::printShaderInfo( GLuint handle ) {
	int params;
	bool hasVertexShader = false;
	bool hasGeometryShader = false;
	bool hasFragmentShader = false;

	printf( "[INFO]: >--------------------------------------------------------<\n");

	GLuint shaders[6];
	int max_count = 6;
	int actual_count;
	glGetAttachedShaders( handle, max_count, &actual_count, shaders );
	printf("[INFO]: | GL_ATTACHED_SHADERS: %33i |\n", actual_count);
	for(int i = 0; i < actual_count; i++ ) {
		GLint shaderType;
		glGetShaderiv( shaders[i], GL_SHADER_TYPE, &shaderType );
		printf("[INFO]: |   %i) %-38s Handle: %2i |\n", i, GL_shader_type_to_string(shaderType), shaders[i]);
		
		if( shaderType == GL_VERTEX_SHADER ) hasVertexShader = true;
		else if( shaderType == GL_GEOMETRY_SHADER ) hasGeometryShader = true;
		else if( shaderType == GL_FRAGMENT_SHADER ) hasFragmentShader = true;
	}

	printf( "[INFO]: |--------------------------------------------------------|\n");

	glGetProgramiv(handle, GL_ACTIVE_ATTRIBUTES, &params);
	printf("[INFO]: | GL_ACTIVE_ATTRIBUTES: %32i |\n", params);
	for (int i = 0; i < params; i++) {
		char name[64];
		int max_length = 64;
		int actual_length = 0;
		int size = 0;
		GLenum type;
		glGetActiveAttrib (
				handle,
				i,
				max_length,
				&actual_length,
				&size,
				&type,
				name
		);
		if (size > 1) {
			for(int j = 0; j < size; j++) {
				char long_name[64];
				sprintf(long_name, "%s[%i]", name, j);
				int location = glGetAttribLocation(handle, long_name);
				printf("[INFO]: |   %i) type: %-15s name: %-13s loc: %2i |\n",
						i, GL_type_to_string(type), long_name, location);
			}
		} else {
			int location = glGetAttribLocation(handle, name);
			printf("[INFO]: |   %i) type: %-15s name: %-13s loc: %2i |\n",
					i, GL_type_to_string(type), name, location);
		}
	}

	printf( "[INFO]: |--------------------------------------------------------|\n");

	glGetProgramiv(handle, GL_ACTIVE_UNIFORMS, &params);
	printf("[INFO]: | GL_ACTIVE_UNIFORMS: %34i |\n", params);
	for(int i = 0; i < params; i++) {
		char name[64];
		int max_length = 64;
		int actual_length = 0;
		int size = 0;
		GLenum type;
		glGetActiveUniform( handle, i, max_length, &actual_length, &size, &type, name );
		if(size > 1) {
			for(int j = 0; j < size; j++) {
				char long_name[64];
				sprintf(long_name, "%s[%i]", name, j);
				int location = glGetUniformLocation(handle, long_name);
				printf("[INFO]: |   %i) type: %-15s name: %-13s loc: %2i |\n",
						i, GL_type_to_string(type), long_name, location);
			}
		} else {
			int location = glGetUniformLocation(handle, name);
			printf("[INFO]: |   %i) type: %-15s name: %-13s loc: %2i |\n",
					i, GL_type_to_string(type), name, location);
		}
	}

	printf( "[INFO]: \\--------------------------------------------------------/\n");
	
	if( hasVertexShader   ) printSubroutineInfo( handle, GL_VERTEX_SHADER );
	if( hasGeometryShader ) printSubroutineInfo( handle, GL_GEOMETRY_SHADER );
	if( hasFragmentShader ) printSubroutineInfo( handle, GL_FRAGMENT_SHADER );
	
	printf( "[INFO]: \\--------------------------------------------------------/\n\n");
}

// compileShader() ///////////////////////////////////////////////////////////////
//
//  Compile a given shader program
//
////////////////////////////////////////////////////////////////////////////////
GLuint CSCI441::ShaderUtils::compileShader( const char *filename, GLenum shaderType ) {
	GLuint shaderHandle = 0;
	char *shaderString;

    /* create a handle to our shader */
	shaderHandle = glCreateShader( shaderType );

    /* read in each text file and store the contents in a string */
    readTextFromFile( filename, shaderString );

    /* send the contents of each program to the GPU */
    glShaderSource( shaderHandle, 1, (const char**)&shaderString, NULL );

    /* we are good programmers so free up the memory used by each buffer */
    delete [] shaderString;

    /* compile each shader on the GPU */
    glCompileShader( shaderHandle );

    /* check the shader log */
    printLog( shaderHandle );

    /* return the handle of our shader */
    return shaderHandle;
}

CSCI441::ShaderUtils::ShaderUtils() {}
