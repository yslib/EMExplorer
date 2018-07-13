#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#include <QDockWidget>
#include <QScrollArea>
#include <QMenu>
#include <QStatusBar>
#include <QMenuBar>
#include <QToolBar>
#include <QAction>
#include <QApplication>
#include "VolumeRenderingWindow.h"

#include "BasicControlWidget.h"
#include "VolumeRenderWidget.h"
#include "TF1DEditor.h"
#include "TF2DEditor.h"
#include "Volume.h"

VolumeRenderingWindow::VolumeRenderingWindow(QWidget *parent)
	: QMainWindow(parent), volumeData(NULL)
{
	//ui.setupUi(this);

	renderWidget = new VolumeRenderWidget(this, this);

	setCentralWidget(renderWidget);

	createActions();
	createMenus();
	createToolBars();
	createStatusBar();
	createDockWindows();
	createConnections();

	setCurrentFile("");
	setUnifiedTitleAndToolBarOnMac(true);

	readSettings();
}

VolumeRenderingWindow::~VolumeRenderingWindow()
{
	writeSettings();
	delete volumeData;
	volumeData = NULL;
}

/*
 * show message in status bar
 */
void VolumeRenderingWindow::showStatusMessage(QString message)
{
	statusBar()->showMessage(message);
}

/*
 * ModelData Interface
 */
Volume* VolumeRenderingWindow::getVolume()
{
	return volumeData;
}

string VolumeRenderingWindow::getVolumeName()
{
	return string(curDataFile.toLocal8Bit().data());
}

void VolumeRenderingWindow::getTransferFunction(float* transferFunction, size_t dimension, float factor)
{
	editorTF1D->getTransferFunction(transferFunction, dimension, factor);
}

unsigned int VolumeRenderingWindow::getTF1DTextureIdx()
{
	return renderWidget->getTF1DIdx();
}

void VolumeRenderingWindow::getTransferFunction(float* transferFunction, size_t width, size_t height, float factor)
{
	editorTF2D->getTransferFunction(transferFunction, width, height, factor);
}

/*
 * Actions
 */
void VolumeRenderingWindow::openVolumeData()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open Volume Data"), curDataFile, tr("Volume Data (*.vifo);;DICOM Data (*.dcm);;All Files (*)"));
	if (!fileName.isEmpty())
		loadVolumeData(fileName);
}

void VolumeRenderingWindow::about()
{
	QMessageBox::about(this, tr("About Application"), tr("Volume Rendering. 2017.7.26"));
}

