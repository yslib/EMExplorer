#include "Rendering/ShaderProgram.h"
#include "ShaderData.h"

class MIPRender : public ShaderProgram
{
public:
	MIPRender() {}
	~MIPRender() {}

	static string shaderName() { return "MIPShader"; }

	virtual bool initShader()
	{
		ShaderProgram::loadShader("", "Shaders/mip.frag");
		return true;
	}

	virtual void loadShader(ShaderData& parameters)
	{
		this->activate();
		this->setUniformSampler("texVolume", GL_TEXTURE3, GL_TEXTURE_3D, parameters.getVolumeTexIdx());
		this->setUniformSampler("texStartPos", GL_TEXTURE0, GL_TEXTURE_RECTANGLE_ARB, parameters.getStartPosTexIdx());
		this->setUniformSampler("texEndPos", GL_TEXTURE1, GL_TEXTURE_RECTANGLE_ARB, parameters.getEndPosTexIdx());
		this->setUniform("step", parameters.getRayStep());
	}

	virtual void loadShader(ShaderData& parameters, Vector3d& L, Vector3d& V, unsigned int startTexIdx, unsigned int endTexIdx) {
		this->activate();
		this->setUniformSampler("texVolume", GL_TEXTURE3, GL_TEXTURE_3D, parameters.getVolumeTexIdx());
		this->setUniformSampler("texStartPos", GL_TEXTURE0, GL_TEXTURE_RECTANGLE_ARB, startTexIdx);
		this->setUniformSampler("texEndPos", GL_TEXTURE1, GL_TEXTURE_RECTANGLE_ARB, endTexIdx);
		this->setUniform("step", parameters.getRayStep());

	}
	virtual void unloadShader() {
		this->deactivate();
	}
};