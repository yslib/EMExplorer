#include <cmath>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <limits>
#include <list>
#include <set>
//#include "GL/glew.h"
#include <GL/glew.h>
#include <QtGui>
#include <QColor>
#include <QMessageBox>
#include "VolumeRenderWidget.h"
#include "ModelData.h"
#include "Volume.h"
//#include "Image.h"
#include "BMPWriter.h"
//#include "imdebuggl.h"
#include "framebufferObject.h"
#include "Rendering/Shader.h"
#include "Rendering/ShaderProgram.h"
#include "Rendering/MIPRender.h"
#include "Rendering/VolumeRenderingTF1D.h"
#include "Rendering/VolumeLightingRenderTF1D.h"
#include "Rendering/VolumeLightingRenderingTF2D.h"
#include "Healpix/arr.h"
#include "Healpix/healpix_base.h"
#include <QtWidgets/qwidget.h>

static void glErrorCheck() 
{
	GLenum errCode;
	const GLubyte *errString;
	if ((errCode = glGetError()) != GL_NO_ERROR) {
		errString = gluErrorString(errCode);
		cout<<"OpenGL Error: "<<errString<<endl;
	}
}

VolumeRenderWidget::VolumeRenderWidget(ModelData *modelData, QWidget *parent)
    : QGLWidget(parent), FBO(NULL), TF1DTextureIdx(-1), vpFBO(NULL)
{
	this->modelData = modelData;

	bbCenter      = Vector3d(0.5, 0.5, 0.5);
	bbRadius      = sqrt(3.0) * 0.5;
	cameraTowards = Vector3d(0, 0, -1);
	cameraUp      = Vector3d(0, 1, 0);
	cameraRight   = Vector3d(1, 0, 0);
	cameraEye     = bbCenter + cameraTowards * 2.5 * bbRadius;
	zoom          = 1.8;
	mousePosX     = -100;
	mousePosY     = -100;

	showBoundingBox = true;

	xCoord = yCoord = zCoord = 1.0;
	xScale = yScale = zScale = 1.0;
	translation = Vector3d(-xCoord * 0.5, -yCoord * 0.5, -zCoord * 0.5);

	ambient   = 1.0; 
	diffuse   = 0.5;
	shininess = 40; 
	specular  = 0.75;

	TF1DReload   = true;
	TF2DReload   = true;
	volumeReload = false; 

	rayCastingStep = 1;
	interactionCoarseness = 1;

	vpWidth  = 512;
	vpHeight = 512;

	recalculated  = false;
	heapixNside   = 5;
	minThetaRange = 0;
	minPhiRange   = 0;
	shaderType    = TF1DLIGHTING;

	//generateViewingSphere();

	srand((unsigned)time(0));
}

VolumeRenderWidget::~VolumeRenderWidget()
{
	for(map<string, ShaderProgram*>::iterator iter = shaders.begin(); iter != shaders.end(); ++iter)
		delete iter->second;
	delete positionShader;

	delete FBO;
	delete vpFBO;
}

QSize VolumeRenderWidget::minimumSizeHint() const
{
    return QSize(512, 512);
}

QSize VolumeRenderWidget::sizeHint() const
{
    return QSize(800, 600);
}

void VolumeRenderWidget::volumeChanged()
{
	volumeReload = true;
	updateGL();
}

void VolumeRenderWidget::TF1DChanged()
{
	if(!TF1DReload && currentShader->requireTF1D())
		updateGL();
	TF1DReload = true;
}

void VolumeRenderWidget::TF2DChanged()
{
	if(!TF2DReload && currentShader->requireTF2D())
		updateGL();
	TF2DReload = true;
}

void VolumeRenderWidget::toggleInteractionMode(bool on)
{
	if(on)
		interactionCoarseness = 3;
	else
		interactionCoarseness = 1;
	if(currentShader->requireTF1D())
		TF1DReload = true;
	if(currentShader->requireTF2D())
		TF2DReload = true;
	updateGL();
}

/*
 * VolumeRenderWidget slots
 */
void VolumeRenderWidget::setMIPShader()
{
	currentShader = shaders[MIPRender::shaderName()];
    glClearColor(0.0, 0.0, 0.0, 1.0);
	//updateGL();
	updateGL();
}

void VolumeRenderWidget::setTF1DShader()
{
	shaderType = TF1DLIGHTING;
	currentShader = shaders[VolumeRenderTF1D::shaderName()];
    glClearColor(1.0, 1.0, 1.0, 1.0);
	//updateGL();
	updateGL();
}

void VolumeRenderWidget::setTF1DLightingShader()
{
	shaderType = TF1DLIGHTING;
	currentShader = shaders[VolumeLightingRenderTF1D::shaderName()];
    glClearColor(1.0, 1.0, 1.0, 1.0);
	//updateGL();
	updateGL();
}

void VolumeRenderWidget::setTF2DLightingShader()
{
	shaderType = TF2DLIGHTING;
	currentShader = shaders[VolumeLightingRenderTF2D::shaderName()];
    glClearColor(1.0, 1.0, 1.0, 1.0);
	//updateGL();
	updateGL();
}

void VolumeRenderWidget::defaultView()
{
	Vector3d coord(xCoord, yCoord, zCoord);
	bbCenter      = coord * 0.5;
	bbRadius      = coord.norm() * 0.5;
	zoom          = 1.8;
	translation   = Vector3d(-xCoord * 0.5, -yCoord * 0.5, -zCoord * 0.5);
	cameraTowards = Vector3d(0, 0, -1);
	cameraUp      = Vector3d(0, 1, 0);
	cameraRight   = Cross(cameraTowards, cameraUp);
	cameraEye     = bbCenter + cameraTowards * 2.5 * bbRadius;
	updateGL();
}

void VolumeRenderWidget::positiveXView()
{
	Vector3d oldCameraEye = bbCenter + cameraTowards * 2.5 * bbRadius;
	double dist = Dot((cameraEye - oldCameraEye), cameraTowards);
	cameraTowards = Vector3d(1, 0, 0);
	cameraUp      = Vector3d(0, 0, 1);
	cameraRight   = Cross(cameraTowards, cameraUp);
	cameraEye     = bbCenter + cameraTowards * (2.5 * bbRadius + dist);
	updateGL();
}

