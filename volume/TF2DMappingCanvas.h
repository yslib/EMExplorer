#ifndef TF2DMAPPINGCANVAS_H
#define TF2DMAPPINGCANVAS_H

#include "Rendering/Vector.h"
#include <QGLWidget>
#include <QOpenGLWidget>

class ModelData;
class TF2DPrimitive;

class TF2DMappingCanvas : public QGLWidget
{
	Q_OBJECT
public:
    TF2DMappingCanvas(ModelData *model, QWidget *parent = 0, QGLWidget *shareWidget = 0);
    ~TF2DMappingCanvas();

	// The central entry point for loading a gradient transfer function.
    // The file extension is extracted and based on that the apropriate
    // load function is called. If there is no extension, loading will be unsuccessful.
    void load(const char *filename);

    // Saves the transfer function to a file. Any data in the file will be overwritten.
    void save(const char *filename);

    // Initializes the painter. The modelviewMatrix is set to identity and the projection is set to orthographic.
	void initializeGL();

    // Called when the size of the canvas changes. Adapts the projection matrix to new size.
    void resizeGL(int width, int height); 
	
	// Paints the histogram texture, the grid when enabled and the primitives the transfer function consists of.
    void paintGL();

    // Handles the event when a mouse button is pressed.
    // Determines whether a primitive is under the mouse cursor or not and selects it
    // if possible.
    void mousePressEvent(QMouseEvent* event);

    // Handles the events when the mouse is moved while a button is pressed.
    // Moves the selected primitive by the movement of mouse. Nothing happens when
    // no mousebutton is pressed or no primitive is selected.
    void mouseMoveEvent(QMouseEvent* event);

    // Handles the events when a pressed mosue button is released. It turns the coarseness
    // mode off when a primitive was moved.
    void mouseReleaseEvent(QMouseEvent* event);

    // Handles mouse doubleclick events. Opens a colordialog when the doubleclick occured
    // on a primitive.
    void mouseDoubleClickEvent(QMouseEvent* event);

    // Handles events of the mousewheel. It changes the transparency of the selected primitive.
    void wheelEvent(QWheelEvent* event);

	// Called when the rendered volume changes.
	void volumeChanged();

    // Enables or disables display of an underlying grid.
	void setGridVisible(bool v);

    // Enables or disables display of the histogram. It creates the histogram texture if
    // there is none.
    void setHistogramVisible(bool v);

	// Enables or disables the logarithmic display of the histogram. The histogram texture
    // is updatet when the state changes.
    void setHistogramLogarithmic(bool  v);

    // Returns the current selected primitive or 0 if no primitive is selected.
	const TF2DPrimitive* getSelectedPrimitive() { return selectedPrimitive; }

	// Get the 2D transfer function
	void getTransferFunction(float* transferFunction, size_t width, size_t height, float factor);

protected:
    // Paints all primitives for selection purposes.
    void paintForSelection();

    // Paints all primitives for display in an editor. Control points are added for every primitive.
    // An outline is added to the selected primitive.
    void paintInEditor();

	// Updates the texture of the transfer function. Therefore the updateTexture()-method of the
    // transfer function object is called. A repaint of the volume rendering is triggered after that.
    void updateTF();

	// Marks the given primitive as selected. It is tested whether a control point was selected
    // or the whole primitive.
    void selectPrimitive(TF2DPrimitive* p, Vector2f mousePos = Vector2f(0.f, 0.f));

    // Resets the selection state of the current selected primitive.
    void deselectPrimitive();

	// Returns the primitive that is under the mouse cursor or 0 if there is none.
    TF2DPrimitive* getPrimitiveForClickedControlPoint(Vector2f pos) const;

	// Returns the primitive that is under the mouse cursor or 0 if there is none.
    // Therefor all primitives are painted with an id in the color and the pixels
    // are read at the clicked position.
    TF2DPrimitive* getPrimitiveUnderMouse(Vector2f pos);

    // Updates all texel in the histogram texture.
    void updateHistogramTexture();

public slots:
    // Adds a quad primitive to the editor and updates the texture of the tf.
    void addQuadPrimitive();

    // Adds a banana primitive to the editor and updates the texture of the tf.
    void addBananaPrimitive();

    // Deletes the current selected primitive.
    void deletePrimitive();

    // Opens a colordialog if a primitive is selected. The color of the primitive
    // is set to the color choosen in the dialog.
    void colorizePrimitive();

    // Updates the histogramtexture to new brightness.
    void histogramBrightnessChanged(int brightness);

    // Sets the fuzziness of the current selected primitive to the given value.
    void fuzzinessChanged(int fuzzi);

    // Sets the transparency of the current selected primitive to the given value.
    void transparencyChanged(int trans);

    // Resets the transfer function to default, i.e. all primitives are deleted.
    void resetTransferFunction();

signals:
    // Signal that is emitted when the user changes the transparency of a primitive.
    // It sets the slider that represents the transparency to the new value.
    void setTransparencySlider(int trans);

    // Signal that is emitted when primitive is deselected. The transparency and fuzziness slider
    // will be disabled and the selected primitive is set to 0.
    void primitiveDeselected();

    // Signal that is emitted when a primitive was selected. It sets the selection stat of the
    // primitive to true and enables the transparency and fuzziness slider.
    void primitiveSelected();

    // Starts or stops the coarseness mode.
    void toggleInteraction(bool on);

    // Signal that notifies the transfer function property about a change in transfer function
    // and triggers a repaint of the volumerendering.
    void repaintSignal();

protected:
	ModelData *modelData;					///< model data
	std::vector<TF2DPrimitive*> primitives;	///< primitives the transfer function consists of
    TF2DPrimitive* selectedPrimitive;		///< the current selected primitive
    bool showHistogram;						///< is the histogram visible or not?
    bool showGrid;							///< should a grid displayed in the widget?
    bool histogramLogarithmic;				///< ist the histogram logarithmic or not?
    float histogramBrightness;				///< brightness of the histogram       
    unsigned int histogramTex;				///< texture created out of the histogramdata
	bool histogramReload;					///< is the histogram reloaded
	bool dragging;							///< is the user dragging a primitive or a control point of a primitive?
    Vector2f mouseCoord;					///< position of the mouse in the last move event
    float scaleFactor;						///< scaling factor for y coordinate of primitives
};

#endif