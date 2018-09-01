#include "shaderprogram.h"
#include "shaderdatainterface.h"
#include <iostream>

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
	m_glfuncs.initializeOpenGLFunctions();
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

PositionShader::PositionShader():ShaderProgram()
{

	static const char * positionFragShaderSource =
		"#version 150\n"
		"in vec4 texCoord;\n"
		"out vec4 fragColor;\n"
		"void main(){\n"
		" fragColor = texCoord;\n"
		"}\n";

	static const char * positionVertShaderSource =
		"#version 150\n"
		"#extension GL_ARB_explicit_attrib_location : enable\n"
		"layout (location = 0) in vec3 vertex;\n"
		"layout (location = 1) in vec3 tex;\n"
		"out vec4 texCoord;\n"
		"uniform mat4 projMatrix;\n"
		"uniform mat4 othoMatrix;\n"
		"uniform mat4 worldMatrix;\n"
		"uniform mat4 viewMatrix;\n"
		"void main() {\n"
		"	texCoord = vec4(tex,1.0);\n"
		"   gl_Position = othoMatrix * viewMatrix * worldMatrix * vec4(vertex,1.0);\n"
		"}\n";

	addShaderFromSourceCode(QOpenGLShader::Fragment, positionFragShaderSource);
	addShaderFromSourceCode(QOpenGLShader::Vertex, positionVertShaderSource);

}

void PositionShader::load(const ShaderDataInterface* data)
{
	this->bind();
	this->setUniformValue("projMatrix",data->getPerspMatrix());
	this->setUniformValue("othoMatrix",data->getOthoMatrix());
	this->setUniformValue("worldMatrix",data->getModelMatrix());
	this->setUniformValue("viewMatrix",data->getViewMatrix());
}

RayCastingShader::RayCastingShader() :ShaderProgram()
{

	addShaderFromSourceFile(QOpenGLShader::Fragment, "D:\\code\\MRCEditor\\volume\\Shaders\\raycast_lighting.frag");
	static const char * raycastingShaderSource=
		"#version 150\n"
		"#extension GL_ARB_explicit_attrib_location : enable\n"
		"layout (location = 0 ) in vec2 vertex;\n"
		"layout (location = 1 ) in vec2 tex;\n"
		"out vec2 textureRectCoord;\n"
		"uniform mat4 othoMatrix;\n"
		"uniform mat4 viewMatrix;\n"
		"void main() {\n"
		"	textureRectCoord = tex;\n"
		"   gl_Position = othoMatrix*viewMatrix*vec4(vertex.x,vertex.y,0.0,1.0);\n"
		"}\n";
	addShaderFromSourceCode(QOpenGLShader::Vertex, raycastingShaderSource);
}

void RayCastingShader::load(const ShaderDataInterface* data)
{
	this->bind();
	QVector3D L = data->getLightDirection();
	QVector3D H = L - data->getCameraTowards();
	if (H.length() > 1e-10) H.normalize();

	this->setUniformSampler("texVolume", GL_TEXTURE3, GL_TEXTURE_3D, data->getVolumeTexIdx());
	this->setUniformSampler("texStartPos", GL_TEXTURE0, GL_TEXTURE_RECTANGLE_NV, data->getStartPosTexIdx());
	this->setUniformSampler("texEndPos", GL_TEXTURE1, GL_TEXTURE_RECTANGLE_NV, data->getEndPosTexIdx());
	this->setUniformSampler("texTransfunc", GL_TEXTURE2, GL_TEXTURE_1D, data->getTF1DIdx());
	this->setUniformValue("viewMatrix", data->getViewMatrix());
	this->setUniformValue("step", data->getRayStep());
	this->setUniformValue("ka", data->getAmbient());
	this->setUniformValue("ks", data->getSpecular());
	this->setUniformValue("kd", data->getDiffuse());
	this->setUniformValue("shininess", data->getShininess());
	this->setUniformValue("lightdir", (float)L.x(), (float)L.y(), (float)L.z());
	this->setUniformValue("halfway", (float)H.x(), (float)H.y(), (float)H.z());
}

///////////
///////////////////////////
///////////////////////////
//////////////////////////