void VolumeRenderWidget::negativeXView()
{
	Vector3d oldCameraEye = bbCenter + cameraTowards * 2.5 * bbRadius;
	double dist = Dot((cameraEye - oldCameraEye), cameraTowards);
	cameraTowards = Vector3d(-1, 0, 0);
	cameraUp      = Vector3d(0, 0, 1);
	cameraRight   = Cross(cameraTowards, cameraUp);
	cameraEye     = bbCenter + cameraTowards * (2.5 * bbRadius + dist);
	updateGL();
}

void VolumeRenderWidget::positiveYView()
{
	Vector3d oldCameraEye = bbCenter + cameraTowards * 2.5 * bbRadius;
	double dist = Dot((cameraEye - oldCameraEye), cameraTowards);
	cameraTowards = Vector3d(0, 1, 0);
	cameraUp      = Vector3d(0, 0, 1);
	cameraRight   = Cross(cameraTowards, cameraUp);
	cameraEye     = bbCenter + cameraTowards * (2.5 * bbRadius + dist);
	updateGL();
}

void VolumeRenderWidget::negativeYView()
{
	Vector3d oldCameraEye = bbCenter + cameraTowards * 2.5 * bbRadius;
	double dist = Dot((cameraEye - oldCameraEye), cameraTowards);
	cameraTowards = Vector3d(0, -1, 0);
	cameraUp      = Vector3d(0, 0, 1);
	cameraRight   = Cross(cameraTowards, cameraUp);
	cameraEye     = bbCenter + cameraTowards * (2.5 * bbRadius + dist);
	updateGL();
}

void VolumeRenderWidget::positiveZView()
{
	Vector3d oldCameraEye = bbCenter + cameraTowards * 2.5 * bbRadius;
	double dist = Dot((cameraEye - oldCameraEye), cameraTowards);
	cameraTowards = Vector3d(0, 0, 1);
	cameraUp      = Vector3d(0, 1, 0);
	cameraRight   = Cross(cameraTowards, cameraUp);
	cameraEye     = bbCenter + cameraTowards * (2.5 * bbRadius + dist);
	updateGL();
}

void VolumeRenderWidget::negativeZView()
{
	Vector3d oldCameraEye = bbCenter + cameraTowards * 2.5 * bbRadius;
	double dist = Dot((cameraEye - oldCameraEye), cameraTowards);
	cameraTowards = Vector3d(0, 0, -1);
	cameraUp      = Vector3d(0, 1, 0);
	cameraRight   = Cross(cameraTowards, cameraUp);
	cameraEye     = bbCenter + cameraTowards * (2.5 * bbRadius + dist);
	updateGL();
}

void VolumeRenderWidget::setAmbient(double value)
{
	ambient = value;
	if(currentShader->requireLighting())
		updateGL();
}

void VolumeRenderWidget::setDiffuse(double value)
{
	diffuse = value;
	if(currentShader->requireLighting())
		updateGL();
}

void VolumeRenderWidget::setShininess(double value)
{
	shininess = value;
	if(currentShader->requireLighting())
		updateGL();
}

void VolumeRenderWidget::setSpecular(double value)
{
	specular = value;
	if(currentShader->requireLighting())
		updateGL();
}

void VolumeRenderWidget::setBoundingBox()
{
	showBoundingBox = !showBoundingBox;
	updateGL();
}

void VolumeRenderWidget::setHealpixNside(double value)
{
	heapixNside = (int)value;
	generateViewingSphere();
	if (!showBoundingBox)
		updateGL();
}

/*
 * QGLWidget event functions
 */
void VolumeRenderWidget::initializeGL()
{
	//QGLWidget::makeCurrent();
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_DEPTH_TEST);

	// bind 1D transfer function table texture
	glEnable(GL_TEXTURE_1D);
	glGenTextures(1, &TF1DTextureIdx);
	glBindTexture(GL_TEXTURE_1D, TF1DTextureIdx);		
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glDisable(GL_TEXTURE_1D);

	// bind 2D transfer function table texture
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &TF2DTextureIdx);
	glBindTexture(GL_TEXTURE_2D, TF2DTextureIdx);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glDisable(GL_TEXTURE_2D);

	// bind 2D start and end position texture
	glEnable(GL_TEXTURE_RECTANGLE_NV);
	glGenTextures(1, &startPosTextureIdx);
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, startPosTextureIdx);		
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glGenTextures(1, &endPosTextureIdx);
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, endPosTextureIdx);		
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glGenTextures(1, &depthTextureIdx);
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, depthTextureIdx);
	glDisable(GL_TEXTURE_RECTANGLE_NV);

	// bind 3D volume texture
	glEnable(GL_TEXTURE_3D);
	glGenTextures(1, &volumeTextureIdx);
	glBindTexture(GL_TEXTURE_3D, volumeTextureIdx);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glGenTextures(1, &gradientTextureIdx);
	glBindTexture(GL_TEXTURE_3D, gradientTextureIdx);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glDisable(GL_TEXTURE_3D);

	// initialize GLSL Shaders
	initializeShader();
	initializeVP();

	//prepareVolumeTexture();
	//prepareTFTexture();
}