void VolumeRenderingWindow::createActions()
{
	openAct = new QAction(tr("&Open..."), this);
	openAct->setIcon(QIcon("./Resources/volumeOpen.png"));
	openAct->setShortcuts(QKeySequence::Open);
	openAct->setStatusTip(tr("Open volume data"));
	connect(openAct, SIGNAL(triggered()), this, SLOT(openVolumeData()));

	exitAct = new QAction(tr("E&xit"), this);
	exitAct->setShortcut(tr("Ctrl+X"));

	viewAct = new QAction(tr("&Generate Images"), this);
	viewAct->setIcon(QIcon("./Resources/VPSelect.png"));
	viewAct->setStatusTip(tr("Generate Viewpoint Images"));
	connect(viewAct, SIGNAL(triggered()), renderWidget, SLOT(generateViewpointImages()));

	MIPAct = new QAction(tr("&MIP Shader"), this);
	MIPAct->setShortcut(tr("Ctrl+P"));
	MIPAct->setStatusTip(tr("MIP Shader"));
	MIPAct->setCheckable(true);
	connect(MIPAct, SIGNAL(triggered()), renderWidget, SLOT(setMIPShader()));

	TF1DAct = new QAction(tr("&TF1D Shader"), this);
	TF1DAct->setShortcut(tr("Ctrl+T"));
	TF1DAct->setStatusTip(tr("TF1D Shader"));
	TF1DAct->setCheckable(true);
	connect(TF1DAct, SIGNAL(triggered()), renderWidget, SLOT(setTF1DShader()));

	TF1DShadingAct = new QAction(tr("TF1D &Shading Shader"), this);
	TF1DShadingAct->setShortcut(tr("Ctrl+S"));
	TF1DShadingAct->setStatusTip(tr("TF1D Shading Shader"));
	TF1DShadingAct->setCheckable(true);
	connect(TF1DShadingAct, SIGNAL(triggered()), renderWidget, SLOT(setTF1DLightingShader()));

	TF2DShadingAct = new QAction(tr("TF&2D Shading Shader"), this);
	TF2DShadingAct->setShortcut(tr("Ctrl+2"));
	TF2DShadingAct->setStatusTip(tr("TF2D Shading Shader"));
	TF2DShadingAct->setCheckable(true);
	connect(TF2DShadingAct, SIGNAL(triggered()), renderWidget, SLOT(setTF2DLightingShader()));

	QActionGroup *group = new QActionGroup(this);
	group->addAction(MIPAct);
	group->addAction(TF1DAct);
	group->addAction(TF1DShadingAct);
	group->addAction(TF2DShadingAct);
	TF1DShadingAct->setChecked(true);

	boundingBoxAct = new QAction(QIcon("./Resources/boundingBox.png"), tr("&Bounding Box"), this);
	boundingBoxAct->setCheckable(true);
	boundingBoxAct->setChecked(true);
	boundingBoxAct->setShortcut(tr("Ctrl+B"));
	boundingBoxAct->setStatusTip(tr("Show Bounding Box"));
	connect(boundingBoxAct, SIGNAL(triggered()), renderWidget, SLOT(setBoundingBox()));

	defaultViewAct = new QAction(QIcon("./Resources/defaultView.png"), tr("&Default View"), this);
	defaultViewAct->setShortcut(tr("Ctrl+D"));
	defaultViewAct->setStatusTip(tr("Default View"));
	connect(defaultViewAct, SIGNAL(triggered()), renderWidget, SLOT(defaultView()));

	positiveXViewAct = new QAction(tr("+X View"), this);//
	positiveXViewAct->setStatusTip(tr("Positive X View"));
	connect(positiveXViewAct, SIGNAL(triggered()), renderWidget, SLOT(positiveXView()));

	negativeXViewAct = new QAction(tr("-X View"), this);
	negativeXViewAct->setStatusTip(tr("Negative X View"));
	connect(negativeXViewAct, SIGNAL(triggered()), renderWidget, SLOT(negativeXView()));

	positiveYViewAct = new QAction(tr("+Y View"), this);
	positiveYViewAct->setStatusTip(tr("Positive Y View"));
	connect(positiveYViewAct, SIGNAL(triggered()), renderWidget, SLOT(positiveYView()));

	negativeYViewAct = new QAction(tr("-Y View"), this);
	negativeYViewAct->setStatusTip(tr("Negative Y View"));
	connect(negativeYViewAct, SIGNAL(triggered()), renderWidget, SLOT(negativeYView()));

	positiveZViewAct = new QAction(tr("+Z View"), this);
	positiveZViewAct->setStatusTip(tr("Positive Z View"));
	connect(positiveZViewAct, SIGNAL(triggered()), renderWidget, SLOT(positiveZView()));

	negativeZViewAct = new QAction(tr("-Z View"), this);
	negativeZViewAct->setStatusTip(tr("Negative Z View"));
	connect(negativeZViewAct, SIGNAL(triggered()), renderWidget, SLOT(negativeZView()));

	aboutAct = new QAction(QIcon("./Resources/about.png"), tr("&About"), this);
	aboutAct->setStatusTip(tr("Show the application's About box"));
	connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
}

void VolumeRenderingWindow::createConnections()
{

}

void VolumeRenderingWindow::createMenus()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(openAct);
	fileMenu->addSeparator();
	fileMenu->addAction(exitAct);
	fileMenu->addSeparator();

	viewselMenu = menuBar()->addMenu(tr("&Viewpoint"));
	viewselMenu->addAction(viewAct);

	shaderMenu = menuBar()->addMenu(tr("&Shader"));
	shaderMenu->addAction(MIPAct);
	shaderMenu->addSeparator();
	shaderMenu->addAction(TF1DAct);
	shaderMenu->addAction(TF1DShadingAct);
	shaderMenu->addSeparator();
	shaderMenu->addAction(TF2DShadingAct);
	fileMenu->addSeparator();

	menuBar()->addSeparator();

	viewMenu = menuBar()->addMenu(tr("&View"));
	viewMenu->addAction(defaultViewAct);
	viewMenu->addSeparator();
	viewMenu->addAction(positiveXViewAct);
	viewMenu->addAction(negativeXViewAct);
	viewMenu->addAction(positiveYViewAct);
	viewMenu->addAction(negativeYViewAct);
	viewMenu->addAction(positiveZViewAct);
	viewMenu->addAction(negativeZViewAct);
	viewMenu->addSeparator();
	viewMenu->addSeparator();

	menuBar()->addSeparator();

	helpMenu = menuBar()->addMenu(tr("&Help"));
	helpMenu->addAction(aboutAct);
}

