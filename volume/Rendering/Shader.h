#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <GL\glew.h>

/**
 * Shader class
 * Shader type: vertex, fragment, or geometry shader
 * Shaders support '#include' statements
 * 
 * Geometry shaders can be controlled using directives in shader source.
 * Accepted directives:
 * GL_GEOMETRY_INPUT_TYPE_EXT(GL_POINTS | GL_LINES | GL_LINES_ADJACENCY_EXT | GL_TRIANGLES | GL_TRIANGLES_ADJACENCY_EXT)
 * GL_GEOMETRY_OUTPUT_TYPE_EXT(GL_POINTS | GL_LINE_STRIP | GL_TRIANGLE_STRIP)
 * GL_GEOMETRY_VERTICES_OUT_EXT(<int>)
 * No newline or space allowed between each pair of brackets.
 *
 * Example geometry shader header:
 * #version 120 
 * #extension GL_EXT_geometry_shader4 : enable
 * //GL_GEOMETRY_INPUT_TYPE_EXT(GL_LINES)
 * //GL_GEOMETRY_OUTPUT_TYPE_EXT(GL_LINE_STRIP)
 * //GL_GEOMETRY_VERTICES_OUT_EXT(42)
 * [...]
 */
class mShader {
public:
	friend class ShaderProgram;
	
	enum ShaderType {
		VERTEX_SHADER   = GL_VERTEX_SHADER,
		FRAGMENT_SHADER = GL_FRAGMENT_SHADER,
		GEOMETRY_SHADER = GL_GEOMETRY_SHADER_EXT
	};

	/**
	 * Shader constructor
	 *
	 * @param filename  the name of the shader file
	 * @param type      the type of the shader
	 */
	mShader(const std::string& filename, ShaderType type = VERTEX_SHADER);

	/**
	 * destructor
	 */
	~mShader();

	/**
	 * Get the shader type
	 */
	ShaderType getType() const { return shaderType; }

	/**
	 * Set the source of the shader
	 */
	void setSource(const std::string &source) {
		this->source = source;
	}

	/**
	 * Load the shader source from the file
	 *
	 * @param filename  the name of the shader file
	 */
	bool loadSourceFromFile(const std::string &filename);

	/**
	 * Get the source of the shader
	 */
	std::string getSource() const { return unparsedSource; }

	/**
	 * Compile the shader
	 */
	bool compileShader();

	/**
	 * Rebuild the shader from the file
	 */
	bool rebuildFromFile();

	/**
	 * Is the shader compiled
	 */
	bool isCompiled() const { return compiled; }

	/**
	 * Get the compiler log
	 */
	std::string getCompilerLog() const;

	/**
	 * Set maximum number of primitives a geometry shader can create.
	 * For this change to take effect call setDirectives()
	 * re-link already linked shaders. Limited by GL_MAX_GEOMETRY_OUTPUT_VERTICES_EXT.
	 */
	void setVerticesOut(GLint verticesOut) { this->verticesOut = verticesOut; }
	GLint getVerticesOut() const { return verticesOut; }

	char* loadShader(const char *filename);

protected:
	/**
	 * update the source to the GPU shader
	 */
	void uploadSource();

protected:
	ShaderType shaderType;		///< the type of the shader
	std::string filename;		///< the filename of the shader source

	GLuint id;					///< the id of the shader in OpenGL
	std::string source;			///< the source of the shader
	std::string unparsedSource;	///< the unparsed source of the shader
	std::string header;			///< the header of the shader
	bool compiled;				///< the flag indicates whether the shader is compiled
// 	GLint inputType;			///< the input type of the geometry shader
// 	GLint outputType;			///< the output type of the geometry shader
	GLint verticesOut;			///< the maximum number of primitives (vertices) of the geometry shader
};

#endif // SHADER_H