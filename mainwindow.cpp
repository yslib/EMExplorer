#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPen>
#include <QRect>
#include <QDockWidget>
#include <QMenu>
#include <QAction>

QSize imageSize(500,500);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	setWindowTitle(tr("MRC Editor"));
    QMenu *m_fileMenu = menuBar()->addMenu(tr("File"));
    QAction *m_openFileAction = m_fileMenu->addAction("Open..");
    QAction *m_saveDataAsAction = m_fileMenu->addAction("Save As..");



    connect(m_openFileAction,SIGNAL(triggered()),this,SLOT(onActionOpenTriggered()));
    connect(m_saveDataAsAction,SIGNAL(triggered()),this,SLOT(onSaveDataAsActionTriggered()));

    m_zoomViewer = new ZoomViwer(this);
    QDockWidget * dock = new QDockWidget(tr("ZoomViewer"),this);
    dock->setAllowedAreas(Qt::RightDockWidgetArea);
    dock->setWidget(m_zoomViewer);
    addDockWidget(Qt::RightDockWidgetArea,dock);

    m_histogram = new Histogram(this);
    dock = new QDockWidget(tr("Histogram"),this);
    dock->setAllowedAreas(Qt::RightDockWidgetArea);
    dock->setWidget(m_histogram);
    addDockWidget(Qt::RightDockWidgetArea,dock);

    m_nestedSliceViewer = new NestedSliceViewer(this);
    dock = new QDockWidget(tr("SliceViewer"),this);
    dock->setAllowedAreas(Qt::RightDockWidgetArea);
    dock->setWidget(m_nestedSliceViewer);
    addDockWidget(Qt::RightDockWidgetArea,dock);

	connect(m_nestedSliceViewer, SIGNAL(ZSliderChanged(int)), this, SLOT(onZSliderValueChanged(int)));
	connect(m_nestedSliceViewer, SIGNAL(YSliderChanged(int)), this, SLOT(onYSliderValueChanged(int)));
	connect(m_nestedSliceViewer, SIGNAL(XSliderChanged(int)), this, SLOT(onXSliderValueChanged(int)));



    _initUI();
	_connection();
    createDockWindows();
    //QDockWidget Test

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
    QString fileName =QFileDialog::getOpenFileName(this,tr("OpenFile"),tr("/Users/Ysl/Downloads/ETdataSegmentation"),tr("mrc Files(*.mrc *mrcs)"));
    if(fileName == ""){
        return;
    }
    //m_mrcs.push_back(MRC(fileName));
    MRCDataModel mrcModel(fileName);
    QString name = fileName.mid(fileName.lastIndexOf('/')+1);
    if(mrcModel.isOpened() == false){
        QMessageBox::critical(NULL, "Error", tr("Can't open this file.\n%1").arg(fileName),
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    }else{
        QString headerInfo = mrcModel.getMRCInfo();
        //
        m_fileInfoViewer->setText(headerInfo);
        _addMRCDataModel(std::move(mrcModel));
        int newCurrentContext = m_mrcDataModels.size() - 1;
        m_fileInfoViewer->addItem(name,newCurrentContext);


        if (m_fileInfoViewer->count() != 1) {
			//If there have already been items we need to select the 
			//newest item manually
            m_fileInfoViewer->setCurrentIndex(m_fileInfoViewer->count()-1);
			_saveMRCDataModel();
			_setMRCDataModel(newCurrentContext);
		}
		else {
			///Do nothing
			//If there was not item before, it will emit activated() signal
			//after addItem()
		}
		qDebug() << "NewCurrentContextId:" << newCurrentContext;
    }

}

void MainWindow::onMRCFilesComboBoxIndexActivated(int index)
{
	//Find first item to change
    QVariant userData =m_fileInfoViewer->itemData(index);
    if(userData.canConvert(QVariant::Int) == false)
        return;
    int context = userData.toInt();
    _saveMRCDataModel();
    _setMRCDataModel(context);
}

void MainWindow::_addMRCDataModel(const MRCDataModel & model)
{
	m_mrcDataModels.push_back(model);
}

