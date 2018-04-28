#include <QRect>
#include <QDockWidget>
#include <QColorDialog>
#include <QMenu>
#include <QAction>
#include <QTreeView>
#include <qglobal.h>
#include <QMessageBox>

#include "imageviewer.h"
#include "ui_mainwindow.h"
#include "mainwindow.h"

QSize imageSize(500, 500);

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	//ui->setupUi(this);
	setWindowTitle(tr("MRC Editor"));
	//Menu [1]
	//File menu
	QMenu *m_fileMenu = menuBar()->addMenu(tr("File"));
	QAction *m_openFileAction = m_fileMenu->addAction("Open..");
	connect(m_openFileAction, SIGNAL(triggered()), this, SLOT(onActionOpenTriggered()));

	QAction *m_saveDataAsAction = m_fileMenu->addAction("Save As..");
	connect(m_saveDataAsAction, SIGNAL(triggered()), this, SLOT(onSaveDataAsActionTriggered()));

	//View menu
	QMenu * viewMenu = menuBar()->addMenu(tr("View"));

	//Dock Widget [2]

	//zoomViewer dock widget

	//m_zoomViewer = new ZoomViwer(this);

	//QDockWidget * dock = new QDockWidget(tr("ZoomViewer"),this);

	//dock->setAllowedAreas(Qt::RightDockWidgetArea);
	//dock->setWidget(m_zoomViewer);
	//addDockWidget(Qt::RightDockWidgetArea,dock);
	//viewMenu->addAction(dock->toggleViewAction());
	//connect(m_zoomViewer, SIGNAL(zoomRegionChanged(const QRectF &)), this, SLOT(onZoomRegionChanged(const QRectF &)));

	//Tree View Model

	//histogram dock widget
