#include <sstream>
#include "volume/Rendering/Shader.h"

mShader::mShader(const std::string& filename, ShaderType type)
	: filename(filename),
	  shaderType(type),
	  compiled(false),
	  verticesOut(16)
{
	id = glCreateShader(shaderType);
	if (id == 0)
		printf("Shader(%s)::glCreateShader() returned 0\n", filename.c_str());
}

mShader::~mShader()
{
	glDeleteShader(id);
}

bool mShader::loadSourceFromFile(const std::string &filename)
{
	printf("---------------------------------\n");
	printf("Loading %s\n", filename.c_str());

	char* tmp = loadShader(filename.c_str());
	if(tmp == NULL)
		return false;

	this->filename = filename;
	source = tmp;
	free(tmp);
	return true;
}

bool mShader::compileShader()
{
	compiled = false;

	uploadSource();

	glCompileShader(id);
	GLint check = 0;
	glGetShaderiv(id, GL_COMPILE_STATUS, &check);
	compiled = (check == GL_TRUE);
	return compiled;
}

bool mShader::rebuildFromFile()
{
	if (!loadSourceFromFile(filename)) {
		printf("Failed to load shader %s\n", filename.c_str());
		return false;
	}

	uploadSource();

	if (!compileShader()) {
		printf("Failed to compile shader object  %s\n", filename.c_str());
		printf("Compiler Log: \n%s\n", getCompilerLog().c_str());
		return false;
	}

	return true;
}

std::string mShader::getCompilerLog() const
{
	GLint len;
	glGetShaderiv(id, GL_INFO_LOG_LENGTH , &len);

	if (len > 1) {
		GLchar* log = new GLchar[len];
		if (log == 0)
			return "Memory allocation for log failed!";
		GLsizei l;  // length returned
		glGetShaderInfoLog(id, len, &l, log);
		std::istringstream str(log);
		delete[] log;

		std::ostringstream result;

		std::string line;
		while (getline(str, line)) {
			result << line;
			result << '\n';
		}

		return result.str();
	} else {
		return "";
	}
}

void mShader::uploadSource()
{
	const GLchar* s = source.c_str();
	glShaderSource(id, 1,  &s, 0);
}

char* mShader::loadShader( const char *filename )
{
	FILE *fp;
	char *src;
	long length;

	fp = fopen(filename, "rb");
	if (fp == NULL) {
		printf("Cannot open file [%s]\n", filename);
		return NULL;
	}

	fseek(fp, 0L, SEEK_END);
	length = ftell(fp);
	if (length <= 0)
	{
		printf("Empty shader file [%s]\n", filename);
		return NULL;
	}

	src = (char *)malloc(length + 1);
	if (src == NULL) {
		fprintf(stderr, "Could not allocate read buffer.\n");
		return NULL;
	}

	rewind(fp);

	if (fread((void *)src, 1, length, fp) != (size_t)length )
	{
		printf("Could not read file: %s.\n", filename);
		free((void*)src);
		src = NULL;
	}
	else src[length] = '\0';

	printf("Shader [%s] loaded ...\n", filename);
	fclose(fp);
	return src;
}
