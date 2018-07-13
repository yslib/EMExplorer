#include <GL/glew.h>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QColorDialog>
#include <QCursor>
#include <QPoint>
#include <QMessageBox>
#include "TF2DPrimitive.h"
#include "TF2DMappingCanvas.h"
#include "ModelData.h"
#include "Volume.h"

extern QGLFormat getQGLFormat();

TF2DMappingCanvas::TF2DMappingCanvas(ModelData *model, QWidget *parent, QGLWidget *shareWidget)
    : QGLWidget(getQGLFormat(), parent, shareWidget)
	, modelData(model)
    , selectedPrimitive(0)
    , showHistogram(false)
    , showGrid(false)
    , histogramLogarithmic(true)
    , histogramBrightness(1.f)
	, histogramReload(false)
    , dragging(false)
    , scaleFactor(1.f)
{
}

TF2DMappingCanvas::~TF2DMappingCanvas() 
{
	for(size_t i = 0; i < primitives.size(); ++i)
		delete primitives[i];
	primitives.clear();
}

void TF2DMappingCanvas::load(const char *filename)
{
	FILE *fp = fopen(filename, "r");
	if(!fp) {
		QMessageBox::critical(this, tr("Error"),
							  "The selected transfer function could not be loaded.");
		return;
	}

	deselectPrimitive();
	for(size_t i = 0; i < primitives.size(); ++i)
		delete primitives[i];
	primitives.clear();

	int type, primNum;
	fscanf(fp, "%d\n", &primNum);
	for(int i = 0; i < primNum; ++i) {
		fscanf(fp, "%d\n", &type);
		TF2DPrimitive *prim = 0;
		if(type == 0) 
			prim = new TransFuncQuad();
		else 
			prim = new TransFuncBanana();
		prim->load(fp);
		primitives.push_back(prim);
	}

	fclose(fp);

    updateTF();

    update();
}

void TF2DMappingCanvas::save(const char *filename)
{
	FILE *fp = fopen(filename, "w");
	if(!fp) {
		QMessageBox::critical(this, tr("Error"),
                              tr("The transfer function could not be saved."));
		return;
	}

	fprintf(fp, "%d\n", primitives.size());
	for(size_t i = 0; i < primitives.size(); ++i) {
		TransFuncQuad *prim = dynamic_cast<TransFuncQuad*>(primitives[i]);
		fprintf(fp, "%d\n", prim ? 0 : 1);
		primitives[i]->save(fp);
	}

	fclose(fp);

}

void TF2DMappingCanvas::initializeGL() 
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.f, 1.f, 0.f, 1.f, -2.f, 1.f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &histogramTex);
	glBindTexture(GL_TEXTURE_2D, histogramTex);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glDisable(GL_TEXTURE_2D);
}

void TF2DMappingCanvas::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.f, 1.f, 0.f, 1.f, -2.f, 1.f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void TF2DMappingCanvas::paintGL() 
{
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    if (showHistogram) {
		if(histogramReload)
			updateHistogramTexture();
        glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, histogramTex);
        glBegin(GL_QUADS);
            // Front Face
            glColor3f(1.f, 1.f, 1.f);
            glTexCoord2f(0.f, 0.f); glVertex3f(0.f, 0.f, -0.5f);  // Bottom Left Of The Texture and Quad
            glTexCoord2f(1.f, 0.f); glVertex3f(1.f, 0.f, -0.5f);  // Bottom Right Of The Texture and Quad
            glTexCoord2f(1.f, 1.f); glVertex3f(1.f, 1.f, -0.5f);  // Top Right Of The Texture and Quad
            glTexCoord2f(0.f, 1.f); glVertex3f(0.f, 1.f, -0.5f);  // Top Left Of The Texture and Quad
        glEnd();
        glDisable(GL_TEXTURE_2D);
    }

    if (showGrid) {
        glBegin(GL_LINES);
        glColor3f(0.7f, 0.7f, 0.7f);
        for (int i = 0; i < 10; ++i) {
            glVertex3f(0.f, i * 0.1f, -0.5f);
            glVertex3f(1.f, i * 0.1f, -0.5f);

            glVertex3f(i * 0.1f,  0.f, -0.5f);
            glVertex3f(i * 0.1f,  1.f, -0.5f);
        }
        glEnd();
    }

    glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    paintInEditor();

    glDisable(GL_BLEND);
}

void TF2DMappingCanvas::mousePressEvent(QMouseEvent* event) 
{
    event->accept();
	int w = width();
	int h = height();
    Vector2f pos = Vector2f(    static_cast<float>(event->x()) / w,
                            1.f-static_cast<float>(event->y()) / h);

    TF2DPrimitive* p = getPrimitiveUnderMouse(pos);

    if (p)
        selectPrimitive(p, pos);
    else
        deselectPrimitive();

    mouseCoord = pos;

    update();
}