//    m_histogram = new Histogram(this);
//    dock = new QDockWidget(tr("Histogram"),this);
//    dock->setAllowedAreas(Qt::RightDockWidgetArea);
//    dock->setWidget(m_histogram);
//    addDockWidget(Qt::RightDockWidgetArea,dock);
//    viewMenu->addAction(dock->toggleViewAction());
//    connect(m_histogram,SIGNAL(minCursorValueChanged(int)),this,SLOT(onMinGrayValueChanged(int)));
//    connect(m_histogram,SIGNAL(maxCursorValueChanged(int)),this,SLOT(onMaxGrayValueChanged(int)));

	m_treeView = new QTreeView(this);
	QDockWidget *dock = new QDockWidget(tr("File Information View"), this);
	dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	dock->setWidget(m_treeView);
	addDockWidget(Qt::LeftDockWidgetArea, dock);
	viewMenu->addAction(dock->toggleViewAction());

	m_treeViewModel = new DataItemModel(QString(), this);
	connect(m_treeView, &QTreeView::doubleClicked, this, &MainWindow::onTreeViewDoubleClicked);
	m_treeView->setModel(m_treeViewModel);

	m_histogramView = new HistogramViewer(this);
	m_histogramView->setModel(m_treeViewModel);

	dock = new QDockWidget(tr("Histgoram"), this);
	dock->setAllowedAreas(Qt::RightDockWidgetArea);
	dock->setWidget(m_histogramView);
	addDockWidget(Qt::RightDockWidgetArea, dock);
	viewMenu->addAction(dock->toggleViewAction());

	///TODO:: m_histogramView
	//connect(m_histogramView,SIGNAL(minValueChanged(int)),this,SLOT(onMinGrayValueChanged(int)));
	//connect(m_histogramView,SIGNAL(maxValueChanged(int)),this,SLOT(onMaxGrayValueChanged(int)));

	//Image Viewer
   // m_imageViewer = new ImageViewer(this);
   // dock = new QDockWidget(tr("Image Viewer"),this);
   // dock->setAllowedAreas(Qt::RightDockWidgetArea);
   // dock->setWidget(m_imageViewer);
   // addDockWidget(Qt::RightDockWidgetArea,dock);
   // viewMenu->addAction(dock->toggleViewAction());


	//Test ImageView
	m_imageView = new ImageView(this);
	//dock = new QDockWidget(tr("Image Viewer"),this);
	//dock->setAllowedAreas(Qt::LeftDockWidgetArea);
	//dock->setWidget(m_imageView);
	//addDockWidget(Qt::LeftDockWidgetArea,dock);
	//viewMenu->addAction(dock->toggleViewAction());
	m_imageView->setModel(m_treeViewModel);
	setCentralWidget(m_imageView);



	//file infomation viwer widget
	//m_fileInfoViewer = new MRCFileInfoViewer(this);
	//dock = new QDockWidget(tr("Files"),this);
	//dock->setAllowedAreas(Qt::LeftDockWidgetArea);
	//dock->setWidget(m_fileInfoViewer);
	//addDockWidget(Qt::LeftDockWidgetArea,dock);
	//connect(m_fileInfoViewer,SIGNAL(currentIndexChanged(int)),this,SLOT(onMRCFilesComboBoxIndexActivated(int)));
	//viewMenu->addAction(dock->toggleViewAction());


	//pixel viewer dock widget
	m_pixelViewer = new PixelViewer(this);
	dock = new QDockWidget(tr("PixelViewer"), this);
	dock->setAllowedAreas(Qt::LeftDockWidgetArea);
	dock->setWidget(m_pixelViewer);
	addDockWidget(Qt::LeftDockWidgetArea, dock);
	viewMenu->addAction(dock->toggleViewAction());
	//m_pixelViewer->setModel(m_treeViewModel);

	//slice viewer

	//m_nestedSliceViewer = new NestedSliceViewer(QSize(),QSize(),QSize(),this);
	//connect(m_nestedSliceViewer, SIGNAL(ZSliderChanged(int)), this, SLOT(onZSliderValueChanged(int)));
	//connect(m_nestedSliceViewer, SIGNAL(YSliderChanged(int)), this, SLOT(onYSliderValueChanged(int)));
	//connect(m_nestedSliceViewer, SIGNAL(XSliderChanged(int)), this, SLOT(onXSliderValueChanged(int)));

	//connect(m_imageView, SIGNAL(ZSliderChanged(int)), this, SLOT(onZSliderValueChanged(int)));
	//connect(m_imageView, SIGNAL(YSliderChanged(int)), this, SLOT(onYSliderValueChanged(int)));
	//connect(m_imageView, SIGNAL(XSliderChanged(int)), this, SLOT(onXSliderValueChanged(int)));

	//connect(m_nestedSliceViewer, SIGNAL(drawingFinished(const QPicture &)), this, SLOT(onSliceViewerDrawingFinished(const QPicture &)));
	//connect(m_nestedSliceViewer,SIGNAL(onMouseMoving(const QPoint &)),m_pixelViewer,SLOT(setPosition(const QPoint &)));

	//ToolBar and Actions [3]
	//open action
	m_actionOpen = new QAction(this);
	m_actionOpen->setText(tr("Open"));
	QToolBar * toolBar = addToolBar(tr("Tools"));
	toolBar->addAction(m_actionOpen);
	connect(m_actionOpen, SIGNAL(triggered()), this, SLOT(onActionOpenTriggered()));
	connect(m_imageView, SIGNAL(zSliceSelected(const QPoint &)), m_pixelViewer, SLOT(setPosition(const QPoint &)));

	//color action
	m_actionColor = new QAction(this);
	m_actionColor->setText(QStringLiteral("Color"));
	toolBar = addToolBar(tr("Tools"));
	toolBar->addAction(m_actionColor);
	connect(m_actionColor, SIGNAL(triggered(bool)), this, SLOT(onColorActionTriggered()));

	//mark action
	//QAction * actionMark = new QAction(this);
	//actionMark->setText(QStringLiteral("Mark"));
	//actionMark->setCheckable(true);
	//toolBar->addAction(actionMark);

	//connect(actionMark, SIGNAL(triggered(bool)), m_nestedSliceViewer, SLOT(paintEnable(bool)));

	//save mark action
	QAction * actionSaveMark = new QAction(this);
	actionSaveMark->setText(QStringLiteral("Save Mark"));
	toolBar->addAction(actionSaveMark);
	connect(actionSaveMark, SIGNAL(triggered()), this, SLOT(onSaveActionTriggered()));

	//save data as action
	QAction * actionSaveDataAs = new QAction(this);
	actionSaveDataAs->setText(QStringLiteral("Save Data As"));
	toolBar->addAction(actionSaveDataAs);
	connect(actionSaveDataAs, SIGNAL(triggered()), this, SLOT(onSaveDataAsActionTriggered()));

	//Status bar


	m_currentContext = -1;
	allControlWidgetsEnable(false);

}
MainWindow::~MainWindow()
{
	_destroy();
	delete ui;
}