void MainWindow::_addMRCDataModel(MRCDataModel && model)
{
	m_mrcDataModels.push_back(std::move(model));
}

/*
 * This function is to set the properties of
 * control widgets according to context
*/
void MainWindow::_setMRCDataModel(int index)
{
    m_currentContext = index;

    const MRCDataModel & model = m_mrcDataModels[m_currentContext];

    /*sliceSlider's*/
    int maxSliceIndex = model.getSliceCount() - 1;
    int currentSliceIndex = model.getCurrentSlice();
    m_sliceSlider->setMaximum(maxSliceIndex);
	m_sliceSlider->setMinimum(0);
    m_sliceSlider->setValue(currentSliceIndex);

    /*sliceSpinBox's*/
    m_sliceSpinBox->setMaximum(maxSliceIndex);
	m_sliceSpinBox->setMinimum(0);
    m_sliceSpinBox->setValue(currentSliceIndex);

	/*Max Gray Slider and SpinBox*/
    int minGrayscaleValue = model.getMinGrayscale();     //Usually 255
    int maxGrayscaleValue = model.getMaxGrayscale();
    int grayscaleStrechingLowerBound = model.getGrayscaleStrechingLowerBound();

    //m_histMinSlider->setMinimum(minGrayscaleValue);
    //m_histMinSlider->setMaximum(maxGrayscaleValue);
    //m_histMinSlider->setValue(grayscaleStrechingLowerBound);
    //m_histMinSpinBox->setMinimum(minGrayscaleValue);
    //m_histMinSpinBox->setMaximum(maxGrayscaleValue);
    //m_histMinSpinBox->setValue(grayscaleStrechingLowerBound);
    m_histogram->setMinimumCursorValue(grayscaleStrechingLowerBound);

	/*Min Gray Slider and SpinBox*/
    int grayscaleStrechingUpperBound = model.getGrayscaleStrechingUpperBound();
    //m_histMaxSlider->setMinimum(minGrayscaleValue);
    //m_histMaxSlider->setMaximum(maxGrayscaleValue);
    //m_histMaxSlider->setValue(grayscaleStrechingUpperBound);
    //m_histMaxSpinBox->setMinimum(minGrayscaleValue);
    //m_histMaxSpinBox->setMaximum(maxGrayscaleValue);
    //m_histMaxSpinBox->setValue(grayscaleStrechingUpperBound);
    m_histogram->setMaximumCursorValue(grayscaleStrechingUpperBound);

	/*Scale Slider and SpinBox*/
    //qreal zoomFactor = model.getZoomFactor();
    //m_zoomSlider->setMinimum(0);
    //m_zoomSlider->setMaximum(ZOOM_SLIDER_MAX_VALUE);
    //int value = zoomFactor*ZOOM_SLIDER_MAX_VALUE;
    //m_zoomSlider->setValue(value);


    //m_zoomSpinBox->setRange(0.0,1.0);
    //m_zoomSpinBox->setSingleStep(0.01);
    //m_zoomSpinBox->setValue(zoomFactor);

    const QImage & image = model.getSlice(currentSliceIndex);

	/*Histogram*/

    QRect region = model.getZoomRegion();
    m_histogram->setImage(image);
	/*ZoomViwer*/
    m_zoomViewer->setImage(image,region);
    /*There should be a image scale region context to be restored*/
    m_sliceViewer->setImage(image,region);

	m_nestedSliceViewer->setImage(image,region);
	m_nestedSliceViewer->setRightImage(model.getRightSlice(0));
	m_nestedSliceViewer->setFrontImage(model.getFrontSlice(20));


	m_sliceViewer->setMarks(model.getMarks(currentSliceIndex));

    /*PixelViewer*/
    m_pixelViewer->setImage(image);

    /*Set all widgets enable*/
    _allControlWidgetsEnable(true);
	

}

