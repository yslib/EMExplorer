#include "volume/Rendering/shaderprogram.h"

ShaderProgram::ShaderProgram()
	: linked(false), ignoreError(false)
{
	id = glCreateProgram();
	if (id == 0)
		printf("ShaderProgram(): glCreateProgram() returned 0\n");
}

ShaderProgram::~ShaderProgram()
{
	for (std::list<mShader*>::iterator iter = shaders.begin(); iter != shaders.end(); ++iter) {
		glDetachShader(id, (*iter)->id);
		delete (*iter);
	}
	glDeleteProgram(id);
}

bool ShaderProgram::loadShader(const std::string &vert_filename, const std::string &frag_filename, const std::string &geom_filename)
{
	mShader* frag = 0;
	mShader* vert = 0;
	mShader* geom = 0;

	if (!vert_filename.empty()) {
		vert = new mShader(vert_filename, mShader::VERTEX_SHADER);

		if (!vert->loadSourceFromFile(vert_filename)) {
			printf("Failed to load vertex shader %s\n", vert_filename.c_str());
			delete vert;
			return false;
		} else {
			vert->uploadSource();

			if (!vert->compileShader()) {
				printf("Failed to compile vertex shader %s", vert_filename.c_str());
				printf("Compiler Log: \n%s\n", vert->getCompilerLog().c_str());
				delete vert;
				return false;
			}
		}
	}

	if (!geom_filename.empty()) {
		geom = new mShader(geom_filename, mShader::GEOMETRY_SHADER);

		if (!geom->loadSourceFromFile(geom_filename)) {
			printf("Failed to load geometry shader %s\n", geom_filename.c_str());
			delete vert;
			delete geom;
			return false;
		} else {
			geom->uploadSource();
			if (!geom->compileShader()) {
				printf("Failed to compile geometry shader %s\n", geom_filename.c_str());
				printf("Compiler Log: \n%s\n", geom->getCompilerLog().c_str());
				delete vert;
				delete geom;
				return false;
			}
		}
	}

	if (!frag_filename.empty()) {
		frag = new mShader(frag_filename, mShader::FRAGMENT_SHADER);


		if (!frag->loadSourceFromFile(frag_filename)) {
			printf("Failed to load fragment shader %s\n", frag_filename.c_str());
			delete frag;
			delete geom;
			delete vert;
			return false;
		} else {
			frag->uploadSource();

			if (!frag->compileShader()) {
				printf("Failed to compile fragment shader %s\n", frag_filename.c_str());
				printf("Compiler Log: \n%s\n", frag->getCompilerLog().c_str());
				delete vert;
				delete geom;
				delete frag;
				return false;
			}
		}
	}

	// Attach Shader, dtor will take care of freeing them
	if (frag)
		attachShader(frag);
	if (vert)
		attachShader(vert);
	if (geom)
		attachShader(geom);

	if (!linkProgram()) {
		printf("Failed to link shader (%s,%s,%s)\n", vert_filename.c_str(), frag_filename.c_str(), geom_filename.c_str());
		if (vert) {
			printf("[%s] Vertex shader compiler log: \n%s\n",vert->filename.c_str(), vert->getCompilerLog().c_str());
			detachShader(vert);
			delete vert;
		}
		if (geom) {
			printf("[%s] Geometry shader compiler log: \n%s\n",geom->filename.c_str(), geom->getCompilerLog().c_str());
			detachShader(geom);
			delete geom;
		}
		if (frag) {
			printf("[%s] Fragment shader compiler log: \n%s\n",frag->filename.c_str(), frag->getCompilerLog().c_str());
			detachShader(frag);
			delete frag;
		}

		printf("Linker Log: \n%s\n", getLinkerLog().c_str());
		return false;
	}


	if (vert && vert->getCompilerLog().size() > 1) {
		printf("Vertex shader compiler log for file '%s': \n%s\n", vert_filename.c_str(), vert->getCompilerLog().c_str());
	}
	if (geom && geom->getCompilerLog().size() > 1) {
		printf("Geometry shader compiler log for file '%s': \n%s\n", geom_filename.c_str(), geom->getCompilerLog().c_str());
	}
	if (frag && frag->getCompilerLog().size() > 1) {
		printf("Fragment shader compiler log for file '%s': \n%s\n", frag_filename.c_str(), frag->getCompilerLog().c_str());
	}

	if (getLinkerLog().size() > 1) {
		printf("Linker log for '%s' and '%s' and '%s': \n%s\n", vert_filename.c_str(), frag_filename.c_str(), geom_filename.c_str(), getLinkerLog().c_str());
	}

	return true;
}

