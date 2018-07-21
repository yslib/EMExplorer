#include <QBoxLayout>
#include <QCheckBox>
#include <QFileDialog>
#include <QGLContext>
#include <QLabel>
#include <QMessageBox>
#include <QSlider>
#include <QSplitter>
#include <QToolButton>
#include "TF2DEditor.h"
#include "TF2DPrimitive.h"
#include "TF2DMappingCanvas.h"

TF2DEditor::TF2DEditor(ModelData *model, QWidget* parent)
    : QWidget(parent)
    , transCanvas(0)
    , maximumIntensity(255)
{
	createWidgets(model);
    createConnections();
}

TF2DEditor::~TF2DEditor() 
{
}

void TF2DEditor::createWidgets(ModelData *model) 
{
	// Creates the layout with all buttons
    QBoxLayout* buttonLayout = new QHBoxLayout();
	buttonLayout->setSpacing(0);

    clearButton = new QToolButton();
    clearButton->setIcon(QIcon("./Resources/TFClear.png"));
    clearButton->setToolTip(tr("Reset transfer function to default"));

    loadButton = new QToolButton();
    loadButton->setIcon(QIcon("./Resources/TFOpen.png"));
    loadButton->setToolTip(tr("Load transfer function"));

    saveButton = new QToolButton();
    saveButton->setIcon(QIcon("./Resources/TFSave.png"));
    saveButton->setToolTip(tr("Save transfer function"));

    gridEnabledButton = new QToolButton();
    gridEnabledButton->setCheckable(true);
    gridEnabledButton->setChecked(false);
    gridEnabledButton->setIcon(QIcon("./Resources/grid.png"));
    gridEnabledButton->setToolTip(tr("Show grid"));

    histogramEnabledButton = new QToolButton();
    histogramEnabledButton->setCheckable(true);
    histogramEnabledButton->setChecked(false);
    histogramEnabledButton->setEnabled(false);
    histogramEnabledButton->setIcon(QIcon("./Resources/histogram.png"));
    histogramEnabledButton->setToolTip(tr("Show data histogram"));

	histogramLogEnabledButton = new QToolButton();
    histogramLogEnabledButton->setCheckable(true);
    histogramLogEnabledButton->setChecked(true);
    histogramLogEnabledButton->setEnabled(false);
    histogramLogEnabledButton->setIcon(QIcon("./Resources/histogramLog.png"));
    histogramLogEnabledButton->setToolTip(tr("Logarithmic histogram"));

    quadButton = new QToolButton();
    quadButton->setIcon(QIcon("./Resources/quad.png"));
    quadButton->setToolTip(tr("Add a quad"));

    bananaButton = new QToolButton();
    bananaButton->setIcon(QIcon(".Resources/banana.png"));
    bananaButton->setToolTip(tr("Add a banana"));

    deleteButton = new QToolButton();
    deleteButton->setIcon(QIcon("./Resources/eraser.png"));
    deleteButton->setToolTip(tr("Delete selected primitive"));

    colorButton = new QToolButton();
    colorButton->setIcon(QIcon("./Resources/colorize.png"));
    colorButton->setToolTip(tr("Change the color of the selected primitive"));

    buttonLayout->addWidget(clearButton);
    buttonLayout->addWidget(loadButton);
    buttonLayout->addWidget(saveButton);
    buttonLayout->addSpacing(7);
    buttonLayout->addWidget(gridEnabledButton);
    buttonLayout->addWidget(histogramEnabledButton);
	buttonLayout->addWidget(histogramLogEnabledButton);
    buttonLayout->addSpacing(7);
    buttonLayout->addWidget(quadButton);
    buttonLayout->addWidget(bananaButton);
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addWidget(colorButton);
    buttonLayout->addStretch();

	// Creates the layout with mapping canvas
    transCanvas = new TF2DMappingCanvas(model, 0);
    transCanvas->setMinimumSize(256, 128);

	// Creates the layout with transparency and fuzzines slider and control elements for the histogram
	QVBoxLayout* sliderLayout = new QVBoxLayout();
	
    histogramBrightness = new QSlider(Qt::Horizontal);
    histogramBrightness->setEnabled(false);
    histogramBrightness->setMinimum(10);
    histogramBrightness->setMaximum(1000);
    histogramBrightness->setValue(100);
	QHBoxLayout* hBoxLayout = new QHBoxLayout();
	hBoxLayout->addWidget(new QLabel(tr("Histogram Brightness:")));
    hBoxLayout->addWidget(histogramBrightness);
	sliderLayout->addLayout(hBoxLayout);

    transparency = new QSlider(Qt::Horizontal);
    transparency->setEnabled(false);
    transparency->setMinimum(0);
    transparency->setMaximum(255);
	hBoxLayout = new QHBoxLayout();
    hBoxLayout->addWidget(new QLabel(tr("Transparency:")));
    hBoxLayout->addWidget(transparency);
	sliderLayout->addLayout(hBoxLayout);

    fuzziness = new QSlider(Qt::Horizontal);
    fuzziness->setEnabled(false);
    fuzziness->setMinimum(0);
    fuzziness->setMaximum(100);
	hBoxLayout = new QHBoxLayout();
    hBoxLayout->addWidget(new QLabel(tr("Fuzziness:")));
    hBoxLayout->addWidget(fuzziness);
	sliderLayout->addLayout(hBoxLayout);

    QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->addLayout(buttonLayout);
	mainLayout->addWidget(transCanvas, 1);
	mainLayout->addLayout(sliderLayout);
    setLayout(mainLayout);
}