void MainWindow::_saveMRCDataModel()
{
    if(m_currentContext == -1)
        return;

    //Save previous context
	MRCDataModel & model = m_mrcDataModels[m_currentContext];

    int sliceIndex = m_sliceSlider->value();

    model.setCurrentSlice(sliceIndex);

    model.setGrayscaleStrechingLowerBound(m_histogram->getMinimumCursorValue());
    model.setGrayScaleStrechingUpperBound(m_histogram->getMaximumCursorValue());
    model.setZoomRegion(m_zoomViewer->zoomRegion().toRect());
}

void MainWindow::_deleteMRCDataModel(int index)
{

}

void MainWindow::_allControlWidgetsEnable(bool enable)
{
    //Slice Viewer
    m_sliceSlider->setEnabled(enable);
    m_sliceSpinBox->setEnabled(enable);

	m_nestedSliceViewer->setEnable(enable);

    //About zoomViwer
    //m_zoomSlider->setEnabled(enable);
    //m_zoomSpinBox->setEnabled(enable);

    //About histSlier
    //m_histMaxSlider->setEnabled(enable);
    //m_histMaxSpinBox->setEnabled(enable);

    //m_histMinSlider->setEnabled(enable);
    //m_histMinSpinBox->setEnabled(enable);
    m_histogram->setEnabled(enable);

}