void ShaderProgram::attachShader(mShader *pShader)
{
	glAttachShader(id, pShader->id);
	shaders.push_back(pShader);
}

void ShaderProgram::detachShader(mShader *pShader)
{
	glDetachShader(id, pShader->id);
	shaders.remove(pShader);
	linked = false;
}

void ShaderProgram::detachShadersByType(mShader::ShaderType type)
{
	for (std::list<mShader*>::iterator iter = shaders.begin(); iter != shaders.end(); ++iter) {
		if ((*iter)->getType() == type)
			detachShader(*iter);
		delete (*iter);
	}
	linked = false;
}

bool ShaderProgram::linkProgram()
{
	if (linked) {
		// program is already linked: detach and re-attach everything
		for (std::list<mShader*>::iterator iter = shaders.begin(); iter != shaders.end(); ++iter) {
			glDetachShader(id, (*iter)->id);
			glAttachShader(id, (*iter)->id);
		}
	}

	linked = false;
	glLinkProgram(id);
	GLint check = 0;
	glGetProgramiv(id, GL_LINK_STATUS, &check);
	if (check)
		linked = true;

	return linked;
}

bool ShaderProgram::rebuild()
{
	if (linked) {
		// program is already linked: detach and re-attach everything
		for (std::list<mShader*>::iterator iter = shaders.begin(); iter != shaders.end(); ++iter) {
			glDetachShader(id, (*iter)->id);
			(*iter)->uploadSource();
			if (!(*iter)->compileShader()) {
				printf("Failed to compile shader object.\n");
				printf("Compiler Log: \n%s\n", (*iter)->getCompilerLog().c_str());
				return false;
			}

			glAttachShader(id, (*iter)->id);
		}
	}
	linked = false;
	glLinkProgram(id);
	GLint check = 0;
	glGetProgramiv(id, GL_LINK_STATUS, &check);

	if (check) {
		linked = true;
		return true;
	} else {
		printf("ShaderProgram::rebuild(): Failed to link shader.\n" );
		printf("Linker Log: \n%s\n", getLinkerLog().c_str());
		return false;
	}
}

bool ShaderProgram::rebuildFromFile()
{
	bool result = true;

	for (std::list<mShader*>::iterator iter = shaders.begin(); iter != shaders.end(); ++iter)
		result &= (*iter)->rebuildFromFile();

	result &= rebuild();

	return result;
}

std::string ShaderProgram::getLinkerLog() const
{
	GLint len;
	glGetProgramiv(id, GL_INFO_LOG_LENGTH , &len);

	if (len > 1) {
		GLchar* log = new GLchar[len];
		if (log == 0)
			return "Memory allocation for log failed!";
		GLsizei l;  // length returned
		glGetProgramInfoLog(id, len, &l, log);
		std::string retStr(log);
		delete[] log;
		return retStr;
	}

	return "";
}

void ShaderProgram::activate()
{
	if (linked)
		glUseProgram(id);
}

void ShaderProgram::activate(GLint id)
{
	glUseProgram(id);
}

void ShaderProgram::deactivate()
{
	glUseProgram(0);
}

GLint ShaderProgram::getCurrentProgram()
{
	GLint id;
	glGetIntegerv(GL_CURRENT_PROGRAM, &id);
	return id;
}

bool ShaderProgram::isActivated()
{
	GLint shader_nr;
	glGetIntegerv(GL_CURRENT_PROGRAM, &shader_nr);
	return (id == static_cast<GLuint>(shader_nr));
}

GLint ShaderProgram::getUniformLocation(const std::string &name)
{
	GLint l;
	l = glGetUniformLocation(id, name.c_str());
	if (l == -1 && !ignoreError)
		printf("Failed to locate uniform Location: %s\n", name.c_str());
	return l;
}

// Floats
bool ShaderProgram::setUniform(const std::string &name, GLfloat value)
{
	GLint l = getUniformLocation(name);
	if (l == -1)
		return false;
	glUniform1f(l, value);
	return true;
}

bool ShaderProgram::setUniform(const std::string &name, GLfloat v1, GLfloat v2)
{
	GLint l = getUniformLocation(name);
	if (l == -1)
		return false;
	glUniform2f(l, v1, v2);
	return true;
}