void MainWindow::onActionOpenTriggered()
{
	//if(m_currentContext != -1){
	//    QMessageBox::critical(this,"Error",
	//                          tr("Multi files aren't supported now\n"),
	//                          QMessageBox::Yes,QMessageBox::Yes);
	//    return;
	//}
	QString fileName = QFileDialog::getOpenFileName(this, tr("OpenFile"), tr("/Users/Ysl/Downloads/ETdataSegmentation"), tr("mrc Files(*.mrc *mrcs)"));
	if (fileName == "") {
		return;
	}
	//m_mrcs.push_back(MRC(fileName));
	//ItemContext mrcModel(fileName);
	QString name = fileName.mid(fileName.lastIndexOf('/') + 1);

	//QString headerInfo = mrcModel.getMRCInfo();


	///TODO:: TEST,insert row into TreeView 
	//m_treeViewModel->addItemHelper(fileName, headerInfo);

	QSharedPointer<ItemContext> sharedItem(new ItemContext(fileName));
	m_treeViewModel->addItem(sharedItem);

	//QSharedPointer<MRC> sharedMRC(new MRC(fileName.toStdString()));
	//m_treeViewModel->addItem(sharedMRC);
	//
	//m_fileInfoViewer->setText(headerInfo);
	//addMRCDataModel(std::move(mrcModel));
	//int newCurrentContext = m_mrcDataModels.size() - 1;
	//m_fileInfoViewer->addItem(name,newCurrentContext);


//      if (m_fileInfoViewer->count() != 1) {
	  //	//If there have already been items we need to select the 
	  //	//newest item manually
//          m_fileInfoViewer->setCurrentIndex(m_fileInfoViewer->count()-1);
	  //	saveMRCDataModel();
	  //	setMRCDataModel(newCurrentContext);
	  //}
	  //else {
	  //	///Do nothing
	  //	//If there was not item before, it will emit activated() signal
	  //	//after addItem()
	  //}

}

void MainWindow::onMRCFilesComboBoxIndexActivated(int index)
{
	//Find first item to change
	//QVariant userData =m_fileInfoViewer->itemData(index);
	//if(userData.canConvert(QVariant::Int) == false)
	//    return;
	//int context = userData.toInt();
	//saveMRCDataModel();
	//setMRCDataModel(context);
}

void MainWindow::addMRCDataModel(const ItemContext & model)
{
	m_mrcDataModels.push_back(model);
}

void MainWindow::addMRCDataModel(ItemContext && model)
{
	m_mrcDataModels.push_back(std::move(model));
}

