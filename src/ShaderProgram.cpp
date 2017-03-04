#include "../include/ShaderProgram.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

CSCI441::ShaderProgram::ShaderProgram( const char *vertexShaderFilename, const char *fragmentShaderFilename ) {
	registerShaderProgram( vertexShaderFilename, "", "", "", fragmentShaderFilename );
}

CSCI441::ShaderProgram::ShaderProgram( const char *vertexShaderFilename, const char *tesselationControlShaderFilename, const char *tesselationEvaluationShaderFilename, const char *geometryShaderFilename, const char *fragmentShaderFilename ) {
	registerShaderProgram( vertexShaderFilename, tesselationControlShaderFilename, tesselationEvaluationShaderFilename, geometryShaderFilename, fragmentShaderFilename );
}

CSCI441::ShaderProgram::ShaderProgram( const char *vertexShaderFilename, const char *geometryShaderFilename, const char *fragmentShaderFilename ) {
	registerShaderProgram( vertexShaderFilename, "", "", geometryShaderFilename, fragmentShaderFilename );
}

bool CSCI441::ShaderProgram::registerShaderProgram( const char *vertexShaderFilename, const char *tesselationControlShaderFilename, const char *tesselationEvaluationShaderFilename, const char *geometryShaderFilename, const char *fragmentShaderFilename ) {
	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);

	printf( "\n[INFO]: /--------------------------------------------------------\\\n");

	/* compile each one of our shaders */
	printf( "[INFO]: | Vertex Shader: %39s |\n", vertexShaderFilename );
	_vertexShaderHandle   = CSCI441::ShaderUtils::compileShader( vertexShaderFilename,   GL_VERTEX_SHADER   );

	if( strcmp( tesselationControlShaderFilename, "" ) != 0 ) {
		printf( "[INFO]: | Tess Control Shader: %33s |\n", tesselationControlShaderFilename );
		if( major < 4 ) {
			printf( "[ERROR]:|   TESSELATION SHADER NOT SUPPORTED!!  UPGRADE TO v4.0+ |\n" );
			_tesselationControlShaderHandle = 0;
		} else {
			_tesselationControlShaderHandle = CSCI441::ShaderUtils::compileShader( tesselationControlShaderFilename, GL_TESS_CONTROL_SHADER );
		}
	} else {
		_tesselationControlShaderHandle = 0;
	}

	if( strcmp( tesselationEvaluationShaderFilename, "" ) != 0 ) {
		printf( "[INFO]: | Tess Evaluation Shader: %30s |\n", tesselationEvaluationShaderFilename );
		if( major < 4 ) {
			printf( "[ERROR]:|   TESSELATION SHADER NOT SUPPORTED!!  UPGRADE TO v4.0+ |\n" );
			_tesselationEvaluationShaderHandle = 0;
		} else {
			_tesselationEvaluationShaderHandle = CSCI441::ShaderUtils::compileShader( tesselationEvaluationShaderFilename, GL_TESS_EVALUATION_SHADER );
		}
	} else {
		_tesselationEvaluationShaderHandle = 0;
	}

	if( strcmp( geometryShaderFilename, "" ) != 0 ) {
		printf( "[INFO]: | Geometry Shader: %37s |\n", geometryShaderFilename );
		if( major < 3 ) {
			printf( "[ERROR]:|   GEOMETRY SHADER NOT SUPPORTED!!!    UPGRADE TO v3.0+ |\n" );
			_geometryShaderHandle = 0;
		} else {
			_geometryShaderHandle = CSCI441::ShaderUtils::compileShader( geometryShaderFilename, GL_GEOMETRY_SHADER );
		}
	} else {
		_geometryShaderHandle = 0;
	}

	printf( "[INFO]: | Fragment Shader: %37s |\n", fragmentShaderFilename );
	_fragmentShaderHandle = CSCI441::ShaderUtils::compileShader( fragmentShaderFilename, GL_FRAGMENT_SHADER );

	/* get a handle to a shader program */
	_shaderProgramHandle = glCreateProgram();

	/* attach the vertex and fragment shaders to the shader program */
	glAttachShader( _shaderProgramHandle, _vertexShaderHandle );
	if( _tesselationControlShaderHandle != 0 ) {
		glAttachShader( _shaderProgramHandle, _tesselationControlShaderHandle );
	}
	if( _tesselationEvaluationShaderHandle != 0 ) {
		glAttachShader( _shaderProgramHandle, _tesselationEvaluationShaderHandle );
	}
	if( _geometryShaderHandle != 0 ) {
		glAttachShader( _shaderProgramHandle, _geometryShaderHandle );
	}
	glAttachShader( _shaderProgramHandle, _fragmentShaderHandle );

	/* link all the programs together on the GPU */
	glLinkProgram( _shaderProgramHandle );

	printf( "[INFO]: | Shader Program: %41s", "|\n" );

	/* check the program log */
	CSCI441::ShaderUtils::printLog( _shaderProgramHandle );

	GLint separable = GL_FALSE;
	glGetProgramiv( _shaderProgramHandle, GL_PROGRAM_SEPARABLE, &separable );

	printf( "[INFO]: | Program Separable: %35s |\n", (separable ? "Yes" : "No"));

	/* print shader info for uniforms & attributes */
	CSCI441::ShaderUtils::printShaderInfo( _shaderProgramHandle );

	/* return handle */
	return _shaderProgramHandle != 0;
}