/*
Copyright (c) 2005,
Aaron Lefohn	  (lefohn@cs.ucdavis.edu)
Robert Strzodka (strzodka@stanford.edu)
Adam Moerschell (atmoerschell@ucdavis.edu)
All rights reserved.

This software is licensed under the BSD open-source license. See
http://www.opensource.org/licenses/bsd-license.php for more detail.

*************************************************************
Redistribution and use in source and binary forms, with or
without modification, are permitted provided that the following
conditions are met:

Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

Neither the name of the University of Californa, Davis nor the names of
the contributors may be used to endorse or promote products derived
from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/


FramebufferObject::FramebufferObject()
	: m_fboId(_GenerateFboId()),
	m_savedFboId(0)
{
	// Bind this FBO so that it actually gets created now
	m_glfuncs.initializeOpenGLFunctions();
	_GuardedBind();
	_GuardedUnbind();
}

FramebufferObject::~FramebufferObject()
{
	m_glfuncs.glDeleteFramebuffers(1, &m_fboId);
}

void FramebufferObject::Bind()
{
	m_glfuncs.glBindFramebuffer(GL_FRAMEBUFFER_EXT, m_fboId);
}
void FramebufferObject::Disable()
{
	QOpenGLFunctions_3_1 glfuncs;
	glfuncs.initializeOpenGLFunctions();
	glfuncs.glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
}

void
FramebufferObject::AttachTexture(GLenum texTarget, GLuint texId,
	GLenum attachment, int mipLevel, int zSlice)
{
	_GuardedBind();

#ifndef NDEBUG
	if (GetAttachedId(attachment) != texId) {
#endif

		_FramebufferTextureND(attachment, texTarget,
			texId, mipLevel, zSlice);

#ifndef NDEBUG
	}
	else {
		std::cerr << "FramebufferObject::AttachTexture PERFORMANCE WARNING:\n"
			<< "\tRedundant bind of texture (id = " << texId << ").\n"
			<< "\tHINT : Compile with -DNDEBUG to remove this warning.\n";
	}
#endif

	_GuardedUnbind();
}

void
FramebufferObject::AttachTextures(int numTextures, GLenum texTarget[], GLuint texId[],
	GLenum attachment[], int mipLevel[], int zSlice[])
{
	for (int i = 0; i < numTextures; ++i) {
		AttachTexture(texTarget[i], texId[i],
			attachment ? attachment[i] : (GL_COLOR_ATTACHMENT0_EXT + i),
			mipLevel ? mipLevel[i] : 0,
			zSlice ? zSlice[i] : 0);
	}
}

void
FramebufferObject::AttachRenderBuffer(GLuint buffId, GLenum attachment)
{
	_GuardedBind();

#ifndef NDEBUG
	if (GetAttachedId(attachment) != buffId) {
#endif

		m_glfuncs.glFramebufferRenderbuffer(GL_FRAMEBUFFER_EXT, attachment,
			GL_RENDERBUFFER_EXT, buffId);

#ifndef NDEBUG
	}
	else {
		std::cerr << "FramebufferObject::AttachRenderBuffer PERFORMANCE WARNING:\n"
			<< "\tRedundant bind of Renderbuffer (id = " << buffId << ")\n"
			<< "\tHINT : Compile with -DNDEBUG to remove this warning.\n";
	}
#endif

	_GuardedUnbind();
}

void
FramebufferObject::AttachRenderBuffers(int numBuffers, GLuint buffId[], GLenum attachment[])
{
	for (int i = 0; i < numBuffers; ++i) {
		AttachRenderBuffer(buffId[i],
			attachment ? attachment[i] : (GL_COLOR_ATTACHMENT0_EXT + i));
	}
}

void
FramebufferObject::Unattach(GLenum attachment)
{
	_GuardedBind();
	GLenum type = GetAttachedType(attachment);

	switch (type) {
	case GL_NONE:
		break;
	case GL_RENDERBUFFER_EXT:
		AttachRenderBuffer(0, attachment);
		break;
	case GL_TEXTURE:
		AttachTexture(GL_TEXTURE_2D, 0, attachment);
		break;
	default:
		std::cerr << "FramebufferObject::unbind_attachment ERROR: Unknown attached resource type\n";
	}
	_GuardedUnbind();
}

void
FramebufferObject::UnattachAll()
{
	int numAttachments = GetMaxColorAttachments();
	for (int i = 0; i < numAttachments; ++i) {
		Unattach(GL_COLOR_ATTACHMENT0_EXT + i);
	}
}

GLint FramebufferObject::GetMaxColorAttachments()
{
	GLint maxAttach = 0;
	QOpenGLFunctions_3_1 glfuncs;
	glfuncs.initializeOpenGLFunctions();
	glfuncs.glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT, &maxAttach);
	return maxAttach;
}

GLuint FramebufferObject::_GenerateFboId()
{
	GLuint id = 0;
	QOpenGLFunctions_3_1 glfuncs;
	glfuncs.initializeOpenGLFunctions();
	glfuncs.glGenFramebuffers(1, &id);
	return id;
}

void FramebufferObject::_GuardedBind()
{
	// Only binds if m_fboId is different than the currently bound FBO
	m_glfuncs.glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &m_savedFboId);
	if (m_fboId != (GLuint)m_savedFboId) {
		m_glfuncs.glBindFramebuffer(GL_FRAMEBUFFER_EXT, m_fboId);
	}
}

void FramebufferObject::_GuardedUnbind()
{
	// Returns FBO binding to the previously enabled FBO
	if (m_fboId != (GLuint)m_savedFboId) {
		m_glfuncs.glBindFramebuffer(GL_FRAMEBUFFER_EXT, (GLuint)m_savedFboId);
	}
}

void
FramebufferObject::_FramebufferTextureND(GLenum attachment, GLenum texTarget,
	GLuint texId, int mipLevel,
	int zSlice)
{
	if (texTarget == GL_TEXTURE_1D) {
		m_glfuncs.glFramebufferTexture1D(GL_FRAMEBUFFER_EXT, attachment,
			GL_TEXTURE_1D, texId, mipLevel);
	}
	else if (texTarget == GL_TEXTURE_3D) {
		m_glfuncs.glFramebufferTexture3D(GL_FRAMEBUFFER_EXT, attachment,
			GL_TEXTURE_3D, texId, mipLevel, zSlice);
	}
	else {
		// Default is GL_TEXTURE_2D, GL_TEXTURE_RECTANGLE_ARB, or cube faces
		m_glfuncs.glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, attachment,
			texTarget, texId, mipLevel);
	}
}

#define NDEBUG

#ifndef NDEBUG
bool FramebufferObject::IsValid(std::ostream& ostr)
{
	_GuardedBind();

	bool isOK = false;

	GLenum status;
	status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	switch (status) {
	case GL_FRAMEBUFFER_COMPLETE_EXT: // Everything's OK
		isOK = true;
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
		ostr << "glift::CheckFramebufferStatus() ERROR:\n\t"
			<< "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT\n";
		isOK = false;
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
		ostr << "glift::CheckFramebufferStatus() ERROR:\n\t"
			<< "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT\n";
		isOK = false;
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
		ostr << "glift::CheckFramebufferStatus() ERROR:\n\t"
			<< "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT\n";
		isOK = false;
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
		ostr << "glift::CheckFramebufferStatus() ERROR:\n\t"
			<< "GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT\n";
		isOK = false;
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
		ostr << "glift::CheckFramebufferStatus() ERROR:\n\t"
			<< "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT\n";
		isOK = false;
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
		ostr << "glift::CheckFramebufferStatus() ERROR:\n\t"
			<< "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT\n";
		isOK = false;
		break;
	case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
		ostr << "glift::CheckFramebufferStatus() ERROR:\n\t"
			<< "GL_FRAMEBUFFER_UNSUPPORTED_EXT\n";
		isOK = false;
		break;
	default:
		ostr << "glift::CheckFramebufferStatus() ERROR:\n\t"
			<< "Unknown ERROR\n";
		isOK = false;
	}

	_GuardedUnbind();
	return isOK;
}
#endif // NDEBUG

/// Accessors
GLenum FramebufferObject::GetAttachedType(GLenum attachment)
{
	// Returns GL_RENDERBUFFER_EXT or GL_TEXTURE
	_GuardedBind();
	GLint type = 0;
	m_glfuncs.glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER_EXT, attachment,
		GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT,
		&type);
	_GuardedUnbind();
	return GLenum(type);
}

GLuint FramebufferObject::GetAttachedId(GLenum attachment)
{
	_GuardedBind();
	GLint id = 0;
	m_glfuncs.glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER_EXT, attachment,
		GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT,
		&id);
	_GuardedUnbind();
	return GLuint(id);
}

GLint FramebufferObject::GetAttachedMipLevel(GLenum attachment)
{
	_GuardedBind();
	GLint level = 0;
	m_glfuncs.glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER_EXT, attachment,
		GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL_EXT,
		&level);
	_GuardedUnbind();
	return level;
}

GLint FramebufferObject::GetAttachedCubeFace(GLenum attachment)
{
	_GuardedBind();
	GLint level = 0;
	m_glfuncs.glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER_EXT, attachment,
		GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE_EXT,
		&level);
	_GuardedUnbind();
	return level;
}

GLint FramebufferObject::GetAttachedZSlice(GLenum attachment)
{
	_GuardedBind();
	GLint slice = 0;
	m_glfuncs.glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, attachment,
		GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_3D_ZOFFSET_EXT,
		&slice);
	_GuardedUnbind();
	return slice;
}