/*
 * This function is to set the properties of
 * control widgets according to context
*/
void MainWindow::setMRCDataModel(int index)
{
	m_currentContext = index;

	const ItemContext & model = m_mrcDataModels[m_currentContext];

	/*sliceSlider's*/
	int maxSliceIndex = model.getTopSliceCount() - 1;
	int currentSliceIndex = model.getCurrentSliceIndex();

	int maxRightSliceIndex = model.getRightSliceCount() - 1;
	int maxFrontSliceIndex = model.getFrontSliceCount() - 1;

	//m_nestedSliceViewer->setMaximumImageCount(maxSliceIndex,maxRightSliceIndex,maxFrontSliceIndex);

	/*Max Gray Slider and SpinBox*/
	///TODO::maybe the tow values are useless
	int minGrayscaleValue = model.getMinGrayscale();     //Usually 255
	int maxGrayscaleValue = model.getMaxGrayscale();
	int grayscaleStrechingLowerBound = model.getGrayscaleStrechingLowerBound();

	///TODO::m_histogramView
	//m_histogramView->setLeftCursorValue(grayscaleStrechingLowerBound);

	/*Min Gray Slider and SpinBox*/
	int grayscaleStrechingUpperBound = model.getGrayscaleStrechingUpperBound();
	//TODO::m_histogramView
	//m_histogramView->setRightCursorValue(grayscaleStrechingUpperBound);

	const QImage & image = model.getSlice(currentSliceIndex);

	//ImageView
	//m_imageView->setTopImage(image);
	//m_imageView->setRightImage(model.getRightSlice(0));
	//m_imageView->setFrontImage(model.getFrontSlice(0));

	int topSliceCount = model.getTopSliceCount();
	int rightSliceCount = model.getRightSliceCount();
	int frontSliceCount = model.getFrontSliceCount();

	//m_imageView->setTopSliceCount(topSliceCount-1);
	//m_imageView->setRightSliceCount(rightSliceCount-1);
	//m_imageView->setFrontSliceCount(frontSliceCount-1);


	/*Histogram*/


	QRect region = model.getZoomRegion();
	//m_histogram->setImage(image);

	///TODO::m_histgoramView
	//m_histogramView->setImage(image);


	/*ZoomViwer*/
	//m_zoomViewer->setImage(image,region);
	/*There should be a image scale region context to be restored*/
	//m_sliceViewer->setImage(image,region);

	//m_nestedSliceViewer->setImage(image,region);
	//m_nestedSliceViewer->setRightImage(model.getRightSlice(0));
	//m_nestedSliceViewer->setFrontImage(model.getFrontSlice(0));


	//m_nestedSliceViewer->setMarks(model.getMarks(currentSliceIndex));

	/*PixelViewer*/
	//m_pixelViewer->setImage(image);

	/*Set all widgets enable*/
	allControlWidgetsEnable(true);


}

void MainWindow::saveMRCDataModel()
{
	if (m_currentContext == -1)
		return;

	//Save previous context
	ItemContext & model = m_mrcDataModels[m_currentContext];

	//int sliceIndex = m_nestedSliceViewer->getZSliceValue();
	//int sliceIndex = m_imageView->getZSliceValue();


	//model.setCurrentSliceIndex(sliceIndex);

	//TODO::m_histogramView
	//model.setGrayscaleStrechingLowerBound(m_histogramView->getLeftCursorValue());
	//model.setGrayScaleStrechingUpperBound(m_histogramView->getRightCursorValue());

	//model.setZoomRegion(m_zoomViewer->zoomRegion().toRect());
}

void MainWindow::deleteMRCDataModel(int index)
{

}

void MainWindow::allControlWidgetsEnable(bool enable)
{
	//TODO::m_histogramView
	//m_histogramView->setEnabled(enable);
}