void TF2DMappingCanvas::mouseMoveEvent(QMouseEvent* event) 
{
    event->accept();
	int w = width();
	int h = height();
    Vector2f pos = Vector2f(    static_cast<float>(event->x()) / w,
                            1.f-static_cast<float>(event->y()) / h);

    Vector2f offset = pos - mouseCoord;

    if (selectedPrimitive) {
        if (!dragging) {
            emit toggleInteraction(true);
            dragging = true;
        }
        // test whether the movement is correct
        bool inside = selectedPrimitive->move(offset);
        // one or more control points are moved outside of the canvas
        // -> do not allow movement and reset mouse cursor to position before movement
        if (!inside) {
            QPoint p(static_cast<int>(mouseCoord.x * w),
                     static_cast<int>((1.f - mouseCoord.y) * h));
            p = mapToGlobal(p);
            QCursor::setPos(p);
        }
        else {
            mouseCoord = pos;
            updateTF();
        }
    }

    update();
}

void TF2DMappingCanvas::mouseReleaseEvent(QMouseEvent* event) 
{
    event->accept();
    if (selectedPrimitive && dragging) {
        dragging = false;
        emit toggleInteraction(false);
    }
}

void TF2DMappingCanvas::mouseDoubleClickEvent(QMouseEvent* event) 
{
    event->accept();
    // mousepress event was send right before doubleClick event
    // -> primitive already selected
    colorizePrimitive();
}

void TF2DMappingCanvas::wheelEvent(QWheelEvent* event) 
{
    event->accept();

    float offset = 0;
	if (event->delta() < 0)
        offset = -10 / 255.f;
    else if (event->delta() > 0)
        offset = 10 / 255.f;

    if (selectedPrimitive) {
        float trans = selectedPrimitive->getColor().w;
        if ((trans + offset) > 1.f)
            trans = 1.f;
        else if (trans + offset < 0)
            trans = 0.f;
        else
            trans += offset;

        transparencyChanged(trans);

        emit setTransparencySlider(trans * 255);
    }
}

void TF2DMappingCanvas::volumeChanged()
{
    histogramBrightness = 1.f;
    showHistogram = false;
    scaleFactor = 1.f;

	//for(size_t i = 0; i < primitives.size(); ++i)
	//	primitives[i]->setScaleFactor(scaleFactor);

	histogramReload = true;
}

void TF2DMappingCanvas::setGridVisible(bool v)
{
	showGrid = v;
	update();
}

void TF2DMappingCanvas::setHistogramVisible(bool v) 
{
    showHistogram = v;
	if (showHistogram && histogramReload) {
		updateHistogramTexture();
		update();
    }
}

void TF2DMappingCanvas::setHistogramLogarithmic(bool v) 
{
    histogramLogarithmic = v;
    updateHistogramTexture();
    update();
}

void TF2DMappingCanvas::getTransferFunction(float* transferFunction, size_t width, size_t height, float factor)
{
	QGLWidget::makeCurrent();
	
	// save current viewport, e.g. size of mapping canvas and current drawbuffer
    glPushAttrib(GL_VIEWPORT_BIT);
    // set viewport to the dimensions of the texture
    glViewport(0, 0, width, height);

    // clear previous content
    glClear(GL_COLOR_BUFFER_BIT);

    // set correct projection and modelview matrices
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.f, 1.f, 0.f, 1.f, -2.f, 1.f);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    // paint primitives
    for (size_t i = 0; i < primitives.size(); ++i)
        primitives[i]->paint();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    // restore viewport
    glPopAttrib();

    // read tf data from framebuffer and copy it into tf texture
    glReadPixels(0, 0, width, height, GL_RGBA, GL_FLOAT, transferFunction);
}

void TF2DMappingCanvas::paintForSelection() 
{
    for (size_t i = 0; i < primitives.size(); ++i)
        primitives[i]->paintForSelection(i);
}

void TF2DMappingCanvas::paintInEditor() 
{
    for (size_t i = 0; i < primitives.size(); ++i)
        primitives[i]->paintInEditor();
}

void TF2DMappingCanvas::updateTF() 
{
    emit repaintSignal();
}

void TF2DMappingCanvas::selectPrimitive(TF2DPrimitive* p, Vector2f pos) 
{
    if (selectedPrimitive)
        selectedPrimitive->deselect();

    selectedPrimitive = p;
    selectedPrimitive->select(pos);

    // inform widget about selection
    emit primitiveSelected();
}

void TF2DMappingCanvas::deselectPrimitive() 
{
    if (selectedPrimitive) {
        selectedPrimitive->deselect();
        selectedPrimitive = 0;
    }

    // inform editorwidget about deselection
    emit primitiveDeselected();
}

TF2DPrimitive* TF2DMappingCanvas::getPrimitiveForClickedControlPoint(Vector2f pos) const 
{
    if (primitives.empty())
        return 0;

    int min = 0;
    // A distance of 2 can never happen because the canvas is normalized to [0,1]x[0,1],
    // so this value is huge enough.
    float mindist = 2.f;
    float d;
    for (size_t i = 0; i < primitives.size(); ++i) {
         d = primitives[i]->getClosestControlPointDist(pos);
         if ((d < mindist) && (d < primitives[i]->getControlPointSize())) {
            mindist = d;
            min = i;
         }
    }
    if (mindist == 2.f)
        return 0;
    else
        return primitives[min];
}


