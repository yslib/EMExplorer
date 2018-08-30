#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_3_1>
#include <iostream>

class ShaderDataInterface;
class ShaderProgram :public QOpenGLShaderProgram
{
public:
    ShaderProgram();
	virtual void load(const ShaderDataInterface * data) = 0;
	void setUniformSampler(const std::string &name, GLenum texUnit, GLenum target, GLuint texID);
private:
	QOpenGLFunctions m_glfuncs;
};


class PositionShader:public ShaderProgram
{
public:
	PositionShader();
	void load(const ShaderDataInterface * data)override;
};


class RayCastingShader:public ShaderProgram
{
public:
	RayCastingShader();
	void load(const ShaderDataInterface * data)override;
};




class FramebufferObject
{
public:
	/// Ctor/Dtor
	FramebufferObject();
	virtual ~FramebufferObject();
	/// Bind this FBO as current render target
	void Bind();

	/// Bind a texture to the "attachment" point of this FBO
	virtual void AttachTexture(GLenum texTarget,
		GLuint texId,
		GLenum attachment = GL_COLOR_ATTACHMENT0_EXT,
		int mipLevel = 0,
		int zSlice = 0);

	/// Bind an array of textures to multiple "attachment" points of this FBO
	///  - By default, the first 'numTextures' attachments are used,
	///    starting with GL_COLOR_ATTACHMENT0_EXT
	virtual void AttachTextures(int numTextures,
		GLenum texTarget[],
		GLuint texId[],
		GLenum attachment[] = NULL,
		int mipLevel[] = NULL,
		int zSlice[] = NULL);

	/// Bind a render buffer to the "attachment" point of this FBO
	virtual void AttachRenderBuffer(GLuint buffId,
		GLenum attachment = GL_COLOR_ATTACHMENT0_EXT);

	/// Bind an array of render buffers to corresponding "attachment" points
	/// of this FBO.
	/// - By default, the first 'numBuffers' attachments are used,
	///   starting with GL_COLOR_ATTACHMENT0_EXT
	virtual void AttachRenderBuffers(int numBuffers, GLuint buffId[],
		GLenum attachment[] = NULL);

	/// Free any resource bound to the "attachment" point of this FBO
	void Unattach(GLenum attachment);

	/// Free any resources bound to any attachment points of this FBO
	void UnattachAll();

	/// Is this FBO currently a valid render target?
	///  - Sends output to std::cerr by default but can
	///    be a user-defined C++ stream
	///
	/// NOTE : This function works correctly in debug build
	///        mode but always returns "true" if NDEBUG is
	///        is defined (optimized builds)
#ifndef NDEBUG
	bool IsValid(std::ostream& ostr = std::cerr);
#else
	bool IsValid(std::ostream& ostr = std::cerr) {
		return true;
	}
#endif

	/// BEGIN : Accessors
	/// Is attached type GL_RENDERBUFFER_EXT or GL_TEXTURE?
	GLenum GetAttachedType(GLenum attachment);

	/// What is the Id of Renderbuffer/texture currently 
	/// attached to "attachement?"
	GLuint GetAttachedId(GLenum attachment);

	/// Which mipmap level is currently attached to "attachement?"
	GLint  GetAttachedMipLevel(GLenum attachment);

	/// Which cube face is currently attached to "attachment?"
	GLint  GetAttachedCubeFace(GLenum attachment);

	/// Which z-slice is currently attached to "attachment?"
	GLint  GetAttachedZSlice(GLenum attachment);
	/// END : Accessors


	/// BEGIN : Static methods global to all FBOs
	/// Return number of color attachments permitted
	static int GetMaxColorAttachments();

	/// Disable all FBO rendering and return to traditional,
	/// windowing-system controlled framebuffer
	///  NOTE:
	///     This is NOT an "unbind" for this specific FBO, but rather
	///     disables all FBO rendering. This call is intentionally "static"
	///     and named "Disable" instead of "Unbind" for this reason. The
	///     motivation for this strange semantic is performance. Providing
	///     "Unbind" would likely lead to a large number of unnecessary
	///     FBO enablings/disabling.
	static void Disable();
	/// END : Static methods global to all FBOs

protected:
	void  _GuardedBind();
	void  _GuardedUnbind();
	void  _FramebufferTextureND(GLenum attachment, GLenum texTarget,
		GLuint texId, int mipLevel, int zSlice);
	static GLuint _GenerateFboId();

private:
	GLuint m_fboId;
	GLint  m_savedFboId;
	QOpenGLFunctions_3_1 m_glfuncs;
};



#endif // SHADERPROGRAM_H