bool ShaderProgram::setUniform(const std::string &name, GLfloat v1, GLfloat v2, GLfloat v3)
{
	GLint l = getUniformLocation(name);
	if (l == -1)
		return false;
	glUniform3f(l, v1, v2, v3);
	return true;
}

bool ShaderProgram::setUniform(const std::string &name, GLfloat v1, GLfloat v2, GLfloat v3, GLfloat v4)
{
	GLint l = getUniformLocation(name);
	if (l == -1)
		return false;
	glUniform4f(l, v1, v2, v3, v4);
	return true;
}

bool ShaderProgram::setUniform(const std::string &name, GLfloat *v, int count)
{
	GLint l = getUniformLocation(name);
	if (l == -1)
		return false;
	glUniform1fv(l, count, v);
	return true;
}

// Integers
bool ShaderProgram::setUniform(const std::string &name, GLint value) 
{
	GLint l = getUniformLocation(name);
	if (l == -1)
		return false;
	glUniform1i(l, value);
	return true;
}

bool ShaderProgram::setUniform(const std::string &name, GLint v1, GLint v2) 
{
	GLint l = getUniformLocation(name);
	if (l == -1)
		return false;
	glUniform2i(l, v1, v2);
	return true;
}

bool ShaderProgram::setUniform(const std::string &name, GLint v1, GLint v2, GLint v3) 
{
	GLint l = getUniformLocation(name);
	if (l == -1)
		return false;
	glUniform3i(l, v1, v2, v3);
	return true;
}

bool ShaderProgram::setUniform(const std::string &name, GLint v1, GLint v2, GLint v3, GLint v4) 
{
	GLint l = getUniformLocation(name);
	if (l == -1)
		return false;
	glUniform4i(l, v1, v2, v3, v4);
	return true;
}

bool ShaderProgram::setUniform(const std::string &name, GLint* v, int count) 
{
	GLint l = getUniformLocation(name);
	if (l == -1)
		return false;
	glUniform1iv(l, count, v);
	return true;
}


bool ShaderProgram::setUniform(const std::string &name, bool value) 
{
	GLint l = getUniformLocation(name);
	if (l == -1)
		return false;
	setUniform(l, static_cast<GLint>(value));
	return true;
}

bool ShaderProgram::setUniform(const std::string &name, bool v1, bool v2) 
{
	GLint l = getUniformLocation(name);
	if (l == -1)
		return false;
	setUniform(l, static_cast<GLint>(v1), static_cast<GLint>(v2));
	return true;
}

bool ShaderProgram::setUniform(const std::string &name, bool v1, bool v2, bool v3) 
{
	GLint l = getUniformLocation(name);
	if (l == -1)
		return false;
	setUniform(l, static_cast<GLint>(v1), static_cast<GLint>(v2), static_cast<GLint>(v3));
	return true;
}

bool ShaderProgram::setUniform(const std::string &name, bool v1, bool v2, bool v3, bool v4)
{
	GLint l = getUniformLocation(name);
	if (l == -1)
		return false;
	setUniform(l, static_cast<GLint>(v1), static_cast<GLint>(v2), static_cast<GLint>(v3), static_cast<GLint>(v4));
	return true;
}

bool ShaderProgram::setUniform(const std::string &name, GLboolean* v, int count)
{
	GLint l = getUniformLocation(name);
	if (l == -1)
		return false;
	GLint* vector = new GLint[count];
	for (int i=0; i < count; i++)
		vector[i] = static_cast<GLint>( v[i] );
	glUniform1iv(l, count, vector);
	delete[] vector;
	return true;
}

// Vectors
bool ShaderProgram::setUniform(const std::string &name, const Vector2f &value)
{
	GLint l = getUniformLocation(name);
	if (l == -1)
		return false;
	setUniform(l, value);
	return true;
}

bool ShaderProgram::setUniform(const std::string &name, Vector2f *vectors, GLsizei count) 
{
	GLint l = getUniformLocation(name);
	if (l == -1)
		return false;
	GLfloat* values = new GLfloat[2*count];
	for (int i=0; i < count; i++){
		values[2*i] = vectors[i].x;
		values[2*i+1] = vectors[i].y;
	}
	glUniform2fv(l, count, values);
	delete[] values;
	return true;
}

bool ShaderProgram::setUniform(const std::string &name, const Vector3f &value)
{
	GLint l = getUniformLocation(name);
	if (l == -1)
		return false;
	setUniform(l, value);
	return true;
}