void MainWindow::_updateGrayThreshold(int lower, int upper)
{
    size_t width = m_mrcDataModels[m_currentContext].getWidth();
    size_t height = m_mrcDataModels[m_currentContext].getHeight();

	QImage originalImage = m_mrcDataModels[m_currentContext].getOriginalSlice(m_sliceSlider->value());
	unsigned char *image = originalImage.bits();

	qreal k = 256.0 / static_cast<qreal>(upper - lower);
	QImage strechingImage(width, height, QImage::Format_Grayscale8);
	unsigned char * data = strechingImage.bits();
    for(int j=0;j<height;j++){
        for(int i=0;i<width;i++){
            int index = i+j*width;
			unsigned char pixelGrayValue = image[index];
			unsigned char clower = static_cast<unsigned char>(lower);
			unsigned char cupper = static_cast<unsigned char>(upper);
			if (pixelGrayValue < clower) {
				data[index] = 0;
			}
			else if (pixelGrayValue>cupper) {
				data[index] = 255;
			}
			else {
				data[index] = static_cast<unsigned char>(pixelGrayValue* k+0.5);
			}
        }
    }
	//m_sliceViewer->setImage(strechingImage);
	m_mrcDataModels[m_currentContext].setSlice(strechingImage,m_sliceSlider->value());
}
/*
 * This function only sets the initial ui layout,
 * and it doesn't set their properties.
*/
void MainWindow::_initUI()
{

	QVBoxLayout * leftMainLayout = new QVBoxLayout(this);
	//SliceViewer
	m_sliceViewer = new SliceViewer(this);
    QVBoxLayout * sliceViewerLayout = new QVBoxLayout(this);
	
    QHBoxLayout * hLayout = new QHBoxLayout(this);
	m_sliceLabel = new QLabel(this);
	m_sliceLabel->setText(QStringLiteral("Slice:"));
	hLayout->addWidget(m_sliceLabel);

	m_sliceSlider = new QSlider(Qt::Horizontal, this);
	//m_sliceSlider->setTracking(false);
	m_sliceSlider->setEnabled(false);
	hLayout->addWidget(m_sliceSlider);

	m_sliceSpinBox = new QSpinBox(this);
	m_sliceSpinBox->setEnabled(false);
	m_sliceSpinBox->setReadOnly(true);
	hLayout->addWidget(m_sliceSpinBox);

    sliceViewerLayout->addWidget(m_sliceViewer);		//add slice viewer
	leftMainLayout->addLayout(hLayout);
	leftMainLayout->addLayout(sliceViewerLayout);
    ui->leftGroupBox->setLayout(leftMainLayout);

	//Histgram
    //m_histogram = new Histogram(this);
    //ui->leftVLayout->addWidget(m_histogram);
    //m_histMinLabel = new QLabel(this);
    //m_histMinLabel->setText(QStringLiteral("Min:"));
    //m_histMinSlider = new QSlider(Qt::Horizontal, this);
    //m_histMinSlider->setEnabled(false);
	//m_histMinSlider->setTracking(false);
    //m_histMinSpinBox = new QSpinBox(this);
    //m_histMinSpinBox->setEnabled(false);
    //m_histMinSpinBox->setReadOnly(true);
    //hLayout = new QHBoxLayout(this);
    //hLayout->addWidget(m_histMinLabel);
    //hLayout->addWidget(m_histMinSlider);
    //hLayout->addWidget(m_histMinSpinBox);
    //ui->leftVLayout->addLayout(hLayout);
    //m_histMaxLabel = new QLabel(this);
    //m_histMaxLabel->setText(QStringLiteral("Max:"));
    //m_histMaxSlider = new QSlider(Qt::Horizontal, this);
    //m_histMaxSlider->setEnabled(false);
	//m_histMaxSlider->setTracking(false);
    //m_histMaxSpinBox = new QSpinBox(this);
    //m_histMaxSpinBox->setEnabled(false);
    //m_histMaxSpinBox->setReadOnly(true);
    //hLayout = new QHBoxLayout(this);
    //hLayout->addWidget(m_histMaxLabel);
    //hLayout->addWidget(m_histMaxSlider);
    //hLayout->addWidget(m_histMaxSpinBox);
    //ui->leftVLayout->addLayout(hLayout);

    //hLayout = new QHBoxLayout(this);
    //ui->leftVLayout->addLayout(hLayout);

    //PixelViewer

    //ui->leftVLayout->addWidget(m_pixelViewer);



    _allControlWidgetsEnable(false);

    //ToolBar and Actions
	actionColor = new QAction(this);
	actionColor->setText(QStringLiteral("Color"));
	ui->mainToolBar->addAction(actionColor);
	connect(actionColor, SIGNAL(triggered(bool)), this, SLOT(onColorActionTriggered()));



	QAction * actionMark = new QAction(this);
	actionMark->setText(QStringLiteral("Mark"));
	actionMark->setCheckable(true);
	ui->mainToolBar->addAction(actionMark);
	connect(actionMark, SIGNAL(triggered(bool)), m_sliceViewer, SLOT(paintEnable(bool)));

    QAction * actionSaveMark = new QAction(this);
    actionSaveMark->setText(QStringLiteral("Save Mark"));
    ui->mainToolBar->addAction(actionSaveMark);
    connect(actionSaveMark, SIGNAL(triggered()), this, SLOT(onSaveActionTriggered()));

    QAction * actionSaveDataAs = new QAction(this);
    actionSaveDataAs->setText(QStringLiteral("Save Data As"));
    ui->mainToolBar->addAction(actionSaveDataAs);
    connect(actionSaveDataAs,SIGNAL(triggered()),this,SLOT(onSaveDataAsActionTriggered()));


    //Set currentContext Unable
    m_currentContext = -1;
}


