#pragma once

#include "ShaderUtils.h"

#include <glm/glm.hpp>
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;
using glm::mat3;

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

		ShaderProgram( const char *vertexShaderFilename,
					   const char *geometryShaderFilename,
					   const char *fragmentShaderFilename,
					   const char *computeShaderFilename);

		~ShaderProgram();

		GLint getUniformLocation( const char *uniformName );
		GLint getAttributeLocation( const char *attributeName );
		GLuint getSubroutineIndex( GLenum shaderStage, const char *subroutineName );

		void   setUniform( const char *name, float x, float y, float z);
		void   setUniform( const char *name, const vec2 & v);
		void   setUniform( const char *name, const vec3 & v);
		void   setUniform( const char *name, const vec4 & v);
		void   setUniform( const char *name, const mat4 & m);
		void   setUniform( const char *name, const mat3 & m);
		void   setUniform( const char *name, float val );
		void   setUniform( const char *name, int val );
		void   setUniform( const char *name, bool val );
		void   setUniform( const char *name, GLuint val );

		GLuint getNumUniforms();
		GLuint getNumAttributes();

		void useProgram();
		
	private:
		
		ShaderProgram();
		GLuint _vertexShaderHandle;
		GLuint _tesselationControlShaderHandle;
		GLuint _tesselationEvaluationShaderHandle;
		GLuint _geometryShaderHandle;
		GLuint _fragmentShaderHandle;
		GLuint _computeShaderHandle;

		GLuint _shaderProgramHandle;

		bool registerShaderProgram( const char *vertexShaderFilename,
									const char *tesselationControlShaderFilename,
									const char *tesselationEvaluationShaderFilename,
									const char *geometryShaderFilename,
									const char *fragmentShaderFilename,
									const char *computeShaderFilename );
	};

}
