#ifndef VOLUMERENDERWIDGET_H
#define VOLUMERENDERWIDGET_H

#include <vector>
#include <QGLWidget>
#include "Rendering/Vector.h"
#include "ShaderData.h"

using namespace std;

class ModelData;
class FramebufferObject;
class ShaderProgram;
enum ShaderType {TF1DLIGHTING, TF2DLIGHTING};

// Viewpoints
struct Viewpoint {
	double theta;
	double phi;
	double value;	// vote
	Viewpoint(double t, double p) : theta(t), phi(p), value(0) {};
	Viewpoint(double t, double p, double v) : theta(t), phi(p), value(v) {};
};

struct Triangle {
	int idxA;
	int idxB;
	int idxC;
	Triangle(int a, int b, int c) : idxA(a), idxB(b), idxC(c) {};
	bool operator = (const Triangle& t) {
		return (idxA == t.idxA && idxB == t.idxB && idxC == t.idxC) ||
			(idxB == t.idxA && idxC == t.idxB && idxA == t.idxC) ||
			(idxC == t.idxA && idxA == t.idxB && idxB == t.idxC);
	}
};

struct ViewingSphere {
	double maxValue;
	vector<size_t> orders;
	vector<Viewpoint> viewpoints;
	vector< pair<int, int> > links;
	vector<Triangle> triangles;
};

class VolumeRenderWidget : public QGLWidget, public ShaderData
{
    Q_OBJECT

public:
    VolumeRenderWidget(ModelData *modelData, QWidget *parent = 0);
    ~VolumeRenderWidget();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

public slots:
	void volumeChanged();
	void TF1DChanged();
	void TF2DChanged();
	void toggleInteractionMode(bool on);
	void generateViewpointImages();
	void generateViewpointImages2();
	void setMIPShader();
	void setTF1DShader();
	void setTF1DLightingShader();
	void setTF2DLightingShader();
	void defaultView();
	void positiveXView();
	void negativeXView();
	void positiveYView();
	void negativeYView();
	void positiveZView();
	void negativeZView();
	void setAmbient(double value);
	void setDiffuse(double value);
	void setShininess(double value);
	void setSpecular(double value);
	void setBoundingBox();
	void setHealpixNside(double value);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
	bool initializeShader();
	void initializeVP();
	void prepareVolumeTexture();
	void prepareTFTexture();
	void projectionMatrix();
	void modelMatrix();
	void loadUniformColor(double value, double alpha = 1.0);
	void drawCube();
	void drawSolidCube();

	void generateViewingSphere();
	void clearViewingSphere();
	QString generateDirectory();
	void drawViewpoints();
	void drawViewingSphereEdges();

// ShaderData Interface
public:
	// volume texture idx
	virtual unsigned int getVolumeTexIdx()             { return volumeTextureIdx;      }
	virtual unsigned int getMagnitudeTexIdx()          { return gradientTextureIdx;	   }

	// voxel size
	virtual Vector3d     getVoxelSize()                { return voxelSize;             }

	// ray casting start and end position texture idx
	virtual unsigned int getStartPosTexIdx()           { return startPosTextureIdx;    }
	virtual unsigned int getEndPosTexIdx()             { return endPosTextureIdx;      }

	// ray casting step
	virtual float        getRayStep()                  { return (float)rayCastingStep * interactionCoarseness; }

	// transfer function idx
	virtual unsigned int getTF1DIdx()                  { return TF1DTextureIdx;        }
	virtual unsigned int getTF2DIdx()				   { return TF2DTextureIdx;		   }

	// camera parameters
	virtual Vector3d     getCameraEye()                { return cameraEye;     }
	virtual Vector3d     getCameraTowards()            { return cameraTowards; }
	virtual Vector3d     getCameraUp()                 { return cameraUp;      }
	virtual Vector3d     getCameraRight()              { return cameraRight;   }

	// lighting parameters
	virtual Vector3d     getLightDirection()           { return -cameraTowards;}
	virtual float        getAmbient()                  { return ambient;       }
	virtual float        getDiffuse()                  { return diffuse;       }
	virtual float        getShininess()                { return shininess;     }
	virtual float        getSpecular()                 { return specular;      }

	// mouse position
	virtual unsigned int getMouseX()                   { return mousePosX;     }
	virtual unsigned int getMouseY()                   { return mousePosY;     }

	// volume boundary
	virtual Vector3d     getVolumeBound()              { return Vector3d(xScale, yScale, zScale); }

private:
	// widget information
	int      widgetWidth;
	int      widgetHeight;

	// bounding box center, scale, and camera information
	Vector3d bbCenter;
	double   bbRadius;
	Vector3d cameraEye;
	Vector3d cameraTowards;
	Vector3d cameraUp;
	Vector3d cameraRight;
	double   zoom;

	// camera interaction
	int      mousePosX;
	int      mousePosY;
	QPoint   lastMousePos;

	// rendering parameter
	bool     showBoundingBox;

	// Model data
	ModelData *modelData;

	// volume information
	int      xVolume, yVolume, zVolume;
	double   xCoord,  yCoord,  zCoord;
	double   xScale,  yScale,  zScale;
	double   m_xcoord, m_ycoord, m_zcoord;
	Vector3d translation;

	// lighting parameters
	float    ambient;
	float    diffuse;
	float    shininess;
	float    specular;

	// Textures
	bool         TF1DReload;
	bool         TF2DReload;
	bool         volumeReload;
	unsigned int TF1DTextureIdx;
	unsigned int TF2DTextureIdx;
	unsigned int volumeTextureIdx;
	unsigned int gradientTextureIdx;

	/*
	 *	Ray Casting in GPU
	 */

	// FBO Object
	FramebufferObject* FBO;
	// start position and end position texture idx
	unsigned int startPosTextureIdx;
	unsigned int endPosTextureIdx;
	unsigned int depthTextureIdx;
	// ray casting step
	double rayCastingStep;
	int interactionCoarseness;
	// voxel size
	Vector3d voxelSize;

	/*
	 * Viewpoint Selection
	 */
	// windows size
	unsigned int vpWidth, vpHeight;
	// FBO Object
	FramebufferObject* vpFBO;
	// start position
	unsigned int vpStartPosTexIdx;
	unsigned int vpEndPosTexIdx;
	unsigned int vpResultTexIdx;
	unsigned int vpDepthTexIdx;

	// Shader
	map<string, ShaderProgram*> shaders;
	ShaderProgram* currentShader;
	ShaderProgram* positionShader;

	bool   recalculated;
	int    heapixNside;
	double minThetaRange;
	double maxThetaRange;
	double minPhiRange;
	double maxPhiRange;
	ViewingSphere viewingSphere;
	ShaderType shaderType;
};

#endif
