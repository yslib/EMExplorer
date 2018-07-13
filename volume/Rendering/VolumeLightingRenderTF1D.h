#include "Rendering/ShaderProgram.h"
#include "ShaderData.h"

class VolumeLightingRenderTF1D : public ShaderProgram
{
public:
	VolumeLightingRenderTF1D() {}
	~VolumeLightingRenderTF1D() {}

	static string shaderName() { return "TF1DLightingShader"; }

	virtual bool initShader()
	{
		ShaderProgram::loadShader("", "Shaders/raycast_lighting.frag");
		return true;
	}

	virtual void loadShader(ShaderData& parameters)
	{
		this->activate();
		Vector3d L = parameters.getLightDirection();
		Vector3d H = L - parameters.getCameraTowards();
		if(H.norm() > 1e-10) H = H.normalize();
		this->setUniformSampler("texVolume", GL_TEXTURE3, GL_TEXTURE_3D, parameters.getVolumeTexIdx());
		this->setUniformSampler("texStartPos", GL_TEXTURE0, GL_TEXTURE_RECTANGLE_ARB, parameters.getStartPosTexIdx());
		this->setUniformSampler("texEndPos", GL_TEXTURE1, GL_TEXTURE_RECTANGLE_ARB, parameters.getEndPosTexIdx());
		this->setUniformSampler("texTransfunc", GL_TEXTURE2, GL_TEXTURE_1D, parameters.getTF1DIdx());
		this->setUniform("step", parameters.getRayStep());
		this->setUniform("ka", parameters.getAmbient());
		this->setUniform("ks", parameters.getSpecular());
		this->setUniform("kd", parameters.getDiffuse());
		this->setUniform("shininess", parameters.getShininess());
		this->setUniform("lightdir",  (float)L.x, (float)L.y, (float)L.z);
		this->setUniform("halfway", (float)H.x, (float)H.y, (float)H.z);
	}

	virtual void loadShader(ShaderData& parameters, Vector3d& L, Vector3d& V, unsigned int startTexIdx, unsigned int endTexIdx) {
		this->activate();
		Vector3d H = L + V;
		if(H.norm() > 1e-10) H = H.normalize();
		this->setUniformSampler("texVolume", GL_TEXTURE3, GL_TEXTURE_3D, parameters.getVolumeTexIdx());
		this->setUniformSampler("texStartPos", GL_TEXTURE0, GL_TEXTURE_RECTANGLE_ARB, startTexIdx);
		this->setUniformSampler("texEndPos", GL_TEXTURE1, GL_TEXTURE_RECTANGLE_ARB, endTexIdx);
		this->setUniformSampler("texTransfunc", GL_TEXTURE2, GL_TEXTURE_1D, parameters.getTF1DIdx());
		this->setUniform("step", parameters.getRayStep());
		this->setUniform("ka", parameters.getAmbient());
		this->setUniform("ks", parameters.getSpecular());
		this->setUniform("kd", parameters.getDiffuse());
		this->setUniform("shininess", parameters.getShininess());
		this->setUniform("lightdir",  (float)L.x, (float)L.y, (float)L.z);
		this->setUniform("halfway", (float)H.x, (float)H.y, (float)H.z);

	}
	virtual void unloadShader() {
		this->deactivate();
	}

	// is TF 1D needed
	virtual bool requireTF1D()     { return true; };

	// is Lighting needed
	virtual bool requireLighting() { return true; };

};