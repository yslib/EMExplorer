#ifndef TF1DTEXTURECANVAS_H
#define TF1DTEXTURECANVAS_H

#include <QGLWidget>

class ModelData;

class TF1DTextureCanvas : public QGLWidget
{
    Q_OBJECT

public:
    TF1DTextureCanvas(ModelData *model, QWidget *parent = 0, QGLWidget *shareWidget = 0);
    ~TF1DTextureCanvas();

protected:
    void initializeGL();
	void resizeGL(int width, int height);
    void paintGL();

protected:
	ModelData *modelData;		///< model data
};

#endif