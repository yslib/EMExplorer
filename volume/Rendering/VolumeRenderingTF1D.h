#include "Rendering/ShaderProgram.h"
#include "ShaderData.h"

class VolumeRenderTF1D : public ShaderProgram
{
public:
	VolumeRenderTF1D() {}
	~VolumeRenderTF1D() {}

	static string shaderName() { return "TF1DShader"; }

	virtual bool initShader()
	{
		ShaderProgram::loadShader("", "Shaders/raycast.frag");
		return true;
	}

	virtual void loadShader(ShaderData& parameters)
	{
		this->activate();
		this->setUniformSampler("texVolume", GL_TEXTURE3, GL_TEXTURE_3D, parameters.getVolumeTexIdx());
		this->setUniformSampler("texStartPos", GL_TEXTURE0, GL_TEXTURE_RECTANGLE_ARB, parameters.getStartPosTexIdx());
		this->setUniformSampler("texEndPos", GL_TEXTURE1, GL_TEXTURE_RECTANGLE_ARB, parameters.getEndPosTexIdx());
		this->setUniformSampler("texTransfunc", GL_TEXTURE2, GL_TEXTURE_1D, parameters.getTF1DIdx());
		this->setUniform("step", parameters.getRayStep());
	}

	virtual void loadShader(ShaderData& parameters, Vector3d& L, Vector3d& V, unsigned int startTexIdx, unsigned int endTexIdx) {
		this->activate();
		this->setUniformSampler("texVolume", GL_TEXTURE3, GL_TEXTURE_3D, parameters.getVolumeTexIdx());
		this->setUniformSampler("texStartPos", GL_TEXTURE0, GL_TEXTURE_RECTANGLE_ARB, startTexIdx);
		this->setUniformSampler("texEndPos", GL_TEXTURE1, GL_TEXTURE_RECTANGLE_ARB, endTexIdx);
		this->setUniformSampler("texTransfunc", GL_TEXTURE2, GL_TEXTURE_1D, parameters.getTF1DIdx());
		this->setUniform("step", parameters.getRayStep());

	}
	virtual void unloadShader() {
		this->deactivate();
	}

	// is TF 1D needed
	virtual bool requireTF1D()     { return true; };

};