TF2DPrimitive* TF2DMappingCanvas::getPrimitiveUnderMouse(Vector2f pos) 
{
    TF2DPrimitive* clicked = 0;

    makeCurrent();
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_BLEND);
    // paint all primitives with special color
    // (first component is id and both other components are 123)
    paintForSelection();

    glPopMatrix();
    glPopAttrib();

    // read pixels at clicked position
    GLubyte pixel[3];
    glReadPixels(static_cast<int>(width()*pos.x),
                 static_cast<int>(height()*pos.y), 1, 1, GL_RGB, GL_UNSIGNED_BYTE, &pixel);

	if ((pixel[1] == 123) && (pixel[2] == 123)) {
		if(pixel[0] >= 0 && pixel[0] < primitives.size())
			clicked = primitives[pixel[0]];
	}

    // no primitive clicked but the user could have clicked on a control point
    if (!clicked)
        clicked = getPrimitiveForClickedControlPoint(pos);

    return clicked;
}

void TF2DMappingCanvas::updateHistogramTexture() 
{
	Volume* volume = modelData->getVolume();
	int width      = volume->getVGWidth();
	int height     = volume->getVGHeight();
	double* values = volume->getVGTable();
	double  maxVal = volume->getMaxVGValue();

	unsigned char* histogram = new unsigned char[width * height];
	for(int i = 0, num = width * height; i < num; ++i) {
		double value = values[i];
		if(histogramLogarithmic)
			value = log(1 + value) / log(1 + maxVal);
		else
			value /= maxVal;
		histogram[i] = static_cast<unsigned char>(Clamp(std::floor(value * 255 * histogramBrightness + 0.5), 0.0, 255.0));
    }

	QGLWidget::makeCurrent();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, histogramTex);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE8, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, histogram);
	glDisable(GL_TEXTURE_2D);

	delete []histogram;
	histogramReload = false;
}


void TF2DMappingCanvas::addQuadPrimitive() 
{
    // create new primitive
    TF2DPrimitive* p = new TransFuncQuad(Vector2f(0.5f, 0.5f), 0.2f, Vector4f(0.5f, 0.5f, 0.5f, 0.5f), scaleFactor);

    // add primitive to transfer function
	primitives.push_back(p);

    // update texture of transfer function
    updateTF();

    // select primitive and update widgets
    selectPrimitive(p);

    // repaint canvas
    update();
}

void TF2DMappingCanvas::addBananaPrimitive() 
{
    // create new primitive
    TF2DPrimitive* p = new TransFuncBanana(Vector2f(0.f, 0.f), Vector2f(0.3f, 0.4f),
                                           Vector2f(0.34f, 0.2f), Vector2f(0.5f, 0.f),
                                           Vector4f(0.5f, 0.5f, 0.5f, 0.5f), scaleFactor);

    // add primitive to transfer function
    primitives.push_back(p);

    // update texture of transfer function
    updateTF();

    // select primitive and update widgets
    selectPrimitive(p);

    // repaint canvas
    update();
}

void TF2DMappingCanvas::deletePrimitive() 
{
    if (selectedPrimitive) {
		std::vector<TF2DPrimitive*>::iterator it;
		for (it = primitives.begin(); it != primitives.end(); ++it) {
			if (*it == selectedPrimitive) {
				primitives.erase(it);
				break;
			}
		}
		delete selectedPrimitive;
        selectedPrimitive = 0;
        updateTF();
        emit primitiveDeselected();

        update();
    }
}

void TF2DMappingCanvas::colorizePrimitive() 
{
    if (selectedPrimitive) {
        Vector4f primitiveColor = selectedPrimitive->getColor() * 255;
        // set start color to current color of the primitive
        QColor qcolor(primitiveColor.x, primitiveColor.y, primitiveColor.z, primitiveColor.w);
        qcolor = QColorDialog::getRgba(qcolor.rgba());
        if (qcolor.isValid()) {
            primitiveColor.x = qcolor.red();
            primitiveColor.y = qcolor.green();
            primitiveColor.z = qcolor.blue();
            selectedPrimitive->setColor(primitiveColor / 255.f);

            // set transparency slider to choosen value
            //emit setTransparencySlider(primitiveColor.w);

            updateTF();

            update();
        }
    }
}

void TF2DMappingCanvas::fuzzinessChanged(int fuzzi)
{
    if (selectedPrimitive)
        selectedPrimitive->setFuzziness(fuzzi/100.f);

    updateTF();

    update();
}

void TF2DMappingCanvas::transparencyChanged(int trans)
{
    if (selectedPrimitive) {
        Vector4f color = selectedPrimitive->getColor();
        color.w = trans / 255.f;
        selectedPrimitive->setColor(color);
    }

    updateTF();

    update();
}

void TF2DMappingCanvas::resetTransferFunction() 
{
	deselectPrimitive();
	for(size_t i = 0; i < primitives.size(); ++i)
		delete primitives[i];
	primitives.clear();

    updateTF();

    update();
}

void TF2DMappingCanvas::histogramBrightnessChanged(int brightness) 
{
    histogramBrightness = brightness / 100.f;

    updateHistogramTexture();

    update();
}