void MainWindow::updateGrayThreshold(int lower, int upper)
{
	//   size_t width = m_mrcDataModels[m_currentContext].getWidth();
	//   size_t height = m_mrcDataModels[m_currentContext].getHeight();

	//   //QImage originalImage = m_mrcDataModels[m_currentContext].getOriginalSlice(m_nestedSliceViewer->getZSliceValue());
	//   QImage originalImage = m_mrcDataModels[m_currentContext].getOriginalSlice(m_imageView->getZSliceValue());

	   //unsigned char *image = originalImage.bits();

	   //qreal k = 256.0 / static_cast<qreal>(upper - lower);
	   //QImage strechingImage(width, height, QImage::Format_Grayscale8);
	   //unsigned char * data = strechingImage.bits();
	//   for(int j=0;j<height;j++){
	//       for(int i=0;i<width;i++){
	//           int index = i+j*width;
	   //		unsigned char pixelGrayValue = image[index];
	   //		unsigned char clower = static_cast<unsigned char>(lower);
	   //		unsigned char cupper = static_cast<unsigned char>(upper);
	   //		if (pixelGrayValue < clower) {
	   //			data[index] = 0;
	   //		}
	   //		else if (pixelGrayValue>cupper) {
	   //			data[index] = 255;
	   //		}
	   //		else {
	   //			data[index] = static_cast<unsigned char>(pixelGrayValue* k+0.5);
	   //		}
	//       }
	//   }
	   //m_sliceViewer->setImage(strechingImage);
	   //m_mrcDataModels[m_currentContext].setSlice(strechingImage,m_nestedSliceViewer->getZSliceValue());
	   //m_mrcDataModels[m_currentContext].setSlice(strechingImage,m_imageView->getZSliceValue());
}
/*
 * This function only sets the initial ui layout,
 * and it doesn't set their properties.
*/
void MainWindow::_initUI()
{

}
/*
 * This function sets all the connections
 * between signal and slot except for the actions'
*/
void MainWindow::_connection()
{

}

void MainWindow::_destroy()
{
	//Nothing need to be destroyed
}

void MainWindow::onMaxGrayValueChanged(int position)
{
	//int minv = m_histogram->getMinimumCursorValue();
	//int maxv = m_histogram->getMaximumCursorValue();
	int minv = m_histogramView->getLeftCursorValue();
	int maxv = m_histogramView->getRightCursorValue();


	//qDebug()<<minv<<" "<<maxv;
	updateGrayThreshold(minv, maxv);

	//QRect rect = m_zoomViewer->zoomRegion().toRect();

	/*int slice = m_nestedSliceViewer->getZSliceValue();
	m_nestedSliceViewer->setImage(m_mrcDataModels[m_currentContext].getSlice(slice),rect);
	m_imageView->setFrontImage(m_mrcDataModels[m_currentContext].getSlice(slice));
	m_zoomViewer->setImage(m_mrcDataModels[m_currentContext].getSlice(slice),rect);*/

	//int slice = m_imageView->getZSliceValue();
	//m_imageView->setFrontImage(m_mrcDataModels[m_currentContext].getSlice(slice));
}

void MainWindow::onMinGrayValueChanged(int position)
{
	//int minv = m_histogram->getMinimumCursorValue();
	//int maxv = m_histogram->getMaximumCursorValue();
	int minv = m_histogramView->getLeftCursorValue();
	int maxv = m_histogramView->getRightCursorValue();
	//qDebug()<<minv<<" "<<maxv;
	updateGrayThreshold(minv, maxv);
	//QRect rect = m_zoomViewer->zoomRegion().toRect();

	//m_nestedSliceViewer->setImage(m_mrcDataModels[m_currentContext].getSlice(m_nestedSliceViewer->getZSliceValue()),rect);
	//m_imageView->setFrontImage(m_mrcDataModels[m_currentContext].getSlice(m_nestedSliceViewer->getZSliceValue()));
	//m_zoomViewer->setImage(m_mrcDataModels[m_currentContext].getSlice(m_nestedSliceViewer->getZSliceValue()),rect);

	//int slice = m_imageView->getZSliceValue();
	//m_imageView->setFrontImage(m_mrcDataModels[m_currentContext].getSlice(slice));

}

void MainWindow::onSliceValueChanged(int value)
{
}

void MainWindow::onZSliderValueChanged(int value)
{
	//QRectF regionf = m_zoomViewer->zoomRegion();
	//QRect region = QRect(regionf.left(),regionf.top(),regionf.width(),regionf.height());
	//qDebug() << "onSliceValueChanged(int):" << region;
	//m_nestedSliceViewer->setImage(m_mrcDataModels[m_currentContext].getSlice(value),region);
	//m_nestedSliceViewer->setMarks(m_mrcDataModels[m_currentContext].getMarks(value));
	//
	//m_histogram->setImage(m_mrcDataModels[m_currentContext].getSlice(value));

	//TODO::m_histogramView
   // m_histogramView->setImage(m_mrcDataModels[m_currentContext].getSlice(value));


	//m_zoomViewer->setImage(m_mrcDataModels[m_currentContext].getSlice(value),region);
	//m_imageView->setTopImage(m_mrcDataModels[m_currentContext].getSlice(value));
}

