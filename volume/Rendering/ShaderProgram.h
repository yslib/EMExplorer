#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <GL/glew.h>
#include "Vector.h"
#include "matrix2.h"
#include "matrix3.h"
#include "matrix4.h"
#include "volume/Rendering/Shader.h"
#include "volume/ShaderData.h"

#include <list>

// forward declarations
class mShader;

/**
 * ShaderProgram class
 * Consists of compiled Shader, linked together
 */
class ShaderProgram {
public:
	/// default constructor
    ShaderProgram();

    /// Detaches all shader objects, deletes them an disposes all textures
    ~ShaderProgram();
	// init CG Function
	virtual bool initShader()
	{
		return true;
	}

	// load shader
	virtual void loadShader(ShaderData& parameters)
	{

	}

	// unload shader
	virtual void unloadShader()
	{

	}

	// is TF 1D needed
	virtual bool requireTF1D()       { return false; }

	// is TF 2D needed
	virtual bool requireTF2D()       { return false; }

	// is Lighting needed
	virtual bool requireLighting()   { return false; }

	// is len supported
	virtual bool requireLen()        { return false; }

	/// Get the shader program ID
    GLint getID() const { return id; }

	/**
	* Load filename.vert and filename.frag (vertex and fragment shader) and link shader
	* An optional header is added @see ShaderObject::generateHeader().
	*
	* @param customHeader Header to be put in front of the shader source. 
	* @param processHeader Header is a list of symbols ("a b") which will be processed to
	*      generate #define statements ("#define a ...")
	*/
	bool loadShader(const std::string &vertFilename, const std::string &fragFilename,
					const std::string &geomFilename = "");

	std::list<mShader*> getShaders() const {return shaders;};

    /// Attach the shader to Shader Program
    void attachShader(mShader *pShader);

	/// Detach the shader from Shader Program
	void detachShader(mShader *pShader);

	/// Detach the shader by the specified shader type
    void detachShadersByType(mShader::ShaderType type);	


    /**
     * Link all shader objects to one shader program
	 * Will re-link already linked shaders.
     * @return true for success
     */
    bool linkProgram();

	/// Rebuild all shaders and link these shaders again
    bool rebuild();
	
	/// Rebuild all shaders from files and link these shaders again
    bool rebuildFromFile();	
	
	/// Returns whether the shader program is linked
    bool isLinked() const { return linked; }

	/// Get the linker log
    std::string getLinkerLog() const;


    // Activates the shader program
	void activate(); /*{
		if (linked) 
			glUseProgram(id); 
	}*/

	/// Activates the shader program through the id
	static void activate(GLint id);/* { glUseProgram(id); }*/

	/// Deactivate the shader program
	static void deactivate(); /*{ glUseProgram(0); }*/

	/// Get the id of the current shader program
    static GLint getCurrentProgram();

    /// Returns whether the shader program is currently activated
    bool isActivated();


    //
    // Uniform stuff
    //

    /**
     * Returns uniform location, or -1 on failure
     */
    GLint getUniformLocation(const std::string &name);
    
	void setIgnoreUniformLocationError(bool ignoreError_) { ignoreError = ignoreError_; }
	bool getIgnoreUniformLocationError() const { return ignoreError; }

    // Floats
    bool setUniform(const std::string &name, GLfloat value);
    bool setUniform(const std::string &name, GLfloat v1, GLfloat v2);
    bool setUniform(const std::string &name, GLfloat v1, GLfloat v2, GLfloat v3);
    bool setUniform(const std::string &name, GLfloat v1, GLfloat v2, GLfloat v3, GLfloat v4);
    bool setUniform(const std::string &name, GLfloat *v, int count);

    // Integers
    bool setUniform(const std::string &name, GLint value);
    bool setUniform(const std::string &name, GLint v1, GLint v2);
    bool setUniform(const std::string &name, GLint v1, GLint v2, GLint v3);
    bool setUniform(const std::string &name, GLint v1, GLint v2, GLint v3, GLint v4);
    bool setUniform(const std::string &name, GLint *v, int count);

    // Booleans
    bool setUniform(const std::string &name, bool value);
    bool setUniform(const std::string &name, bool v1, bool v2);
    bool setUniform(const std::string &name, bool v1, bool v2, bool v3);
    bool setUniform(const std::string &name, bool v1, bool v2, bool v3, bool v4);
    bool setUniform(const std::string &name, GLboolean *v, int count);

    // Vectors
    bool setUniform(const std::string &name, const Vector2f &value);
    bool setUniform(const std::string &name, Vector2f *vectors, GLsizei count = 1);
    bool setUniform(const std::string &name, const Vector3f &value);
    bool setUniform(const std::string &name, Vector3f *vectors, GLsizei count = 1);
    bool setUniform(const std::string &name, const Vector4f &value);
    bool setUniform(const std::string &name, Vector4f* vectors, GLsizei count = 1);
    bool setUniform(const std::string &name, const Vector2i &value);
    bool setUniform(const std::string &name, Vector2i *vectors, GLsizei count = 1);
    bool setUniform(const std::string &name, const Vector3i &value);
    bool setUniform(const std::string &name, Vector3i *vectors, GLsizei count = 1);
    bool setUniform(const std::string &name, const Vector4i &value);
    bool setUniform(const std::string &name, Vector4i *vectors, GLsizei count = 1);