void VolumeRenderWidget::paintGL()
{
	prepareVolumeTexture();
	prepareTFTexture();

	// using FBO
	FBO->Bind();
	modelMatrix();
	// position fragment program
	positionShader->activate();
	// front faces
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawSolidCube();
	// back face
	glDrawBuffer(GL_COLOR_ATTACHMENT1_EXT);
	glClear(GL_COLOR_BUFFER_BIT);
	glDepthFunc(GL_GREATER);
	drawSolidCube();
	glDepthFunc(GL_LESS);
	// disable position fragment program and FBO
	positionShader->deactivate();
	FBO->IsValid();
	FramebufferObject::Disable();
	//imdebugTexImagef(GL_TEXTURE_RECTANGLE_NV, startPosTextureIdx);
	//imdebugTexImagef(GL_TEXTURE_RECTANGLE_NV, endPosTextureIdx);

	glEnable(GL_BLEND);
	glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA);

	// GPU Ray Casting in original framebuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if(showBoundingBox) drawCube();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, widgetWidth, 0, widgetHeight);
	glEnable(GL_TEXTURE_RECTANGLE_NV);
	glEnable(GL_TEXTURE_3D);

	currentShader->loadShader(*this);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);
	glVertex2i(0, 0);
	glTexCoord2f(0, widgetHeight);
	glVertex2f(0, widgetHeight);  
	glTexCoord2f(widgetWidth, widgetHeight);
	glVertex2f(widgetWidth, widgetHeight);
	glTexCoord2f(widgetWidth, 0.0);
	glVertex2f(widgetWidth, 0);
	glEnd();
	currentShader->unloadShader();

	glDisable(GL_TEXTURE_RECTANGLE_NV);
	glDisable(GL_TEXTURE_3D);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	//if(!showBoundingBox) {
	//	drawViewpoints();
	//}
	glDisable(GL_BLEND);
}

void VolumeRenderWidget::resizeGL(int width, int height)
{
	widgetWidth  = width;
	widgetHeight = height;
	setMouseTracking(true);

	QGLWidget::makeCurrent();
	glViewport(0, 0, width, height);
	projectionMatrix();

	// initialize texture size of framebuffer
	glEnable(GL_TEXTURE_RECTANGLE_NV);
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, startPosTextureIdx);	
	glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_RGBA32F_ARB, widgetWidth, widgetHeight, 0, GL_RGBA, GL_FLOAT, 0);
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, endPosTextureIdx);	
	glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_RGBA32F_ARB, widgetWidth, widgetHeight, 0, GL_RGBA, GL_FLOAT, 0);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, depthTextureIdx);		
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_DEPTH_COMPONENT, widgetWidth, widgetHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glDisable(GL_TEXTURE_RECTANGLE_NV);
	if(FBO == NULL) {
		FBO = new FramebufferObject();
		FBO->Bind();
		FBO->AttachTexture(GL_TEXTURE_RECTANGLE_NV, startPosTextureIdx,GL_COLOR_ATTACHMENT0_EXT);
		FBO->AttachTexture(GL_TEXTURE_RECTANGLE_NV, endPosTextureIdx,  GL_COLOR_ATTACHMENT1_EXT);
		FBO->AttachTexture(GL_TEXTURE_RECTANGLE_ARB,depthTextureIdx,   GL_DEPTH_ATTACHMENT_EXT);
		FBO->IsValid();
		FramebufferObject::Disable();
	}
}

/*
 * mouse events
 */
void VolumeRenderWidget::mousePressEvent(QMouseEvent *event)
{
    lastMousePos = event->pos();
}

void VolumeRenderWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastMousePos.x(); // ---->
    int dy = lastMousePos.y() - event->y();	// |
											// v

	if(event->buttons() & Qt::LeftButton && event->buttons() & Qt::RightButton) {
		// Translate world
		double length = 1;//(bbCenter - cameraEye).norm() * tan(cameraYFov);
		double vx = length * (double) dx / (double) widgetWidth;
		double vy = length * (double) dy / (double) widgetHeight;
		translation  += ((cameraRight * vx) + (cameraUp * vy));
		updateGL();
	} 
	else if (event->buttons() & Qt::LeftButton) {
		// Rotate world
		double vx = (double) dx / (double) widgetWidth;
		double vy = (double) dy / (double) widgetHeight;
		double theta = 4.0 * (fabs(vx) + fabs(vy));
		Vector3d vector = (cameraRight * vx) + (cameraUp * vy);
		Vector3d rotation_axis = Cross(vector, -cameraTowards);
		rotation_axis = rotation_axis.normalize();
		cameraEye     = Rotate(cameraEye, bbCenter, rotation_axis, theta);
		cameraTowards = Rotate(cameraTowards, rotation_axis, theta);
		cameraUp      = Rotate(cameraUp, rotation_axis, theta);
		cameraRight   = Cross(cameraTowards, cameraUp);
		cameraUp      = Cross(cameraRight, cameraTowards);
		cameraTowards = cameraTowards.normalize();
		cameraUp      = cameraUp.normalize();
		cameraRight   = cameraRight.normalize();
		updateGL();
    } 
	else if (event->buttons() & Qt::RightButton) {
		// Scale world 
		if(dy > 0) 
			zoom /= 0.98;
		else if(dy < 0)
			zoom *= 0.98;
		updateGL();
    }
    lastMousePos = event->pos();
	mousePosX = event->x();
	mousePosY = widgetHeight - event->y();
}

bool VolumeRenderWidget::initializeShader()
{
	GLenum err = glewInit();
	if (GLEW_OK != err){
		/* Problem: glewInit failed, something is seriously wrong. */
		cout<<"Error: "<<glewGetErrorString(err)<<endl;
		return false;
	}

	// Get the entry points for the extension.
	if (!glewIsSupported("GL_NV_fragment_program GL_VERSION_2_0")) {
		cout<<"Not Support Vertex Program or Fragment Program"<<endl;
		return false;
	}

	 //const GLubyte* OpenGLVersion = glGetString(GL_VERSION); //返回当前OpenGL实现的版本号  
	 //printf("OpenGL实现的版本号：%s\n", OpenGLVersion);
	shaders.insert(make_pair(MIPRender::shaderName(),                new MIPRender()));
	shaders.insert(make_pair(VolumeRenderTF1D::shaderName(),	     new VolumeRenderTF1D()));
	shaders.insert(make_pair(VolumeLightingRenderTF1D::shaderName(), new VolumeLightingRenderTF1D()));
	shaders.insert(make_pair(VolumeLightingRenderTF2D::shaderName(), new VolumeLightingRenderTF2D()));
	currentShader = shaders[VolumeLightingRenderTF1D::shaderName()];

	positionShader = new ShaderProgram;
	positionShader->loadShader("", "Shaders/rayposition.frag");
	for(map<string, ShaderProgram*>::iterator iter = shaders.begin(); iter != shaders.end(); ++iter) 
		iter->second->initShader();

	return true;
}
/*
 * initialize texture and framebuffer for viewpoint selection
 */
