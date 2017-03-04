#pragma once

#include "ShaderUtils.h"

#include <GL/gl.h>

namespace CSCI441 {

	class ShaderProgram {
	public:
		ShaderProgram( const char *vertexShaderFilename,
					   const char *fragmentShaderFilename );

		ShaderProgram( const char *vertexShaderFilename,
					   const char *tesselationControlShaderFilename,
					   const char *tesselationEvaluationShaderFilename,
					   const char *geometryShaderFilename,
					   const char *fragmentShaderFilename );

		ShaderProgram( const char *vertexShaderFilename,
					   const char *geometryShaderFilename,
					   const char *fragmentShaderFilename );

		~ShaderProgram();

		GLint getUniformLocation( const char *uniformName );

		GLint getUniformBlockIndex( const char *uniformBlockName );
		GLint getUniformBlockSize( const char *uniformBlockName );
		GLubyte* getUniformBlockBuffer( const char *uniformBlockName );
		GLint* getUniformBlockOffsets( const char *uniformBlockName );
		GLint* getUniformBlockOffsets( const char *uniformBlockName, const char *names[] );
		void setUniformBlockBinding( const char *uniformBlockName, GLuint binding );

		GLint getAttributeLocation( const char *attributeName );

		GLuint getSubroutineIndex( GLenum shaderStage, const char *subroutineName );

		GLuint getNumUniforms();
		GLuint getNumUniformBlocks();
		GLuint getNumAttributes();

		GLuint getShaderProgramHandle();

		void useProgram();
	private:
		ShaderProgram();

		GLuint _vertexShaderHandle;
		GLuint _tesselationControlShaderHandle;
		GLuint _tesselationEvaluationShaderHandle;
		GLuint _geometryShaderHandle;
		GLuint _fragmentShaderHandle;

		GLuint _shaderProgramHandle;

		bool registerShaderProgram( const char *vertexShaderFilename,
									const char *tesselationControlShaderFilename,
									const char *tesselationEvaluationShaderFilename,
									const char *geometryShaderFilename,
									const char *fragmentShaderFilename );

		GLint* getUniformBlockOffsets( GLint uniformBlockIndex );
		GLint* getUniformBlockOffsets( GLint uniformBlockIndex, const char *names[] );
	};

}