    // Note: Matrix is transposed by OpenGL
    bool setUniform(const std::string &name, const Matrix2f &value, bool transpose = false);
    bool setUniform(const std::string &name, const Matrix3f &value, bool transpose = false);
    bool setUniform(const std::string &name, const Matrix4f &value, bool transpose = false);

    // No location lookup
    // 
    // Floats    
    static void setUniform(GLint l, GLfloat value);
    static void setUniform(GLint l, GLfloat v1, GLfloat v2);
    static void setUniform(GLint l, GLfloat v1, GLfloat v2, GLfloat v3);
    static void setUniform(GLint l, GLfloat v1, GLfloat v2, GLfloat v3, GLfloat v4);

    // Integers
    static void setUniform(GLint l, GLint value);
    static void setUniform(GLint l, GLint v1, GLint v2);
    static void setUniform(GLint l, GLint v1, GLint v2, GLint v3);
    static void setUniform(GLint l, GLint v1, GLint v2, GLint v3, GLint v4);

    // Vectors
    static void setUniform(GLint l, const Vector2f &value);
    static void setUniform(GLint l, const Vector3f &value);
    static void setUniform(GLint l, const Vector4f &value);
    static void setUniform(GLint l, const Vector2i &value);
    static void setUniform(GLint l, const Vector3i &value);
    static void setUniform(GLint l, const Vector4i &value);
    static void setUniform(GLint l, const Matrix2f &value, bool transpose = false);
    static void setUniform(GLint l, const Matrix3f &value, bool transpose = false);
    static void setUniform(GLint l, const Matrix4f &value, bool transpose = false);

    // Attributes
    // 
    // 1 component
    static void setAttribute(GLint index, GLfloat v1);
    static void setAttribute(GLint index, GLshort v1);
    static void setAttribute(GLint index, GLdouble v1);

    // 2 components
    static void setAttribute(GLint index, GLfloat v1, GLfloat v2);
    static void setAttribute(GLint index, GLshort v1, GLshort v2);
    static void setAttribute(GLint index, GLdouble v1, GLdouble v2);

    // 3 components
    static void setAttribute(GLint index, GLfloat v1, GLfloat v2, GLfloat v3);
    static void setAttribute(GLint index, GLshort v1, GLshort v2, GLshort v3);
    static void setAttribute(GLint index, GLdouble v1, GLdouble v2, GLdouble v3);

    // 4 components
    static void setAttribute(GLint index, GLfloat v1, GLfloat v2, GLfloat v3, GLfloat v4);
    static void setAttribute(GLint index, GLshort v1, GLshort v2, GLshort v3, GLshort v4);
    static void setAttribute(GLint index, GLdouble v1, GLdouble v2, GLdouble v3, GLdouble v4);

    // For vectors
    static void setAttribute(GLint index, const Vector2f &v);
    static void setAttribute(GLint index, const Vector3f &v);
    static void setAttribute(GLint index, const Vector4f &v);

    static void setAttribute(GLint index, const Vector2d &v);
    static void setAttribute(GLint index, const Vector3d &v);
    static void setAttribute(GLint index, const Vector4d &v);

    static void setAttribute(GLint index, const Vector2<GLshort> &v);
    static void setAttribute(GLint index, const Vector3<GLshort> &v);
    static void setAttribute(GLint index, const Vector4<GLshort> &v);

    static void setAttribute(GLint index, const Vector4<GLint> &v);
    static void setAttribute(GLint index, const Vector4<GLbyte> &v);
    static void setAttribute(GLint index, const Vector4<GLubyte> &v);
    static void setAttribute(GLint index, const Vector4<GLushort> &v);
    static void setAttribute(GLint index, const Vector4<GLuint> &v);

    // Attribute locations
    void setAttributeLocation(GLuint index, const std::string &name);
    GLint getAttributeLocation(const std::string &name);

    // Normalized attributes
    static void setNormalizedAttribute(GLint index, GLubyte v1, GLubyte v2, GLubyte v3, GLubyte v4);

    static void setNormalizedAttribute(GLint index, const Vector4<GLbyte> &v);
    static void setNormalizedAttribute(GLint index, const Vector4<GLshort> &v);
    static void setNormalizedAttribute(GLint index, const Vector4<GLint> &v);

    // Unsigned version
    static void setNormalizedAttribute(GLint index, const Vector4<GLubyte> &v);
    static void setNormalizedAttribute(GLint index, const Vector4<GLushort> &v);
    static void setNormalizedAttribute(GLint index, const Vector4<GLuint> &v);

	// Uniform sampler
	bool setUniformSampler(const std::string &name, GLenum texUnit, GLenum target, GLuint texID);

protected:
	/// the shaders in this shader program
	std::list<mShader*> shaders;

	/// the OpenGL id of this shader program
    GLuint id;
	/// the flag indicates whether the shader program is linked
    bool linked;
	/// the flag indicates whether to ignore error
    bool ignoreError;

	/// the logger category
    static const std::string loggerCat;
};

#endif // SHADERPROGRAM_H