/*
 * This function sets all the connections
 * between signal and slot except for the actions'
*/
void MainWindow::_connection()
{

	connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(onActionOpenTriggered()));


    connect(m_zoomViewer, SIGNAL(zoomRegionChanged(const QRectF &)), this, SLOT(onZoomRegionChanged(const QRectF &)));
    //connect(m_zoomSpinBox,SIGNAL(valueChanged(double)),this,SLOT(onZoomDoubleSpinBoxValueChanged(double)));
    //connect(m_zoomSlider,SIGNAL(sliderMoved(int)),this,SLOT(onZoomValueChanged(int)));

    connect(m_sliceSlider, SIGNAL(valueChanged(int)), this, SLOT(onSliceValueChanged(int)));
    connect(m_sliceSlider,SIGNAL(sliderMoved(int)),this,SLOT(onSliceValueChanged(int)));
    //connect(m_sliceSpinBox,SIGNAL(valueChanged(int)),this,SLOT(onSliceValueChanged(int)));

    //connect(m_histMinSlider,SIGNAL(valueChanged(int)),this,SLOT(onMinGrayValueChanged(int)));
    //connect(m_histMinSlider, SIGNAL(sliderMoved(int)), this, SLOT(onMinGrayValueChanged(int)));
    //connect(m_histMinSpinBox,SIGNAL(valueChanged(int)),this,SLOT(onMinGrayValueChanged(int)));
    //connect(m_histMaxSlider,SIGNAL(valueChanged(int)),this,SLOT(onMaxGrayValueChanged(int)));
    //connect(m_histMaxSlider, SIGNAL(sliderMoved(int)), this, SLOT(onMaxGrayValueChanged(int)));
    connect(m_histogram,SIGNAL(minCursorValueChanged(int)),this,SLOT(onMinGrayValueChanged(int)));
    connect(m_histogram,SIGNAL(maxCursorValueChanged(int)),this,SLOT(onMaxGrayValueChanged(int)));
    //connect(m_histMaxSpinBox,SIGNAL(valueChanged(int)),this,SLOT(onMaxGrayValueChanged(int)));
    //PixelViewer
    connect(m_sliceViewer, SIGNAL(drawingFinished(const QPicture &)), this, SLOT(onSliceViewerDrawingFinished(const QPicture &)));
}

void MainWindow::_destroy()
{
    //Nothing need to be destroyed
}

void MainWindow::onMaxGrayValueChanged(int position)
{
//    qDebug()<<position;
//	if (position <m_histMinSlider->value()) {
//		m_histMinSlider->setValue(position);
//		m_histMinSpinBox->setValue(position);
//        m_histogram->setMinimumCursorValue(position);
//	}
//	m_histMaxSpinBox->setValue(position);
    //m_histogram->setMaximumCursorValue(position);
    //int maxValue = m_histMaxSlider->value();
    //int minValue = m_histMinSlider->value();
    int minv = m_histogram->getMinimumCursorValue();
    int maxv = m_histogram->getMaximumCursorValue();
    qDebug()<<minv<<" "<<maxv;
    _updateGrayThreshold(minv,maxv);

	QRect rect = m_zoomViewer->zoomRegion().toRect();
	m_sliceViewer->setImage(m_mrcDataModels[m_currentContext].getSlice(m_sliceSlider->value()),rect);
	m_zoomViewer->setImage(m_mrcDataModels[m_currentContext].getSlice(m_sliceSlider->value()),rect);
}

void MainWindow::onMinGrayValueChanged(int position)
{
//  qDebug()<<position;
//    if(position > m_histMaxSlider->value()){
//		m_histMaxSlider->setValue(position);
//        m_histMaxSpinBox->setValue(position);
//        m_histogram->setMaximumCursorValue(position);
//    }
//	m_histMinSpinBox->setValue(position);

    //m_histogram->setMinimumCursorValue(position);
    //int maxValue = m_histMaxSlider->value();
    //int minValue = m_histMinSlider->value();
    int minv = m_histogram->getMinimumCursorValue();
    int maxv = m_histogram->getMaximumCursorValue();
    qDebug()<<minv<<" "<<maxv;

    _updateGrayThreshold(minv,maxv);
	QRect rect = m_zoomViewer->zoomRegion().toRect();
	m_sliceViewer->setImage(m_mrcDataModels[m_currentContext].getSlice(m_sliceSlider->value()),rect);
	m_zoomViewer->setImage(m_mrcDataModels[m_currentContext].getSlice(m_sliceSlider->value()),rect);
}

void MainWindow::onSliceValueChanged(int value)
{
    m_sliceSpinBox->setValue(value);

	QRectF regionf = m_zoomViewer->zoomRegion();
	QRect region = QRect(regionf.left(),regionf.top(),regionf.width(),regionf.height());
	qDebug() << "onSliceValueChanged(int):" << region;
	m_sliceViewer->setImage(m_mrcDataModels[m_currentContext].getSlice(value),region);
	m_sliceViewer->setMarks(m_mrcDataModels[m_currentContext].getMarks(value));
	//

    m_histogram->setImage(m_mrcDataModels[m_currentContext].getSlice(value));
	m_zoomViewer->setImage(m_mrcDataModels[m_currentContext].getSlice(value),region);

}