void VolumeRenderWidget::initializeVP()
{
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	glGenTextures(1, &vpStartPosTexIdx);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, vpStartPosTexIdx);		
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA32F_ARB, vpWidth,vpHeight, 0, GL_RGBA, GL_FLOAT, 0);
	glGenTextures(1, &vpEndPosTexIdx);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, vpEndPosTexIdx);		
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA32F_ARB, vpWidth, vpHeight, 0, GL_RGBA, GL_FLOAT, 0);
	glGenTextures(1, &vpResultTexIdx);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, vpResultTexIdx);		
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA32F_ARB, vpWidth, vpHeight, 0, GL_RGBA, GL_FLOAT, 0);
	glGenTextures(1, &vpDepthTexIdx);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, vpDepthTexIdx);
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_DEPTH_COMPONENT, vpWidth, vpHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glDisable(GL_TEXTURE_RECTANGLE_NV);
	glErrorCheck();

	vpFBO = new FramebufferObject();
	vpFBO->Bind();
	vpFBO->AttachTexture(GL_TEXTURE_RECTANGLE_NV,  vpStartPosTexIdx, GL_COLOR_ATTACHMENT0_EXT);
	vpFBO->AttachTexture(GL_TEXTURE_RECTANGLE_NV,  vpEndPosTexIdx,   GL_COLOR_ATTACHMENT1_EXT);
	vpFBO->AttachTexture(GL_TEXTURE_RECTANGLE_NV,  vpResultTexIdx,   GL_COLOR_ATTACHMENT2_EXT);
	vpFBO->AttachTexture(GL_TEXTURE_RECTANGLE_ARB, vpDepthTexIdx,    GL_DEPTH_ATTACHMENT_EXT);
	vpFBO->IsValid();
	FramebufferObject::Disable();
}

/*
 * prepare volume texture
 */


void VolumeRenderWidget::prepareVolumeTexture()
{
	if(!volumeReload) return;

	Volume* volume = modelData->getVolume();
	// update the x, y, z, ray step, and voxel size
	xVolume = volume->getXiSize();
	yVolume = volume->getYiSize();
	zVolume = volume->getZiSize();
	xScale  = volume->getXfSize();
	yScale  = volume->getYfSize();
	zScale  = volume->getZfSize();
	m_xcoord = volume->getXiSize() /(double)(volume->getXpSize());
	m_ycoord = volume->getYiSize() /(double)(volume->getYpSize());
	m_zcoord = volume->getZiSize() /(double)(volume->getZpSize());

	rayCastingStep = 0.3 * Min(1.0 / xVolume, 1.0 / yVolume, 1.0 / zVolume);
	voxelSize = Vector3d(1.0 / xVolume, 1.0 / yVolume, 1.0 / zVolume);

	// update the camera position
	Vector3d oldCameraEye = bbCenter + cameraTowards * 2.5 * bbRadius;
	double dist = Dot((cameraEye - oldCameraEye), cameraTowards);
	bbCenter  = Vector3d(xScale, yScale, zScale) * 0.5;
	bbRadius  = Vector3d(xScale, yScale, zScale).norm() * 0.5;
	cameraEye = bbCenter + cameraTowards * (2.5 * bbRadius + dist);

	// calculate the gradient data and download 3D Texture Data
	unsigned char *pData = new unsigned char[xVolume * yVolume * zVolume * 4];

	/***********
	* TODO:: A frequency new operation here will cause a heavy time-consuming.
	* One of possible remedies could be replacing it with a memory pool
	*************/

	glEnable(GL_TEXTURE_3D);

	// update texture - the gradient (x, y, z) and scalar data of the original volume
	volume->getOriginalXYZV(pData);
	glBindTexture(GL_TEXTURE_3D, volumeTextureIdx);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage3DEXT(GL_TEXTURE_3D, 0, GL_RGBA8, xVolume, yVolume, zVolume, 0, GL_RGBA, GL_UNSIGNED_BYTE, pData);

	// update texture - the magnitude of gradient
	//volume->getGradientData(pData);
	//glBindTexture(GL_TEXTURE_3D, gradientTextureIdx);
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//glTexImage3DEXT(GL_TEXTURE_3D, 0, GL_LUMINANCE8, xVolume, yVolume, zVolume, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, pData);

	glDisable(GL_TEXTURE_3D);
	glErrorCheck();

	// release memory
	delete []pData;
	volumeReload = false;
}

void VolumeRenderWidget::prepareTFTexture()
{
	if(TF1DReload && currentShader->requireTF1D()) {
		int dimension = 256;
		float *transferFunction = new float[dimension * 4];
		modelData->getTransferFunction(transferFunction, dimension, interactionCoarseness);

		// download 1D Texture Data
		glEnable(GL_TEXTURE_1D);
		glBindTexture(GL_TEXTURE_1D, TF1DTextureIdx);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA16, dimension, 0, GL_RGBA, GL_FLOAT, transferFunction);
		glDisable(GL_TEXTURE_1D);

		TF1DReload = false;
		delete []transferFunction;
	}
	
	if(TF2DReload && currentShader->requireTF2D()) {
		int width = 256;
		int height = 128;
		float *transferFunction = new float[width * height * 4];
		modelData->getTransferFunction(transferFunction, width, height, interactionCoarseness);
		QGLWidget::makeCurrent();

		// download 2D Texture Data
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, TF2DTextureIdx);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, width, height, 0, GL_RGBA, GL_FLOAT, transferFunction);
		glDisable(GL_TEXTURE_2D);

		delete []transferFunction;
		TF2DReload = false;
	}
}

/*
 * loads the projection matrix
 */
void VolumeRenderWidget::projectionMatrix()
{
	// Set projection transformation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float aspect = (float)widgetWidth / (widgetHeight > 1 ? widgetHeight : 1);
	glOrtho(-aspect * 2, aspect * 2, -2, 2, -10.0, 10.0);
}

/*
 * loads the model matrix
 * scales to the bounding box of the model
 */