bool ShaderProgram::setUniform(const std::string &name, Vector3f *vectors, GLsizei count)
{
	GLint l = getUniformLocation(name);
	if (l == -1)
		return false;
	GLfloat* values = new GLfloat[3*count];
	for (int i=0; i < count; i++) {
		values[3*i] = vectors[i].x;
		values[3*i+1] = vectors[i].y;
		values[3*i+2] = vectors[i].z;
	}
	glUniform3fv(l, count, values);
	delete[] values;
	return true;
}

bool ShaderProgram::setUniform(const std::string &name, const Vector4f &value)
{
	GLint l = getUniformLocation(name);
	if (l == -1)
		return false;
	setUniform(l, value);
	return true;
}

bool ShaderProgram::setUniform(const std::string &name, Vector4f *vectors, GLsizei count)
{
	GLint l = getUniformLocation(name);
	if (l == -1)
		return false;
	GLfloat* values = new GLfloat[4*count];
	for (int i=0; i < count; i++) {
		values[4*i] = vectors[i].x;
		values[4*i+1] = vectors[i].y;
		values[4*i+2] = vectors[i].z;
		values[4*i+3] = vectors[i].w;
	}
	glUniform4fv(l, count, values);
	delete[] values;
	return true;
}

bool ShaderProgram::setUniform(const std::string &name, const Vector2i &value) 
{
	GLint l = getUniformLocation(name);
	if (l == -1)
		return false;
	setUniform(l, value);
	return true;
}

bool ShaderProgram::setUniform(const std::string &name, Vector2i *vectors, GLsizei count) 
{
	GLint l = getUniformLocation(name);
	if (l == -1)
		return false;
	GLint* values = new GLint[2*count];
	for (int i=0; i < count; i++) {
		values[2*i] = vectors[i].x;
		values[2*i+1] = vectors[i].y;
	}
	glUniform2iv(l, count, values);
	delete[] values;
	return true;
}

bool ShaderProgram::setUniform(const std::string &name, const Vector3i &value)
{
	GLint l = getUniformLocation(name);
	if (l == -1)
		return false;
	setUniform(l, value);
	return true;
}

bool ShaderProgram::setUniform(const std::string &name, Vector3i *vectors, GLsizei count)
{
	GLint l = getUniformLocation(name);
	if (l == -1)
		return false;
	GLint* values = new GLint[3*count];
	for (int i=0; i < count; i++) {
		values[3*i] = vectors[i].x;
		values[3*i+1] = vectors[i].y;
		values[3*i+2] = vectors[i].z;
	}
	glUniform3iv(l, count, values);
	delete[] values;
	return true;
}

bool ShaderProgram::setUniform(const std::string &name, const Vector4i &value)
{
	GLint l = getUniformLocation(name);
	if (l == -1)
		return false;
	setUniform(l, value);    
	return true;
}

bool ShaderProgram::setUniform(const std::string &name, Vector4i *vectors, GLsizei count)
{
	GLint l = getUniformLocation(name);
	if (l == -1)
		return false;
	GLint* values = new GLint[4*count];
	for (int i=0; i < count; i++) {
		values[4*i] = vectors[i].x;
		values[4*i+1] = vectors[i].y;
		values[4*i+2] = vectors[i].z;
		values[4*i+3] = vectors[i].w;
	}
	glUniform4iv(l, count, values);
	delete[] values;
	return true;
}

// Note: Matrix is transposed by OpenGL
bool ShaderProgram::setUniform(const std::string &name, const Matrix2f &value, bool transpose)
{
	GLint l = getUniformLocation(name);
	if (l == -1)
		return false;
	setUniform(l, value, transpose);
	return true;
}

bool ShaderProgram::setUniform(const std::string &name, const Matrix3f &value, bool transpose)
{
	GLint l = getUniformLocation(name);
	if (l == -1)
		return false;
	setUniform(l, value, transpose);
	return true;
}

bool ShaderProgram::setUniform(const std::string &name, const Matrix4f &value, bool transpose)
{
	GLint l = getUniformLocation(name);
	if (l == -1)
		return false;
	setUniform(l, value, transpose);
	return true;
}

// No location lookup
//
// Floats
void ShaderProgram::setUniform(GLint l, GLfloat value)
{
	glUniform1f(l, value);
}
	
void ShaderProgram::setUniform(GLint l, GLfloat v1, GLfloat v2)
{
	glUniform2f(l, v1, v2);
}

void ShaderProgram::setUniform(GLint l, GLfloat v1, GLfloat v2, GLfloat v3)
{
	glUniform3f(l, v1, v2, v3);
}