void TF2DEditor::createConnections()
{
    // buttons
    connect(loadButton,  SIGNAL(clicked()), this, SLOT(loadTransferFunction()));
    connect(saveButton,  SIGNAL(clicked()), this, SLOT(saveTransferFunction()));
    connect(clearButton, SIGNAL(clicked()), transCanvas, SLOT(resetTransferFunction()));

    connect(gridEnabledButton,      SIGNAL(clicked()), this, SLOT(toggleShowGrid()));
    connect(histogramEnabledButton, SIGNAL(clicked()), this, SLOT(toggleShowHistogram()));
	connect(histogramLogEnabledButton, SIGNAL(clicked()), this, SLOT(toggleHistogramLogarithmic()));

    connect(quadButton,   SIGNAL(clicked()), transCanvas, SLOT(addQuadPrimitive()));
    connect(bananaButton, SIGNAL(clicked()), transCanvas, SLOT(addBananaPrimitive()));
    connect(deleteButton, SIGNAL(clicked()), transCanvas, SLOT(deletePrimitive()));
    connect(colorButton,  SIGNAL(clicked()), transCanvas, SLOT(colorizePrimitive()));

    connect(histogramBrightness, SIGNAL(sliderMoved(int)), transCanvas, SLOT(histogramBrightnessChanged(int)));
    
    // slider
    connect(fuzziness, SIGNAL(valueChanged(int)), transCanvas, SLOT(fuzzinessChanged(int)));
    connect(transparency, SIGNAL(valueChanged(int)), transCanvas, SLOT(transparencyChanged(int)));

    connect(fuzziness, SIGNAL(sliderPressed()), this, SLOT(startTracking()));
    connect(transparency, SIGNAL(sliderPressed()), this, SLOT(startTracking()));

    connect(fuzziness, SIGNAL(sliderReleased()), this, SLOT(stopTracking()));
    connect(transparency, SIGNAL(sliderReleased()), this, SLOT(stopTracking()));

    connect(transCanvas, SIGNAL(setTransparencySlider(int)), this, SLOT(setTransparency(int)));
    connect(transCanvas, SIGNAL(primitiveDeselected()), this, SLOT(primitiveDeselected()));
    connect(transCanvas, SIGNAL(primitiveSelected()), this, SLOT(primitiveSelected()));
    connect(transCanvas, SIGNAL(toggleInteraction(bool)), this, SLOT(toggleInteraction(bool)));
    connect(transCanvas, SIGNAL(repaintSignal()), this, SLOT(repaintSignal()));
}

void TF2DEditor::volumeChanged()
{
    // update control elements
    histogramEnabledButton->setEnabled(true);
    histogramEnabledButton->blockSignals(true);
    histogramEnabledButton->setChecked(false);
    histogramEnabledButton->blockSignals(false);
	histogramLogEnabledButton->setEnabled(false);
    histogramBrightness->setEnabled(false);
    histogramBrightness->blockSignals(true);
    histogramBrightness->setValue(100);
    histogramBrightness->blockSignals(false);

    maximumIntensity = 255;

    // propagate volume to painter where the histogram is calculated
	transCanvas->volumeChanged();
}

void TF2DEditor::getTransferFunction(float* transferFunction, size_t width, size_t height, float factor)
{
	transCanvas->getTransferFunction(transferFunction, width, height, factor);
}

void TF2DEditor::loadTransferFunction() 
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open TF2D"), curTFFile, tr("TF2D (*.TF2D);;All Files (*)"));
	if (!fileName.isEmpty()) {
		transCanvas->load(fileName.toLocal8Bit().data());
		curTFFile = QFileInfo(fileName).absolutePath();
	}
}

void TF2DEditor::saveTransferFunction() 
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save TF2D"), curTFFile, tr("TF2D (*.TF2D);;All Files (*)"));
	if (!fileName.isEmpty()) {
		transCanvas->save(fileName.toLocal8Bit().data());
		curTFFile = QFileInfo(fileName).absolutePath();
	}
}

void TF2DEditor::toggleShowGrid() 
{
    transCanvas->setGridVisible(gridEnabledButton->isChecked());
}

void TF2DEditor::toggleShowHistogram()
{
    transCanvas->setHistogramVisible(histogramEnabledButton->isChecked());
    histogramBrightness->setEnabled(histogramEnabledButton->isChecked());
    histogramLogEnabledButton->setEnabled(histogramEnabledButton->isChecked());
    transCanvas->update();
}

void TF2DEditor::toggleHistogramLogarithmic()
{
	transCanvas->setHistogramLogarithmic(histogramLogEnabledButton->isChecked());
}

void TF2DEditor::primitiveSelected() 
{
    const TF2DPrimitive* p = transCanvas->getSelectedPrimitive();
    fuzziness->setValue(static_cast<int>(p->getFuzziness() * 100.f));
    fuzziness->setEnabled(true);
    transparency->setValue(p->getColor().w * 255);
    transparency->setEnabled(true);
}

void TF2DEditor::primitiveDeselected() 
{
    fuzziness->setValue(0);
    fuzziness->setEnabled(false);
    transparency->setValue(0);
    transparency->setEnabled(false);
}

void TF2DEditor::setTransparency(int trans) 
{
    transparency->blockSignals(true);
    transparency->setValue(trans);
    transparency->blockSignals(false);
}

void TF2DEditor::startTracking() 
{
    toggleInteractionMode(true);
}

void TF2DEditor::stopTracking() 
{
    toggleInteractionMode(false);
}

void TF2DEditor::toggleInteraction(bool on)
{
	emit toggleInteractionMode(on);	
}

void TF2DEditor::repaintSignal() 
{
    emit TF2DChanged();
}