void MainWindow::onYSliderValueChanged(int value)
{
	//m_nestedSliceViewer->setFrontImage(m_mrcDataModels[m_currentContext].getFrontSlice(value));
	//m_imageView->setFrontImage(m_mrcDataModels[m_currentContext].getFrontSlice(value));
}

void MainWindow::onXSliderValueChanged(int value)
{
	//m_nestedSliceViewer->setRightImage(m_mrcDataModels[m_currentContext].getFrontSlice(value));
	//m_imageView->setRightImage(m_mrcDataModels[m_currentContext].getRightSlice(value));
}


void MainWindow::onZoomDoubleSpinBoxValueChanged(double d)
{

}

/*
 *
*/
void MainWindow::onZoomRegionChanged(const QRectF &region)
{
	//int slice = m_nestedSliceViewer->getZSliceValue();
	//QImage image = m_mrcDataModels[m_currentContext].getSlice(slice);
	//m_nestedSliceViewer->setImage(image,region.toRect());
}

void MainWindow::onSliceViewerDrawingFinished(const QPicture & p)
{
	//int slice = m_nestedSliceViewer->getZSliceValue();
	//m_mrcDataModels[m_currentContext].addMark(slice,p);
	//m_nestedSliceViewer->setMarks(m_mrcDataModels[m_currentContext].getMarks(slice));
}

void MainWindow::onColorActionTriggered()
{
	//qDebug() << "ColorActionTriggered";
	QColor color = QColorDialog::getColor(Qt::black, this, QStringLiteral("Color Selection"));
	//m_nestedSliceViewer->setMarkColor(color);
	//m_imageView->setColor(color);
}

void MainWindow::onSaveActionTriggered()
{
	QString fileName = QFileDialog::getSaveFileName(this, QString("Mark Save"),
		"", QString("Raw Files(*.raw);;MRC Files(*.mrc)"));
	if (fileName.isEmpty() == true)
		return;
	if (fileName.endsWith(QString(".raw")) == true) {
		bool ok = m_mrcDataModels[m_currentContext].saveMarks(fileName, ItemContext::MarkFormat::RAW);
		if (ok == false) {
			QMessageBox::critical(this,
				QStringLiteral("Error"),
				QStringLiteral("Can not save this marks"),
				QMessageBox::Ok, QMessageBox::Ok);
		}
	}
	else if (fileName.endsWith(QString(".mrc")) == true) {
		bool ok = m_mrcDataModels[m_currentContext].saveMarks(fileName, ItemContext::MarkFormat::MRC);
		if (ok == false) {
			QMessageBox::critical(this,
				QStringLiteral("Error"),
				QStringLiteral("Can not save this marks"),
				QMessageBox::Ok, QMessageBox::Ok);
		}
	}
}

void MainWindow::onSaveDataAsActionTriggered()
{
	QString fileName = QFileDialog::getSaveFileName(this,
		QStringLiteral("Save Data As"),
		".", QString("mrc File(*.mrc);;raw File(*.raw)"));
	if (fileName.isEmpty() == true)
		return;
	if (fileName.endsWith(".raw") == true) {
		m_mrcDataModels[m_currentContext].save(fileName);
	}
	else if (fileName.endsWith(".mrc") == true) {
		m_mrcDataModels[m_currentContext].save(fileName);
	}
}

void MainWindow::onTreeViewDoubleClicked(const QModelIndex & index)
{
	///TODO::
	QModelIndex parent;
	QModelIndex rootItem = index;
	while ((parent = m_treeViewModel->parent(rootItem)).isValid())
		rootItem = parent;
	m_histogramView->activateItem(rootItem);

	m_imageView->activateItem(rootItem);

	m_pixelViewer->activateItem(rootItem);
}
void MainWindow::createDockWindows()
{

}