GLint CSCI441::ShaderProgram::getUniformLocation( const char *uniformName ) {
	return glGetUniformLocation( _shaderProgramHandle, uniformName );
}

GLint CSCI441::ShaderProgram::getUniformBlockIndex( const char *uniformBlockName ) {
	return glGetUniformBlockIndex( _shaderProgramHandle, uniformBlockName );
}

GLint CSCI441::ShaderProgram::getUniformBlockSize( const char *uniformBlockName ) {
	GLint blockSize;
	glGetActiveUniformBlockiv( _shaderProgramHandle, getUniformBlockIndex(uniformBlockName), GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize );
	return blockSize;
}

GLubyte* CSCI441::ShaderProgram::getUniformBlockBuffer( const char *uniformBlockName ) {
	GLubyte *blockBuffer;

	GLint blockSize = getUniformBlockSize( uniformBlockName );

	blockBuffer = (GLubyte*)malloc(blockSize);

	return blockBuffer;
}

GLint* CSCI441::ShaderProgram::getUniformBlockOffsets( const char *uniformBlockName ) {
	return getUniformBlockOffsets( getUniformBlockIndex(uniformBlockName) );
}

GLint* CSCI441::ShaderProgram::getUniformBlockOffsets( const char *uniformBlockName, const char *names[] ) {
	return getUniformBlockOffsets( getUniformBlockIndex(uniformBlockName), names );
}

GLint* CSCI441::ShaderProgram::getUniformBlockOffsets( GLint uniformBlockIndex ) {
	GLint numUniforms;
	glGetActiveUniformBlockiv( _shaderProgramHandle, uniformBlockIndex, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &numUniforms );

	GLuint *indices = (GLuint*)malloc(numUniforms*sizeof(GLuint));
	glGetActiveUniformBlockiv( _shaderProgramHandle, uniformBlockIndex, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, (GLint*)indices);

	GLint *offsets = (GLint*)malloc(numUniforms*sizeof(GLint));
	glGetActiveUniformsiv( _shaderProgramHandle, numUniforms, indices, GL_UNIFORM_OFFSET, offsets );
	return offsets;
}

GLint* CSCI441::ShaderProgram::getUniformBlockOffsets( GLint uniformBlockIndex, const char *names[] ) {
	GLint numUniforms;
	glGetActiveUniformBlockiv( _shaderProgramHandle, uniformBlockIndex, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &numUniforms );

	GLuint *indices = (GLuint*)malloc(numUniforms*sizeof(GLuint));
	glGetUniformIndices( _shaderProgramHandle, numUniforms, names, indices );

	GLint *offsets = (GLint*)malloc(numUniforms*sizeof(GLint));
	glGetActiveUniformsiv( _shaderProgramHandle, numUniforms, indices, GL_UNIFORM_OFFSET, offsets );
	return offsets;
}

void CSCI441::ShaderProgram::setUniformBlockBinding( const char *uniformBlockName, GLuint binding ) {
	glUniformBlockBinding( _shaderProgramHandle, getUniformBlockIndex(uniformBlockName), binding );
}

GLint CSCI441::ShaderProgram::getAttributeLocation( const char *attributeName ) {
	return glGetAttribLocation( _shaderProgramHandle, attributeName );
}

GLuint CSCI441::ShaderProgram::getSubroutineIndex( GLenum shaderStage, const char *subroutineName ) {
	return glGetSubroutineIndex( _shaderProgramHandle, shaderStage, subroutineName );
} 

GLuint CSCI441::ShaderProgram::getNumUniforms() {
	int numUniform = 0;
	glGetProgramiv( _shaderProgramHandle, GL_ACTIVE_UNIFORMS, &numUniform );
	return numUniform;
}

GLuint CSCI441::ShaderProgram::getNumUniformBlocks() {
	int numUniformBlocks = 0;
	glGetProgramiv( _shaderProgramHandle, GL_ACTIVE_UNIFORM_BLOCKS, &numUniformBlocks );
	return numUniformBlocks;
}

GLuint CSCI441::ShaderProgram::getNumAttributes() {
	int numAttr = 0;
	glGetProgramiv( _shaderProgramHandle, GL_ACTIVE_ATTRIBUTES, &numAttr );
	return numAttr;
}

GLuint CSCI441::ShaderProgram::getShaderProgramHandle() {
	return _shaderProgramHandle;
}

void CSCI441::ShaderProgram::useProgram() {
	glUseProgram( _shaderProgramHandle );
}

CSCI441::ShaderProgram::ShaderProgram() {}

CSCI441::ShaderProgram::~ShaderProgram() {
	glDeleteShader( _vertexShaderHandle );
	glDeleteShader( _tesselationControlShaderHandle );
	glDeleteShader( _tesselationEvaluationShaderHandle );
	glDeleteShader( _geometryShaderHandle );
	glDeleteShader( _fragmentShaderHandle );
	glDeleteProgram( _shaderProgramHandle );
}