void MainWindow::onZSliderValueChanged(int value)
{
	onSliceValueChanged(value);

}

void MainWindow::onYSliderValueChanged(int value)
{

}

void MainWindow::onXSliderValueChanged(int value)
{
}

//void MainWindow::onZoomValueChanged(int value)
//{
//    qreal zoomFactor = 1.0/static_cast<qreal>(ZOOM_SLIDER_MAX_VALUE)*value;
//    m_zoomSpinBox->setValue(zoomFactor);
//    //m_zoomSlider->setValue(value);
//    m_zoomViewer->setZoomFactor(zoomFactor);
//}

void MainWindow::onZoomDoubleSpinBoxValueChanged(double d)
{
    //onZoomValueChanged(d*ZOOM_SLIDER_MAX_VALUE);
}


/*
 *
*/
void MainWindow::onZoomRegionChanged(const QRectF &region)
{
    int slice = m_sliceSlider->value();
    QImage image = m_mrcDataModels[m_currentContext].getSlice(slice);
    m_sliceViewer->setImage(image,region.toRect());
}

void MainWindow::onSliceViewerDrawingFinished(const QPicture & p)
{
	int slice = m_sliceSlider->value();
    m_mrcDataModels[m_currentContext].addMark(slice,p);
    m_sliceViewer->setMarks(m_mrcDataModels[m_currentContext].getMarks(slice));
}

void MainWindow::onColorActionTriggered()
{
	qDebug() << "ColorActionTriggered";
	QColor color = QColorDialog::getColor(Qt::white, this, QStringLiteral("Color Selection"));
	m_sliceViewer->setMarkColor(color);
}

void MainWindow::onSaveActionTriggered()
{
	QString fileName = QFileDialog::getSaveFileName(this, QString("Mark Save"),
		"", QString("Raw Files(*.raw);;MRC Files(*.mrc)"));
	if (fileName.isEmpty() == true)
		return;
	if (fileName.endsWith(QString(".raw")) == true) {
		bool ok = m_mrcDataModels[m_currentContext].saveMarks(fileName,MRCDataModel::MarkFormat::RAW);
		if (ok == false) {
			QMessageBox::critical(this,
				QStringLiteral("Error"),
				QStringLiteral("Can not save this marks"),
				QMessageBox::Ok, QMessageBox::Ok);
		}
	}
	else if (fileName.endsWith(QString(".mrc")) == true) {
			bool ok = m_mrcDataModels[m_currentContext].saveMarks(fileName,MRCDataModel::MarkFormat::MRC);
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
                                                    ".",QString("mrc File(*.mrc);;raw File(*.raw)"));
    if(fileName.isEmpty() == true)
        return;
    if(fileName.endsWith(".raw")==true){
        m_mrcDataModels[m_currentContext].save(fileName);
    }else if(fileName.endsWith(".mrc") == true){
        m_mrcDataModels[m_currentContext].save(fileName);
    }
}

void MainWindow::createDockWindows()
{
    m_fileInfoViewer = new MRCFileInfoViewer(this);
    QDockWidget * dock = new QDockWidget(tr("Files"),this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea);
    dock->setWidget(m_fileInfoViewer);
    addDockWidget(Qt::LeftDockWidgetArea,dock);
    connect(m_fileInfoViewer,SIGNAL(currentIndexChanged(int)),this,SLOT(onMRCFilesComboBoxIndexActivated(int)));
    //menuBar()->addAction(dock->toggleViewAction());


    m_pixelViewer = new PixelViewer(this);
    dock = new QDockWidget(tr("PixelViewer"),this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea);
    dock->setWidget(m_pixelViewer);
    addDockWidget(Qt::LeftDockWidgetArea,dock);
    connect(m_sliceViewer,SIGNAL(onMouseMoving(const QPoint &)),m_pixelViewer,SLOT(setPosition(const QPoint &)));
}
