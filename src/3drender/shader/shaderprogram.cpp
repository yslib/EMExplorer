#include "shaderprogram.h"
//#include "shaderdatainterface.h"

#define GLERROR(str)									\
	{													\
		GLenum err;										\
		while ((err = m_glfuncs.glGetError()) != GL_NO_ERROR)		\
		{												\
			std::cout<<err<<" "<<str<<std::endl;		\
		}												\
	}	

ShaderProgram::ShaderProgram():QOpenGLShaderProgram()
{
	if(m_glfuncs.initializeOpenGLFunctions() == false) {
		qWarning("OpenGL Functions initialize failed");
		return;
	}
}

void ShaderProgram::setUniformSampler(const std::string& name, GLenum texUnit, GLenum target, GLuint texID)
{
	GLint l = uniformLocation(name.c_str());
	if (l == -1)
		return;
	m_glfuncs.glActiveTexture(texUnit);		
	m_glfuncs.glBindTexture(target, texID);
	m_glfuncs.glUniform1i(l, texUnit - GL_TEXTURE0); 
}
