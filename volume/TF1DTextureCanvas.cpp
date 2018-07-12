#include <GL/glew.h>
#include "TF1DTextureCanvas.h"
#include "ModelData.h"

QGLFormat getQGLFormat() 
{
	QGLFormat format = QGLFormat::defaultFormat();
	format.setAlpha(true);
    return format;
}

TF1DTextureCanvas::TF1DTextureCanvas(ModelData *model, QWidget *parent, QGLWidget *shareWidget)
	: QGLWidget(getQGLFormat(), parent, shareWidget)
	, modelData(model)
{
	
}

TF1DTextureCanvas::~TF1DTextureCanvas()
{

}

void TF1DTextureCanvas::initializeGL()
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
}

void TF1DTextureCanvas::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.f, 1.f, 0.f, 1.f, -2.f, 1.f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void TF1DTextureCanvas::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    float inc = 0.1f;
    // paint checkerboard
    for (int i = 0 ; i < 10 ; ++i) {
        glBegin(GL_QUADS);
            // Front Face
            if (i % 2)
                glColor3f(0.6f, 0.6f, 0.6f);
            else
                glColor3f(1.f, 1.f, 1.f);
            glVertex3f( i      * inc, 0.0f,  -0.5f);  // Bottom Left
            glVertex3f((i + 1) * inc, 0.0f,  -0.5f);  // Bottom Right
            glVertex3f((i + 1) * inc, 0.5f,  -0.5f);  // Top Right
            glVertex3f( i      * inc, 0.5f,  -0.5f);  // Top Left
        glEnd();
        glBegin(GL_QUADS);
            // Front Face
            if (i % 2)
                glColor3f(1.f, 1.f, 1.f);
            else
                glColor3f(0.6f, 0.6f, 0.6f);
            glVertex3f( i      * inc, 0.5f,  -0.5f);  // Bottom Left
            glVertex3f((i + 1) * inc, 0.5f,  -0.5f);  // Bottom Right
            glVertex3f((i + 1) * inc, 1.0f,  -0.5f);  // Top Right
            glVertex3f( i      * inc, 1.0f,  -0.5f);  // Top Left
        glEnd();
    }

	// paint transfer function
	glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_TEXTURE_1D);
	unsigned int texIdx = modelData->getTF1DTextureIdx();
	glBindTexture(GL_TEXTURE_1D, texIdx);

    glBegin(GL_QUADS);
        glColor4f(1.f, 1.f, 1.f, 1.f);
        // Bottom Left
        glTexCoord1f(0.f);
        glVertex3f(0.f, 0.f, -0.5f);

        // Bottom Right
        glTexCoord1f(1.f);
        glVertex3f(1.f, 0.f, -0.5f);

        // Top Right
        glTexCoord1f(1.f);
        glVertex3f(1.f, 1.f, -0.5f);

        // Top Left
        glTexCoord1f(0.f);
        glVertex3f(0.f, 1.f, -0.5f);

    glEnd();

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_1D);
}