void VolumeRenderWidget::modelMatrix()
{
	// Set camera transformation
	const Vector3d& t = cameraTowards;
	const Vector3d& u = cameraUp;
	const Vector3d& r = cameraRight;
	GLdouble camera_matrix[16] = { r.x, u.x, -t.x, 0, r.y, u.y, -t.y, 0, r.z, u.z, -t.z, 0, 0, 0, 0, 1 };
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMultMatrixd(camera_matrix);
	glScaled(zoom, zoom, zoom);
	// volume scale
	glScalef(xScale, yScale, zScale);
	glTranslated(translation.x, translation.y, translation.z);
}

/*
 *	compute the uniform color [minValue, maxValue] --> [Blue, Red]
 */
void VolumeRenderWidget::loadUniformColor(double value, double alpha)
{
	double r, g, b, h, s, v;
	h = (1.0 - value) * 2.0 / 3.0;
	s = 1.0;
	v = 1.0;

	// hsv2rgb
	int i;
	double aa, bb, cc, f;

	if (s == 0) /* Grayscale */
		r = g = b = v;
	else {
		if (h == 1.0) h = 0;
		h *= 6.0;
		i = (int)floor (h);
		f = h - i;
		aa = v * (1 - s);
		bb = v * (1 - (s * f));
		cc = v * (1 - (s * (1 - f)));
		switch (i) {
		case 0: r = v;  g = cc; b = aa; break;
		case 1: r = bb; g = v;  b = aa; break;
		case 2: r = aa; g = v;  b = cc; break;
		case 3: r = aa; g = bb; b = v;  break;
		case 4: r = cc; g = aa; b = v;  break;
		case 5: r = v;  g = aa; b = bb; break;
		}
	}

	glColor4f(r, g, b, alpha);
}

/*
 *	Draw Cube, the bounding box
 */