void ShaderProgram::setUniform(GLint l, GLfloat v1, GLfloat v2, GLfloat v3, GLfloat v4)
{
	glUniform4f(l, v1, v2, v3, v4);
}

// Integers
void ShaderProgram::setUniform(GLint l, GLint value)
{
	glUniform1i(l, value);
}

void ShaderProgram::setUniform(GLint l, GLint v1, GLint v2)
{
	glUniform2i(l, v1, v2);
}

void ShaderProgram::setUniform(GLint l, GLint v1, GLint v2, GLint v3)
{
	glUniform3i(l, v1, v2, v3);
}

void ShaderProgram::setUniform(GLint l, GLint v1, GLint v2, GLint v3, GLint v4)
{
	glUniform4i(l, v1, v2, v3, v4);
}

// Vectors
void ShaderProgram::setUniform(GLint l, const Vector2f &value)
{
	glUniform2f(l, value.x, value.y);
}

void ShaderProgram::setUniform(GLint l, const Vector3f &value)
{
	glUniform3f(l, value.x, value.y, value.z);
}

void ShaderProgram::setUniform(GLint l, const Vector4f &value)
{
	glUniform4f(l, value.x, value.y, value.z, value.w);
}

void ShaderProgram::setUniform(GLint l, const Vector2i &value)
{
	glUniform2i(l, static_cast<GLint>(value.x), static_cast<GLint>(value.y));
}

void ShaderProgram::setUniform(GLint l, const Vector3i &value)
{
	glUniform3i(l, static_cast<GLint>(value.x), static_cast<GLint>(value.y), static_cast<GLint>(value.z));
}

void ShaderProgram::setUniform(GLint l, const Vector4i &value) 
{
	glUniform4i(l, static_cast<GLint>(value.x), static_cast<GLint>(value.y),
				static_cast<GLint>(value.z), static_cast<GLint>(value.w));
}

void ShaderProgram::setUniform(GLint l, const Matrix2f &value, bool transpose)
{
	glUniformMatrix2fv(l, 1, !transpose, value.m[0]);
}

void ShaderProgram::setUniform(GLint l, const Matrix3f &value, bool transpose)
{
	glUniformMatrix3fv(l, 1, !transpose, value.m[0]);
}

void ShaderProgram::setUniform(GLint l, const Matrix4f &value, bool transpose)
{
	glUniformMatrix4fv(l, 1, !transpose, value.m[0]);
}

// Attributes
//
// 1 component
void ShaderProgram::setAttribute(GLint index, GLfloat v1)
{
	glVertexAttrib1f(index, v1);
}

void ShaderProgram::setAttribute(GLint index, GLshort v1)
{
	glVertexAttrib1s(index, v1);
}

void ShaderProgram::setAttribute(GLint index, GLdouble v1)
{
	glVertexAttrib1d(index, v1);
}

// 2 components
void ShaderProgram::setAttribute(GLint index, GLfloat v1, GLfloat v2)
{
	glVertexAttrib2f(index, v1, v2);
}

void ShaderProgram::setAttribute(GLint index, GLshort v1, GLshort v2)
{
	glVertexAttrib2s(index, v1, v2);
}

void ShaderProgram::setAttribute(GLint index, GLdouble v1, GLdouble v2)
{
	glVertexAttrib2d(index, v1, v2);
}

// 3 components
void ShaderProgram::setAttribute(GLint index, GLfloat v1, GLfloat v2, GLfloat v3)
{
	glVertexAttrib3f(index, v1, v2, v3);
}

void ShaderProgram::setAttribute(GLint index, GLshort v1, GLshort v2, GLshort v3) 
{
	glVertexAttrib3s(index, v1, v2, v3);
}

void ShaderProgram::setAttribute(GLint index, GLdouble v1, GLdouble v2, GLdouble v3) 
{
	glVertexAttrib3d(index, v1, v2, v3);
}

// 4 components
void ShaderProgram::setAttribute(GLint index, GLfloat v1, GLfloat v2, GLfloat v3, GLfloat v4)
{
	glVertexAttrib4f(index, v1, v2, v3, v4);
}

void ShaderProgram::setAttribute(GLint index, GLshort v1, GLshort v2, GLshort v3, GLshort v4)
{
	glVertexAttrib4s(index, v1, v2, v3, v4);
}

void ShaderProgram::setAttribute(GLint index, GLdouble v1, GLdouble v2, GLdouble v3, GLdouble v4)
{
	glVertexAttrib4d(index, v1, v2, v3, v4);
}

