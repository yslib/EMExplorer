#ifndef TF1DTEXTURECANVAS_H
#define TF1DTEXTURECANVAS_H

#include <QGLWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>

class ModelData;
class TF1DMappingCanvas;

class TF1DTextureCanvas : public QOpenGLWidget,protected QOpenGLFunctions
{
    Q_OBJECT

public:
    TF1DTextureCanvas(ModelData *model,TF1DMappingCanvas * tf, QWidget *parent = 0);
    ~TF1DTextureCanvas();

protected:
    void initializeGL();
	void resizeGL(int width, int height);
    void paintGL();

protected:
	ModelData *modelData;		///< model data
	TF1DMappingCanvas * m_transferFunction;
	unsigned int m_texture;
};

#endif