void VolumeRenderWidget::drawCube()
{
	double delta = 0.02;
	double scale = 0.0;
	glColor4f(0.8667f, 0.4667f, 0.4667f, 0.f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);
	// X
	glVertex3f( xCoord + delta - scale, scale,				    scale );
	glVertex3f( xCoord + delta - scale, scale,				    zCoord + delta - scale );
	glVertex3f( xCoord + delta - scale, yCoord + delta - scale, zCoord + delta - scale );
	glVertex3f( xCoord + delta - scale, yCoord + delta - scale, scale );

	glVertex3f( scale, scale,			       scale );
	glVertex3f( scale, scale,			       zCoord + delta - scale );
	glVertex3f( scale, yCoord + delta - scale, zCoord + delta - scale );
	glVertex3f( scale, yCoord + delta - scale, scale );
	// Z
	glVertex3f( scale,					scale,					zCoord + delta - scale );
	glVertex3f( scale,				    yCoord + delta - scale, zCoord + delta - scale );
	glVertex3f( xCoord + delta - scale, yCoord + delta - scale, zCoord + delta - scale );
	glVertex3f( xCoord + delta - scale, scale,					zCoord + delta - scale );

	glVertex3f( scale,					scale,					scale );
	glVertex3f( scale,					yCoord + delta - scale, scale );
	glVertex3f( xCoord + delta - scale, yCoord + delta - scale, scale );
	glVertex3f( xCoord + delta - scale, scale,					scale );
	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

/*
 *	Draw Solid Cube
 */
void VolumeRenderWidget::drawSolidCube()
{
	Vector3d vertex[8] = {			// volume vertex edge coordinates
		Vector3d(0, 0, 0), Vector3d(xCoord, 0, 0), Vector3d(0, yCoord, 0), 
		Vector3d(xCoord, yCoord, 0), Vector3d(0, 0, zCoord), 
		Vector3d(xCoord, 0, zCoord), Vector3d(0, yCoord, zCoord), 
		Vector3d(xCoord, yCoord, zCoord) };

	int faceID[] = { 1, 3, 7, 5, 0, 4, 6, 2, 2, 6, 7, 3, 0, 1, 5, 4, 4, 5, 7, 6, 0, 2, 3, 1 };

	glBegin(GL_QUADS);
	for(int i = 0; i < 24; i += 4) {
		glTexCoord3f(vertex[faceID[i]].x, vertex[faceID[i]].y, vertex[faceID[i]].z);
		glVertex3f(vertex[faceID[i]].x, vertex[faceID[i]].y, vertex[faceID[i]].z);

		glTexCoord3f(vertex[faceID[i + 1]].x, vertex[faceID[i + 1]].y, vertex[faceID[i + 1]].z);
		glVertex3f(vertex[faceID[i + 1]].x, vertex[faceID[i + 1]].y, vertex[faceID[i + 1]].z);

		glTexCoord3f(vertex[faceID[i + 2]].x, vertex[faceID[i + 2]].y, vertex[faceID[i + 2]].z);
		glVertex3f(vertex[faceID[i + 2]].x, vertex[faceID[i + 2]].y, vertex[faceID[i + 2]].z);

		glTexCoord3f(vertex[faceID[i + 3]].x, vertex[faceID[i + 3]].y, vertex[faceID[i + 3]].z);
		glVertex3f(vertex[faceID[i + 3]].x, vertex[faceID[i + 3]].y, vertex[faceID[i + 3]].z);
	}
	glEnd();
}

/*
 * Generate viewpoints on the viewing shpere
 */
void VolumeRenderWidget::generateViewingSphere()
{
	double minTheta = 1000;
	double maxTheta = 0;
	double minPhi = 1000;
	double maxPhi = 0;
	clearViewingSphere();
	Healpix::Healpix_Base sampling(heapixNside, Healpix::RING, Healpix::nside_dummy());
	for(int i = 0; i < sampling.Npix(); ++i) {
		Healpix::pointing point = sampling.pix2ang(i);
		viewingSphere.viewpoints.push_back(Viewpoint(point.theta, point.phi));
		viewingSphere.orders.push_back(i);
		minTheta = Min(minTheta, point.theta);
		maxTheta = Max(maxTheta, point.theta);
		minPhi = Min(minPhi, point.phi);
		maxPhi = Max(maxPhi, point.phi);
	}

	minThetaRange = 10;
	maxThetaRange = 0;
	minPhiRange   = 10;
	maxThetaRange = 0;
	Healpix::fix_arr<int, 8> array;
	for (int i = 0; i < sampling.Npix(); ++i) {
		Viewpoint &v1 = viewingSphere.viewpoints[i];
		sampling.neighbors(i, array);
		for (int j = 0; j < 8; j += 2) {
			if (array[j] > i) {
				Viewpoint &v2 = viewingSphere.viewpoints[array[j]];
				if (v1.theta != v2.theta) {
					minThetaRange = Min(minThetaRange, abs(v1.theta - v2.theta));
					maxThetaRange = Max(maxThetaRange, abs(v1.theta - v2.theta));
				}
				if (v1.phi != v2.phi && v1.theta != minTheta && v2.theta != minTheta && v1.theta != maxTheta && v2.theta != maxTheta) {
					double range = min(min(abs(v1.phi - v2.phi), abs(v1.phi - v2.phi - 2 * MI_PI)), abs(v2.phi - v1.phi - 2 * MI_PI));
					minPhiRange = Min(minPhiRange, range);
					maxPhiRange = Max(maxPhiRange, range);
				}
			}
		}
	}
	double minThetaDegree = minThetaRange / MI_PI * 180;
	double maxThetaDegree = maxThetaRange / MI_PI * 180;
	double minPhiDegree = minPhiRange / MI_PI * 90;
	double maxPhiDegree = maxPhiRange / MI_PI * 90;
}

/*
 * generate images under viewpoints in the viewing sphere
 */
void VolumeRenderWidget::generateViewpointImages()
{
	QString filePath = generateDirectory();
	if (filePath.isEmpty())
		return;
	
	QString angles = filePath + "/viewpoints.txt";
	ofstream fout(angles.toLocal8Bit().data());
	if (fout.is_open() == false) {
		cout << "view angle open error!" << endl;
		return;
	}

	QGLWidget::makeCurrent();
	vpFBO->Bind();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glViewport(0, 0, vpWidth, vpHeight);

	clearViewingSphere();
	float* pBuffer = new float[vpWidth * vpHeight * 4];
	unsigned char* pImage = new unsigned char[vpWidth * vpHeight * 3];
	Vector3d center(xCoord * 0.5, yCoord * 0.5, zCoord * 0.5);
	Healpix::Healpix_Base sampling(heapixNside, Healpix::RING, Healpix::nside_dummy());
	for(int i = 0; i < sampling.Npix(); ++i) {
		// direction
		Healpix::pointing point = sampling.pix2ang(i);
		double   theta     = point.theta + ((rand() % 1000) / 500.0 - 1.0) * minThetaRange;
		double   phi       = point.phi + ((rand() % 1000) / 500.0 - 1.0) * minPhiRange;
		Vector3d direction = Vector3d(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
		Vector3d up        = Vector3d(sin(theta + MI_PI /2) * cos(phi), sin(theta + MI_PI / 2) * sin(phi), cos(theta + MI_PI / 2));
		//Vector3d up        = Vector3d(-sin(phi), cos(phi), 0.0);	// phi direction
		Vector3d eye       = center + direction;

		// set viewpoint
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-0.8, 0.8, -0.8, 0.8, -10.0, 10.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(eye.x, eye.y, eye.z, center.x, center.y, center.z, up.x, up.y, up.z);

		glTranslatef(m_xcoord * 0.5, m_ycoord * 0.5, m_zcoord * 0.5);
		glScalef(xScale, yScale, zScale);
		glScalef(1.6, 1.6, 1.6);
		glTranslatef(-m_xcoord * 0.5, -m_ycoord * 0.5, -m_zcoord * 0.5);

		// position fragment program
		positionShader->activate();
		// front faces
		glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		drawSolidCube();
		// back face
		glDrawBuffer(GL_COLOR_ATTACHMENT1_EXT);
		glClear(GL_COLOR_BUFFER_BIT);
		glDepthFunc(GL_GREATER);
		drawSolidCube();
		glDepthFunc(GL_LESS);
		positionShader->deactivate();

		// viewpoint selection using GPU ray casting
		glDrawBuffer(GL_COLOR_ATTACHMENT2_EXT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
		glEnable(GL_TEXTURE_RECTANGLE_NV);
		glEnable(GL_TEXTURE_3D);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0, vpWidth, 0, vpHeight);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		if(shaderType == TF1DLIGHTING) {
			VolumeLightingRenderTF1D *render = (VolumeLightingRenderTF1D *)shaders[VolumeLightingRenderTF1D::shaderName()];
			render->loadShader(*this, direction, direction, vpStartPosTexIdx, vpEndPosTexIdx);
			glBegin(GL_QUADS);
			glTexCoord2f(0.0, 0.0);
			glVertex2i(0, 0);
			glTexCoord2f(0.0, vpHeight);
			glVertex2f(0, vpHeight);  
			glTexCoord2f(vpWidth, vpHeight);
			glVertex2f(vpWidth, vpHeight);
			glTexCoord2f(vpWidth, 0.0);
			glVertex2f(vpWidth, 0);
			glEnd();
			render->unloadShader();
		}
		else {
			VolumeLightingRenderTF2D * render = ((VolumeLightingRenderTF2D *)shaders[VolumeLightingRenderTF2D::shaderName()]);
			render->loadShader(*this, direction, direction, vpStartPosTexIdx, vpEndPosTexIdx);
			glBegin(GL_QUADS);
			glTexCoord2f(0.0, 0.0);
			glVertex2i(0, 0);
			glTexCoord2f(0.0, vpHeight);
			glVertex2f(0, vpHeight);  
			glTexCoord2f(vpWidth, vpHeight);
			glVertex2f(vpWidth, vpHeight);
			glTexCoord2f(vpWidth, 0.0);
			glVertex2f(vpWidth, 0);
			glEnd();
			render->unloadShader();
		}

		glDisable(GL_TEXTURE_RECTANGLE_NV);
		glDisable(GL_TEXTURE_3D);
		glReadBuffer(GL_COLOR_ATTACHMENT2_EXT);
		glReadPixels(0, 0, vpWidth, vpHeight, GL_RGBA, GL_FLOAT, pBuffer);

		char filename[1024];
		sprintf(filename, "%s/%d.bmp", filePath.toLocal8Bit().data(), i);
		for (unsigned int j = 0; j < vpHeight; ++j) {
			for (unsigned int k = 0; k < vpWidth; ++k) {
				int index = j * vpWidth + k;
				pImage[index * 3 + 0] = unsigned char(Min(pBuffer[index * 4 + 0] * 255, 255.0f));
				pImage[index * 3 + 1] = unsigned char(Min(pBuffer[index * 4 + 1] * 255, 255.0f));
				pImage[index * 3 + 2] = unsigned char(Min(pBuffer[index * 4 + 2] * 255, 255.0f));
			}
		}
		Util::BMPWriteImage(filename, pImage, vpWidth, vpHeight, true);
		fout<<i<<" "<<theta<<" "<<phi<<endl;
		viewingSphere.viewpoints.push_back(Viewpoint(theta, phi));
	}

	delete []pBuffer;
	delete []pImage;
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glViewport(0, 0, widgetWidth, widgetHeight);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	vpFBO->IsValid();
	FramebufferObject::Disable();
}

void VolumeRenderWidget::generateViewpointImages2()
{
	return;
	QString filePath = generateDirectory();
	if (filePath.isEmpty())
		return;

	QString angles = filePath + "/viewpoints.txt";
	ofstream fout(angles.toLocal8Bit().data());
	if (fout.is_open() == false) {
		cout << "view angle open error!" << endl;
		return;
	}

	QGLWidget::makeCurrent();
	vpFBO->Bind();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glViewport(0, 0, vpWidth, vpHeight);

	float* pBuffer = new float[vpWidth * vpHeight * 4];
	unsigned char* pImage = new unsigned char[vpWidth * vpHeight * 3];
	Vector3d center(xCoord * 0.5, yCoord * 0.5, zCoord * 0.5);
	for (int i = 0; i < 100; ++i) {
		// direction
		double   value = rand() * 10.0 / RAND_MAX - 5.0;
		double   theta = rand() * MI_PI / RAND_MAX;
		double   phi   = rand() * MI_PI * 2 / RAND_MAX;
		Vector3d direction = Vector3d(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
		Vector3d up = Vector3d(sin(theta + MI_PI / 2) * cos(phi), sin(theta + MI_PI / 2) * sin(phi), cos(theta + MI_PI / 2));
		Vector3d right = Cross(direction, up);
		up = (up + right * value).normalize();
		Vector3d eye = center + direction;

		// set viewpoint
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-0.8, 0.8, -0.8, 0.8, -10.0, 10.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(eye.x, eye.y, eye.z, center.x, center.y, center.z, up.x, up.y, up.z);

		glTranslatef(m_xcoord * 0.5, m_ycoord * 0.5, m_zcoord * 0.5);
		glScalef(xScale, yScale, zScale);
		glScalef(1.7, 1.7, 1.7);
		glTranslatef(-m_xcoord * 0.5, -m_ycoord * 0.5, -m_zcoord * 0.5);

		// position fragment program
		positionShader->activate();
		// front faces
		glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		drawSolidCube();
		// back face
		glDrawBuffer(GL_COLOR_ATTACHMENT1_EXT);
		glClear(GL_COLOR_BUFFER_BIT);
		glDepthFunc(GL_GREATER);
		drawSolidCube();
		glDepthFunc(GL_LESS);
		positionShader->deactivate();

		// viewpoint selection using GPU ray casting
		glDrawBuffer(GL_COLOR_ATTACHMENT2_EXT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_TEXTURE_RECTANGLE_NV);
		glEnable(GL_TEXTURE_3D);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0, vpWidth, 0, vpHeight);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		if (shaderType == TF1DLIGHTING) {
			VolumeLightingRenderTF1D *render = (VolumeLightingRenderTF1D *)shaders[VolumeLightingRenderTF1D::shaderName()];
			render->loadShader(*this, direction, direction, vpStartPosTexIdx, vpEndPosTexIdx);
			glBegin(GL_QUADS);
			glTexCoord2f(0.0, 0.0);
			glVertex2i(0, 0);
			glTexCoord2f(0.0, vpHeight);
			glVertex2f(0, vpHeight);
			glTexCoord2f(vpWidth, vpHeight);
			glVertex2f(vpWidth, vpHeight);
			glTexCoord2f(vpWidth, 0.0);
			glVertex2f(vpWidth, 0);
			glEnd();
			render->unloadShader();
		}
		else {
			VolumeLightingRenderTF2D * render = ((VolumeLightingRenderTF2D *)shaders[VolumeLightingRenderTF2D::shaderName()]);
			render->loadShader(*this, direction, direction, vpStartPosTexIdx, vpEndPosTexIdx);
			glBegin(GL_QUADS);
			glTexCoord2f(0.0, 0.0);
			glVertex2i(0, 0);
			glTexCoord2f(0.0, vpHeight);
			glVertex2f(0, vpHeight);
			glTexCoord2f(vpWidth, vpHeight);
			glVertex2f(vpWidth, vpHeight);
			glTexCoord2f(vpWidth, 0.0);
			glVertex2f(vpWidth, 0);
			glEnd();
			render->unloadShader();
		}

		glDisable(GL_TEXTURE_RECTANGLE_NV);
		glDisable(GL_TEXTURE_3D);
		glReadBuffer(GL_COLOR_ATTACHMENT2_EXT);
		glReadPixels(0, 0, vpWidth, vpHeight, GL_RGBA, GL_FLOAT, pBuffer);

		char filename[1024];
		sprintf(filename, "%s/%d.bmp", filePath.toLocal8Bit().data(), i);
		for (unsigned int j = 0; j < vpHeight; ++j) {
			for (unsigned int k = 0; k < vpWidth; ++k) {
				int index = j * vpWidth + k;
				pImage[index * 3 + 0] = unsigned char(Min(pBuffer[index * 4 + 0] * 255, 255.0f));
				pImage[index * 3 + 1] = unsigned char(Min(pBuffer[index * 4 + 1] * 255, 255.0f));
				pImage[index * 3 + 2] = unsigned char(Min(pBuffer[index * 4 + 2] * 255, 255.0f));
			}
		}
		Util::BMPWriteImage(filename, pImage, vpWidth, vpHeight, true);
		fout << i << " " << theta << " " << phi << endl;
	}

	delete[]pBuffer;
	delete[]pImage;
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glViewport(0, 0, widgetWidth, widgetHeight);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	vpFBO->IsValid();
	FramebufferObject::Disable();
}

/*
 * create a directory for generated images
 */
QString VolumeRenderWidget::generateDirectory()
{
	QString curDataFile(modelData->getVolumeName().c_str());
	QString filePath = curDataFile.left(curDataFile.length() - 5);

	QDateTime time = QDateTime::currentDateTime();			//获取系统现在的时间
	QString timeStr = time.toString("yyyy-MM-dd-hh-mm-ss"); //设置显示格式

	QString dirName = filePath + "-" + timeStr;

	QDir dir;
	bool exist = dir.exists(dirName);
	if (exist) {
		QMessageBox::warning(this, tr("Create Directory"), tr("文件夹已经存在！"));
		return "";
	}
	else {
		bool ok = dir.mkdir(dirName);
		if (!ok) {
			QMessageBox::warning(this, tr("Create Directory"), tr("文件夹创建识别！"));
			return "";
		}
	}

	return dirName;
}

/*
 * clear viewpoints in the viewing sphere
 */
void VolumeRenderWidget::clearViewingSphere()
{
	viewingSphere.orders.clear();
	viewingSphere.viewpoints.clear();
	viewingSphere.links.clear();
	viewingSphere.triangles.clear();
}

/*
 * draw viewpoints
 */
void VolumeRenderWidget::drawViewpoints()
{
	if (viewingSphere.viewpoints.empty()) return;

	Vector3d& t = cameraTowards;
	Vector3d& u = cameraUp;
	Vector3d& r = cameraRight;
	GLdouble camera_matrix[16] = { r.x, u.x, -t.x, 0, r.y, u.y, -t.y, 0, r.z, u.z, -t.z, 0, 0, 0, 0, 1 };

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glMultMatrixd(camera_matrix);
	glScaled(zoom, zoom, zoom);

	glPointSize(5);
	glColor3f(0.8, 0, 0);
	glBegin(GL_POINTS);
	for (size_t i = 0; i < viewingSphere.viewpoints.size(); ++i) {
		Viewpoint &v = viewingSphere.viewpoints[i];
		Vector3d p(sin(v.theta) * cos(v.phi), sin(v.theta) * sin(v.phi), cos(v.theta));
		if (Dot(p, t) < 0)
			glVertex3f(p.x, p.y, p.z);
	}
	glEnd();

	//glPointSize(3);
	//glColor3f(0.4, 0.4, 0.4);
	//glBegin(GL_POINTS);
	//for (size_t i = 0; i < viewingSphere.viewpoints.size(); ++i) {
	//	Viewpoint &v = viewingSphere.viewpoints[i];
	//	double theta = v.theta + ((rand() % 1000) / 500.0 - 1.0) * minThetaRange;
	//	double phi   = v.phi   + ((rand() % 1000) / 500.0 - 1.0) * minPhiRange;
	//	Vector3d p(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
	//	if (Dot(p, t) < 0)
	//		glVertex3f(p.x, p.y, p.z);
	//}
	//glEnd();

	glPointSize(1);

	//glColor3f(0, 0.6, 0);
	//glBegin(GL_LINES);
	//for (size_t i = 0; i < viewingSphere.viewpoints.size(); ++i) {
	//	Viewpoint &v = viewingSphere.viewpoints[i];
	//	double theta = v.theta + ((rand() % 1000) / 500.0 - 1.0) * minThetaRange;
	//	double phi = v.phi + ((rand() % 1000) / 500.0 - 1.0) * minPhiRange;
	//	Vector3d p1(sin(v.theta) * cos(v.phi), sin(v.theta) * sin(v.phi), cos(v.theta));
	//	Vector3d p2(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
	//	if (Dot(p1, t) < 0) {
	//		glVertex3f(p1.x, p1.y, p1.z);
	//		glVertex3f(p2.x, p2.y, p2.z);
	//	}
	//}
	//glEnd();

	glPopMatrix();
}


/*
 * draw edges of viewing sphere
 */
void VolumeRenderWidget::drawViewingSphereEdges()
{
	if (viewingSphere.viewpoints.empty()) return;

	Vector3d& t = cameraTowards;
	Vector3d& u = cameraUp;
	Vector3d& r = cameraRight;
	GLdouble camera_matrix[16] = { r.x, u.x, -t.x, 0, r.y, u.y, -t.y, 0, r.z, u.z, -t.z, 0, 0, 0, 0, 1 };

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glMultMatrixd(camera_matrix);
	glScaled(zoom, zoom, zoom);

	glColor3f(0.8, 0.8, 0.8);
	glBegin(GL_LINES);
	Healpix::Healpix_Base sampling(heapixNside, Healpix::RING, Healpix::nside_dummy());
	Healpix::fix_arr<int, 8> array;
	for (int i = 0; i < sampling.Npix(); ++i) {
		Viewpoint v1 = viewingSphere.viewpoints[i];
		sampling.neighbors(i, array);
		for (int j = 0; j < 8; j += 2) {
			if (array[j] > i) {
				Viewpoint v2 = viewingSphere.viewpoints[array[j]];
				Vector3d p1(sin(v1.theta) * cos(v1.phi), sin(v1.theta) * sin(v1.phi), cos(v1.theta));
				Vector3d p2(sin(v2.theta) * cos(v2.phi), sin(v2.theta) * sin(v2.phi), cos(v2.theta));
				if (v1.phi > MI_PI * 1.5 && v2.phi < MI_PI * 0.5)
					v1.phi -= MI_PI * 2;
				if (v2.phi > MI_PI * 1.5 && v1.phi < MI_PI * 0.5)
					v2.phi -= MI_PI * 2;
				if (abs(v1.phi - v2.phi) > 1)
					glColor3f(0.8, 0, 0);
				else
					glColor3f(0.8, 0.8, 0.8);
				glVertex3f(p1.x, p1.y, p1.z);
				glVertex3f(p2.x, p2.y, p2.z);
			}
		}
	}
	glEnd();

	glPopMatrix();
}