// For vectors
void ShaderProgram::setAttribute(GLint index, const Vector2f &v) 
{
	glVertexAttrib2fv(index, &(v.x));
}

void ShaderProgram::setAttribute(GLint index, const Vector3f &v) 
{
	glVertexAttrib3fv(index, &(v.x));
}

void ShaderProgram::setAttribute(GLint index, const Vector4f &v)
{
	glVertexAttrib4fv(index, &(v.x));
}

void ShaderProgram::setAttribute(GLint index, const Vector2d &v)
{
	glVertexAttrib2dv(index, &(v.x));
}

void ShaderProgram::setAttribute(GLint index, const Vector3d &v)
{
	glVertexAttrib3dv(index, &(v.x));
}

void ShaderProgram::setAttribute(GLint index, const Vector4d &v) 
{
	glVertexAttrib4dv(index, &(v.x));
}

void ShaderProgram::setAttribute(GLint index, const Vector2<GLshort> &v)
{
	glVertexAttrib2sv(index, &(v.x));
}

void ShaderProgram::setAttribute(GLint index, const Vector3<GLshort> &v)
{
	glVertexAttrib3sv(index, &(v.x));
}

void ShaderProgram::setAttribute(GLint index, const Vector4<GLshort> &v)
{
	glVertexAttrib4sv(index, &(v.x));
}

void ShaderProgram::setAttribute(GLint index, const Vector4<GLint> &v)
{
	glVertexAttrib4iv(index, &(v.x));
}

void ShaderProgram::setAttribute(GLint index, const Vector4<GLbyte> &v) 
{
	glVertexAttrib4bv(index, &(v.x));
}

void ShaderProgram::setAttribute(GLint index, const Vector4<GLubyte> &v) 
{
	glVertexAttrib4ubv(index, &(v.x));
}

void ShaderProgram::setAttribute(GLint index, const Vector4<GLushort> &v)
{
	glVertexAttrib4usv(index, &(v.x));
}

void ShaderProgram::setAttribute(GLint index, const Vector4<GLuint> &v)
{
	glVertexAttrib4uiv(index, &(v.x));
}

// Attribute locations
void ShaderProgram::setAttributeLocation(GLuint index, const std::string &name)
{
	glBindAttribLocation(id, index, name.c_str());
}

GLint ShaderProgram::getAttributeLocation(const std::string &name) 
{
	GLint l;
	l = glGetAttribLocation(id, name.c_str());
	if (l == -1)
		printf("Failed to locate attribute Location: %s\n", name.c_str());
	return l;
}

// Normalized attributes
void ShaderProgram::setNormalizedAttribute(GLint index, GLubyte v1, GLubyte v2, GLubyte v3, GLubyte v4) 
{
	glVertexAttrib4Nub(index, v1, v2, v3, v4);
}

void ShaderProgram::setNormalizedAttribute(GLint index, const Vector4<GLbyte> &v)
{
	glVertexAttrib4Nbv(index, &(v.x));
}

void ShaderProgram::setNormalizedAttribute(GLint index, const Vector4<GLshort> &v) 
{
	glVertexAttrib4Nsv(index, &(v.x));
}

void ShaderProgram::setNormalizedAttribute(GLint index, const Vector4<GLint> &v) 
{
	glVertexAttrib4Niv(index, &(v.x));
}

// Unsigned version
void ShaderProgram::setNormalizedAttribute(GLint index, const Vector4<GLubyte> &v)
{
	glVertexAttrib4Nubv(index, &(v.x));
}

void ShaderProgram::setNormalizedAttribute(GLint index, const Vector4<GLushort> &v) 
{
	glVertexAttrib4Nusv(index, &(v.x));
}

void ShaderProgram::setNormalizedAttribute(GLint index, const Vector4<GLuint> &v) 
{
	glVertexAttrib4Nuiv(index, &(v.x));
}


bool ShaderProgram::setUniformSampler(const std::string &name, GLenum texUnit, GLenum target, GLuint texID)
{
	GLint l = getUniformLocation(name);
	if (l == -1)
		return false;
	glActiveTexture(texUnit);		//使用一个纹理单元：或者GL_TEXTURE0+2(最多31)
	glBindTexture(target, texID);	//给这个纹理单元绑定一个纹理
	glUniform1i(l, texUnit - GL_TEXTURE0); //将纹理单元的序号传递给glsl
	return true;
}