void VolumeRenderingWindow::createToolBars()
{
	QToolBar * toolBar = addToolBar(tr("mainToolBar"));
	toolBar->addAction(openAct);
	toolBar->addAction(viewAct);
	toolBar->addAction(boundingBoxAct);
	toolBar->addAction(defaultViewAct);
	toolBar->addAction(aboutAct);
	
}

void VolumeRenderingWindow::createStatusBar()
{
	statusBar()->showMessage(tr("Ready"));
}

void VolumeRenderingWindow::createDockWindows()
{
	QDockWidget *dock = new QDockWidget(tr("Volume Rendering Parameters"), this);
	dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	controlWidget = new BasicControlWidget(renderWidget, dock);
	dock->setWidget(controlWidget);
	addDockWidget(Qt::RightDockWidgetArea, dock);
	viewMenu->addAction(dock->toggleViewAction());

	dock = new QDockWidget(tr("1D Transfer Function"), this);
	dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	editorTF1D = new TF1DEditor(this, dock, renderWidget);
	dock->setWidget(editorTF1D);
	addDockWidget(Qt::RightDockWidgetArea, dock);
	viewMenu->addAction(dock->toggleViewAction());	
	
	connect(editorTF1D, SIGNAL(TF1DChanged()), renderWidget, SLOT(TF1DChanged()));
	connect(editorTF1D, SIGNAL(toggleInteractionMode(bool)), renderWidget, SLOT(toggleInteractionMode(bool)));

	dock = new QDockWidget(tr("Transfer Function 2D"), this);
	dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	editorTF2D = new TF2DEditor(this, dock, renderWidget);
	dock->setWidget(editorTF2D);
	dock->setVisible(false);
	addDockWidget(Qt::RightDockWidgetArea, dock);
	viewMenu->addAction(dock->toggleViewAction());

	connect(editorTF2D, SIGNAL(TF2DChanged()), renderWidget, SLOT(TF2DChanged()));
	connect(editorTF2D, SIGNAL(toggleInteractionMode(bool)), renderWidget, SLOT(toggleInteractionMode(bool)));
}

void VolumeRenderingWindow::readSettings()
{
	QSettings settings("Trolltech", "VolumeRender");
	QPoint pos   = settings.value("pos", QPoint(100, 100)).toPoint();
	QSize size   = settings.value("size", QSize(1104, 872)).toSize();
	curDataFile  = settings.value("dir", QString("")).toString();
	editorTF1D->setTFFileName(settings.value("TF1D", QString("")).toString());
	editorTF2D->setTFFileName(settings.value("TF2D", QString("")).toString());

	resize(size);
	move(pos);
}

void VolumeRenderingWindow::writeSettings()
{
	QSettings settings("Trolltech", "VolumeRender");
	settings.setValue("pos", pos());
	settings.setValue("size", size());
	settings.setValue("dir", curDataFile);
	settings.setValue("TF1D", editorTF1D->getTFFileName());
	settings.setValue("TF2D", editorTF2D->getTFFileName());
}

void VolumeRenderingWindow::loadVolumeData(const QString &fileName)
{
	if(!volumeData)
		volumeData = new Volume();
	showStatusMessage("Loading volume data ...");
	QApplication::setOverrideCursor(Qt::BusyCursor);

	bool loadSuccess = true;
	if(fileName.endsWith(".vifo", Qt::CaseInsensitive))
		loadSuccess = volumeData->loadRawData(fileName.toLocal8Bit().data());
	else if(fileName.endsWith(".dcm", Qt::CaseInsensitive))
		loadSuccess = volumeData->loadDICOMData(fileName.toLocal8Bit().data());

	if(loadSuccess) {
		controlWidget->updateVolumeInfo(volumeData);
		renderWidget->volumeChanged();
		editorTF1D->update();
		editorTF2D->volumeChanged();
		setCurrentFile(fileName);
	} else {
		showStatusMessage("Loading volume data failed");
	}

	QApplication::restoreOverrideCursor();
	showStatusMessage("Ready");
}

void VolumeRenderingWindow::setCurrentFile(const QString &fileName)
{
	curDataFile = fileName;

	QString shownName;
	if (curDataFile.isEmpty())
		shownName = "";
	else
		shownName = strippedName(curDataFile);

	setWindowTitle(tr("%1[*] - %2").arg("Volume Rendering").arg(shownName));
}

QString VolumeRenderingWindow::strippedName(const QString &fullFileName)
{
	return QFileInfo(fullFileName).fileName();
}