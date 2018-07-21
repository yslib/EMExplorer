#ifndef TF2DEDITOR_H
#define TF2DEDITOR_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QCheckBox;
class QLayout;
class QSlider;
class QToolButton;
QT_END_NAMESPACE

class ModelData;
class TF2DMappingCanvas;

class TF2DEditor : public QWidget {
    Q_OBJECT
public:
    // Constructor
    TF2DEditor(ModelData *model, QWidget* parent = 0);

    // Destructor    
    ~TF2DEditor();

    // Creates the whole layout for this widget.
    void createWidgets(ModelData *model);

    // Creates the connections for all control elements.
    void createConnections();

	// Called when the rendered volume changes.
	void volumeChanged();

	// Get the 2D transfer function
	void getTransferFunction(float* transferFunction, size_t width, size_t height, float factor);

	// Set the previous TF file name
	void setTFFileName(QString filename) { curTFFile = filename; }

	// Get the previous TF file name
	QString getTFFileName() { return curTFFile; }

public slots:
    // Opens a filedialog and loads the selected transfer function.
    void loadTransferFunction();

    // Opens a filedialog and saves the current transfer function to the desired file.
    void saveTransferFunction();

    // Enables or disables the grid in the background.
    void toggleShowGrid();

    // Enables or disables the display of the histogram texture.
    void toggleShowHistogram();

	// Enables or disables the logarithmic display of the histogram.
	void toggleHistogramLogarithmic();

    // Slot that is called when a primitive was selected. It enables the transparency
    // and fuzzines slider.
    void primitiveSelected();

    // Slot that is called when a primitive was deselected. The transparency and fuzzines
    // slider are disabled.
    void primitiveDeselected();

    // Sets the transparency slider to given value.
    void setTransparency(int trans);

    // Activates the coarseness mode.
    void startTracking();

    // Deactivates the coarseness mode.
    void stopTracking();
	
	// Starts or stops the interaction mode.
    void toggleInteraction(bool on);

    // Informs the transfer function property about a change in transfer function
    // and emits transferFunctionChanged() signal.
    void repaintSignal();

signals:
    // Signal that is emited when the volume rendering needs to be repainted.
    void TF2DChanged();

	// Signal that is emitted when the user drags a key or a line.
	// It turns coarseness mode on or off.
    void toggleInteractionMode(bool on);

protected:
	TF2DMappingCanvas* transCanvas;			///< the 2D transfer function and the canvas in which the TF can be edited

    QToolButton* loadButton;				///< button for loading of a 2D tf
    QToolButton* saveButton;				///< button for saving of a 2D tf
    QToolButton* clearButton;				///< button for resetting of the tf
    QToolButton* gridEnabledButton;			///< button for enabling and disabling the grid
    QToolButton* histogramEnabledButton;	///< button for enabling and disabling the histogram
    QToolButton* histogramLogEnabledButton;	///< checkbox for enabling and disabling the logarithmic display of the histogram
	QToolButton* quadButton;				///< button for adding a quad primitive
    QToolButton* bananaButton;				///< button for adding a banana primitive
    QToolButton* deleteButton;				///< button for deletion of current selected primitive
    QToolButton* colorButton;				///< button for opening of a colordialog for the current selected primitive
    QSlider* histogramBrightness;			///< slider for adjusting the brightness of the histogram
    QSlider* fuzziness;						///< slider for adjusting the fuzzines of a primitive
    QSlider* transparency;					///< slider for adjusting the transparency of a primitive

    int maximumIntensity;					///< maximum intensity that can occur in the current rendered volume

	QString curTFFile;
};

#endif