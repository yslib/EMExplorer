#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "testinfodialog.h"
#include <QPen>
#include <QRect>

QSize imageSize(500,500);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _initUI();
	_connection();

}
MainWindow::~MainWindow()
{
	_destroy();
    delete ui;
}

void MainWindow::on_actionOpen_triggered()
{
    if(m_currentContext != -1){
        QMessageBox::critical(this,"Error",
                              tr("Multi files aren't supported now\n"),
                              QMessageBox::Yes,QMessageBox::Yes);
        return;
    }
    QString fileName =QFileDialog::getOpenFileName(this,tr("OpenFile"),tr("/Users/Ysl/Downloads/ETdataSegmentation"),tr("Image Files(*.mrc)"));
    if(fileName == ""){
        return;
    }
   // ui->comboBox->addItem(fileName);
    //m_mrcs.push_back(MRC(fileName));
	MRCDataModel mrcModel(fileName);
    QString name = fileName.mid(fileName.lastIndexOf('/')+1);
    if(mrcModel.isOpened() == false){
        QMessageBox::critical(NULL, "Error", tr("Can't open this file.\n%1").arg(fileName),
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    }else{
        QString headerInfo = mrcModel.getMRCInfo();
        ui->textEdit->setText(headerInfo);
        //APPENDINFO(headerInfo)
        //m_mrcs.push_back(std::move(mrc));
        _addMRCDataModel(std::move(mrcModel));
        _setMRCDataModel(m_mrcDataModels.size()-1);
		//m_sliceViewer->setImage(m_mrcs[m_currentContext].mrcFile.getSlice(0));
        m_currentContext = m_mrcDataModels.size() - 1;
		//m_mrcFileCBox->insertItem(m_mrcDataModels.size(),name, m_currentContext);
        m_mrcFileCBox->addItem(name,m_currentContext);
        //m_imageView->setPixmap(QPixmap::fromImage(m_mrcs.back().getSlice(0)));
    }

}


void MainWindow::_addMRCDataModel(const MRCDataModel & model)
{
	m_mrcDataModels.push_back(model);
}

void MainWindow::_addMRCDataModel(MRCDataModel && model)
{
	m_mrcDataModels.push_back(std::move(model));
}

void MainWindow::_setMRCDataModel(int index)
{
	m_sliceSlider->setEnabled(true);
	m_sliceSpinBox->setEnabled(true);

	m_sliceSlider->setEnabled(true);
	m_zoomSpinBox->setEnabled(true);
	m_histMaxSlider->setEnabled(true);
	m_histMaxSpinBox->setEnabled(true);
	m_histMinSlider->setEnabled(true);
	m_histMinSpinBox->setEnabled(true);
	m_zoomSlider->setEnabled(true);
	m_zoomSpinBox->setEnabled(true);

	const MRCDataModel & model = m_mrcDataModels[index];

	m_sliceSlider->setMaximum(model.getSliceCount()-1);
	m_sliceSlider->setMinimum(0);
	m_sliceSlider->setValue(model.getCurrentSlice());

	m_sliceSpinBox->setMaximum(model.getSliceCount()-1);
	m_sliceSpinBox->setMinimum(0);
	m_sliceSpinBox->setValue(model.getCurrentSlice());

	/*Max Gray Slider and SpinBox*/
	m_histMinSlider->setMinimum(model.getMinGrayscale());
	m_histMinSlider->setMaximum(model.getMaxGrayscale());
	m_histMinSlider->setValue(model.getGrayscaleStrechingLowerBound());
	m_histMinSlider->setMinimum(model.getMinGrayscale());
	m_histMinSlider->setMaximum(model.getMaxGrayscale());
	m_histMinSlider->setValue(model.getGrayscaleStrechingLowerBound());

	/*Min Gray Slider and SpinBox*/
	m_histMaxSlider->setMinimum(model.getMinGrayscale());
	m_histMaxSlider->setMaximum(model.getMaxGrayscale());
	m_histMaxSlider->setValue(model.getGrayscaleStrechingUpperBound());
	m_histMaxSpinBox->setMinimum(model.getMinGrayscale());
	m_histMaxSpinBox->setMaximum(model.getMaxGrayscale());
	m_histMaxSpinBox->setValue(model.getGrayscaleStrechingUpperBound());

	/*Scale Slider and SpinBox*/
	m_zoomSlider->setValue(model.getZoomFactor());
	m_zoomSlider->setMinimum(0);

	const QImage & image = model.getSlice(model.getCurrentSlice());
	/*Histogram*/
	m_histogramViewer->setImage(image);

	/*ZoomViwer*/
	m_zoomViewer->setImage(image);
	m_sliceViewer->setImage(image);

	m_currentContext = index;

}

void MainWindow::_saveMRCDataModel()
{
	MRCDataModel & model = m_mrcDataModels[m_currentContext];
	model.setCurrentSlice(m_sliceSlider->value());
	model.setGrayscaleStrechingLowerBound(m_histMinSlider->value());
	model.setGrayScaleStrechingUpperBound(m_histMaxSlider->value());
}

void MainWindow::_createMRCContext(const MRC &mrc)
{
    MRCContext mrcContext;
    mrcContext.currentMaxGray=255;
    mrcContext.currentMinGray = 0;
    mrcContext.minSlice = 0;
    mrcContext.maxSlice=mrc.getSliceCount()-1;
    mrcContext.currentSlice = 0;
    mrcContext.currentScale =1.0f;
    mrcContext.mrcFile = mrc;
    int imageCount = mrcContext.maxSlice;
	//for (int i = 0; i < imageCount; i++) {
	//	mrcContext.images.push_back(QPixmap::fromImage(mrcContext.mrcFile.getSlice(i)));
	//}
    m_mrcs.push_back(mrcContext);

}

void MainWindow::_createMRCContext(MRC && mrc)
{
    MRCContext mrcContext;
    mrcContext.currentMaxGray=255;
    mrcContext.currentMinGray = 0;
    mrcContext.minSlice = 0;
    mrcContext.maxSlice=mrc.getSliceCount()-1;
    mrcContext.currentSlice = 0;
    mrcContext.currentScale =1.0f;
    mrcContext.mrcFile = std::move(mrc);
    int imageCount = mrcContext.maxSlice;
    //for(int i=0;i<imageCount;i++){
    //    mrcContext.images.push_back(QPixmap::fromImage(mrcContext.mrcFile.getSlice(i)));
    //}
    m_mrcs.push_back(mrcContext);
}

void MainWindow::_setMRCContext(int index)
{

 //   ui->sliceSlider->setEnabled(true);
 //   ui->scaleSpinBox->setEnabled(true);
 //   ui->minGraySlider->setEnabled(true);
 //   ui->minGraySpinBox->setEnabled(true);
 //   ui->maxGraySlider->setEnabled(true);
 //   ui->maxGraySpinBox->setEnabled(true);
 //   ui->scaleSlider->setEnabled(true);
 //   ui->scaleSpinBox->setEnabled(true);

 //   const MRCContext CTX = m_mrcs[index];

 //   //other settings
 //   //ui->horizontalSlider->setMaximum(m_mrcs[index].maxSlice);
 //   /*Min Gray Slider and SpinBox*/
 //   ui->sliceSlider->setMaximum(CTX.maxSlice);
 //   ui->sliceSlider->setMinimum(CTX.minSlice);
 //   ui->sliceSlider->setValue(CTX.currentSlice);

 //   ui->sliceSpinBox->setMaximum(CTX.maxSlice);
 //   ui->sliceSpinBox->setMinimum(CTX.minSlice);
 //   ui->sliceSpinBox->setValue(CTX.currentSlice);

 //   /*Max Gray Slider and SpinBox*/
 //   ui->minGraySlider->setMinimum(0);
 //   ui->minGraySlider->setMaximum(255);
 //   ui->minGraySlider->setValue(CTX.currentMinGray);
 //   ui->minGraySpinBox->setMinimum(0);
 //   ui->minGraySpinBox->setMaximum(255);
 //   ui->minGraySpinBox->setValue(CTX.currentMinGray);

 //   /*Min Gray Slider and SpinBox*/
 //   ui->maxGraySlider->setMinimum(0);
 //   ui->maxGraySlider->setMaximum(255);
 //   ui->maxGraySlider->setValue(CTX.currentMaxGray);
 //   ui->maxGraySpinBox->setMinimum(0);
 //   ui->maxGraySpinBox->setMaximum(255);
 //   ui->maxGraySpinBox->setValue(CTX.currentMaxGray);

 //   /*Scale Slider and SpinBox*/
 //   ui->scaleSlider->setValue(CTX.currentScale);
 //   ui->scaleSlider->setMinimum(0);

 //   /*Histogram*/
 //   m_histogramViewer->setImage(CTX.mrcFile.getSlice(CTX.currentSlice));

	///*ZoomViwer*/
	//m_zoomViewer->setImage(CTX.mrcFile.getSlice(CTX.currentSlice));

 //   m_currentContext = index;

}

void MainWindow::_saveMRCContext()
{
    MRCContext & CTX = m_mrcs[m_currentContext];
    CTX.currentSlice = m_sliceSlider->value();
    CTX.currentMinGray = m_histMinSlider->value();
    CTX.currentMaxGray = m_histMaxSlider->value();
    //CTX.currentScale = ui->scaleSlider->value();

}

void MainWindow::_updateGrayThreshold(int lower, int upper)
{
    size_t width = m_mrcDataModels[m_currentContext].getWidth();
    size_t height = m_mrcDataModels[m_currentContext].getHeight();
    //qDebug()<<"W & H of the image:"<<width<<" "<<height;
	QImage originalImage = m_mrcDataModels[m_currentContext].getOriginalSlice(m_sliceSlider->value());
	unsigned char *image = originalImage.bits();

	qreal k = 256.0 / (upper - lower);
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
				data[index] = static_cast<unsigned char>(pixelGrayValue* k);
			}
        }
    }
	//m_sliceViewer->setImage(strechingImage);
	m_mrcDataModels[m_currentContext].setSlice(strechingImage,m_sliceSlider->value());
	
}

//void MainWindow::_displayImage(QSize size)
//{
//    int index = ui->sliceSlider->value();
//	m_sliceViewer->setImage(m_mrcs[m_currentContext].mrcFile.getSlice(index).scaled(size, Qt::IgnoreAspectRatio));
//    //m_image = m_mrcs[m_currentContext].images[index].scaled(size,Qt::IgnoreAspectRatio);
//    //m_imageLabel->setPixmap(m_image);
//}

void MainWindow::_initUI()
{

	QVBoxLayout * leftMainLayout = new QVBoxLayout(this);
	//MRCFile Combox
	m_mrcFileLabel = new QLabel(this);
	m_mrcFileLabel->setText(QStringLiteral("MrcFiles:"));
	m_mrcFileCBox = new QComboBox(this);

	QHBoxLayout * hLayout = new QHBoxLayout(this);
	hLayout->addWidget(m_mrcFileLabel);
	hLayout->addWidget(m_mrcFileCBox);
	ui->leftVLayout->addLayout(hLayout);


	//SliceViewer
	m_sliceViewer = new SliceViewer(this);
    QVBoxLayout * sliceViewerLayout = new QVBoxLayout(this);
	
	hLayout = new QHBoxLayout(this);
	m_sliceLabel = new QLabel(this);
	m_sliceLabel->setText(QStringLiteral("Slice:"));
	hLayout->addWidget(m_sliceLabel);

	m_sliceSlider = new QSlider(Qt::Horizontal, this);
	m_sliceSlider->setEnabled(false);
	hLayout->addWidget(m_sliceSlider);

	m_sliceSpinBox = new QSpinBox(this);
	m_sliceSpinBox->setEnabled(false);
	hLayout->addWidget(m_sliceSpinBox);

    sliceViewerLayout->addWidget(m_sliceViewer);		//add slice viewer
	leftMainLayout->addLayout(hLayout);
	leftMainLayout->addLayout(sliceViewerLayout);
    ui->leftGroupBox->setLayout(leftMainLayout);

	//Histgram
	m_histogramViewer = new Histogram(this);
	ui->leftVLayout->addWidget(m_histogramViewer);
	m_histMinLabel = new QLabel(this);
	m_histMinLabel->setText(QStringLiteral("Min:"));
	m_histMinSlider = new QSlider(Qt::Horizontal, this);
	m_histMinSlider->setEnabled(false);
	m_histMinSpinBox = new QSpinBox(this);
	m_histMinSpinBox->setEnabled(false);
	hLayout = new QHBoxLayout(this);
	hLayout->addWidget(m_histMinLabel);
	hLayout->addWidget(m_histMinSlider);
	hLayout->addWidget(m_histMinSpinBox);
	ui->leftVLayout->addLayout(hLayout);
	m_histMaxLabel = new QLabel(this);
	m_histMaxLabel->setText(QStringLiteral("Max:"));
	m_histMaxSlider = new QSlider(Qt::Horizontal, this);
	m_histMaxSlider->setEnabled(false);
	m_histMaxSpinBox = new QSpinBox(this);
	m_histMaxSpinBox->setEnabled(false);
	hLayout = new QHBoxLayout(this);
	hLayout->addWidget(m_histMaxLabel);
	hLayout->addWidget(m_histMaxSlider);
	hLayout->addWidget(m_histMaxSpinBox);
	ui->leftVLayout->addLayout(hLayout);

	//ZoomViewer
	m_zoomViewer = new ZoomViwer(this);
	ui->leftVLayout->addWidget(m_zoomViewer);
	m_zoomLabel = new QLabel(this);
	m_zoomLabel->setText(QStringLiteral("Zoom:"));
	m_zoomSlider = new QSlider(Qt::Horizontal, this);
	m_zoomSpinBox = new QDoubleSpinBox(this);
	m_zoomSpinBox->setEnabled(false);
	hLayout = new QHBoxLayout(this);
	hLayout->addWidget(m_zoomLabel);
	hLayout->addWidget(m_zoomSlider);
	hLayout->addWidget(m_zoomSpinBox);
	ui->leftVLayout->addLayout(hLayout);

    m_sliceSlider->setEnabled(false);
    m_zoomSpinBox->setEnabled(false);
    m_histMinSlider->setEnabled(false);
    m_histMinSpinBox->setEnabled(false);
    m_histMaxSlider->setEnabled(false);
    m_histMaxSpinBox->setEnabled(false);
    m_zoomSlider->setEnabled(false);
    m_zoomSpinBox->setEnabled(false);

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

    m_currentContext = -1;
}

void MainWindow::_connection()
{
	connect(m_zoomViewer, SIGNAL(zoomRegionChanged(QRectF)), this, SLOT(onZoomRegionChanged(QRectF)));
	connect(m_sliceSlider, SIGNAL(valueChanged(int)), this, SLOT(on_sliceSlider_valueChanged(int)));
	connect(m_histMinSlider, SIGNAL(sliderMoved(int)), this, SLOT(on_minGraySlider_sliderMoved(int)));
	connect(m_histMaxSlider, SIGNAL(sliderMoved(int)), this, SLOT(on_maxGraySlider_sliderMoved(int)));
	connect(m_sliceViewer, SIGNAL(onDrawing(const QPoint &)), this, SLOT(onSliceViewerDrawing(const QPoint &)));
}

void MainWindow::_destroy()
{
	delete m_histogramViewer;
	delete m_zoomViewer;
	delete m_sliceViewer;
}

void MainWindow::on_sliceSlider_sliderMoved(int position)
{
    //qDebug()<<"Slider moved:"<<position<<" "<<ui->sliceSlider->maximum();
    //on_sliceSlider_valueChanged(position);
    //m_imageLabel->setPixmap(m_mrcs[m_currentContext].images[position]);
}

void MainWindow::on_maxGraySlider_sliderMoved(int position)
{
	if (position <m_histMinSlider->value()) {
		m_histMinSlider->setValue(position);
		m_histMinSpinBox->setValue(position);
		m_histogramViewer->setMinimumValue(position);
	}
	m_histMaxSlider->setValue(position);
	m_histMaxSpinBox->setValue(position);
	m_histogramViewer->setMaximumValue(position);
	int maxValue = m_histMaxSlider->value();
	int minValue = m_histMinSlider->value();
	_updateGrayThreshold(minValue,maxValue);
	m_sliceViewer->setImage(m_mrcDataModels[m_currentContext].getSlice(m_sliceSlider->value()));
	m_zoomViewer->setImage(m_mrcDataModels[m_currentContext].getSlice(m_sliceSlider->value()));
}

void MainWindow::on_minGraySlider_sliderMoved(int position)
{
    if(position > m_histMaxSlider->value()){
		m_histMaxSlider->setValue(position);
        m_histMaxSpinBox->setValue(position);
		m_histogramViewer->setMaximumValue(position);
    }
	m_histMinSlider->setValue(position);
	m_histMinSpinBox->setValue(position);
	m_histogramViewer->setMinimumValue(position);
	int maxValue = m_histMaxSlider->value();
	int minValue = m_histMinSlider->value();
    _updateGrayThreshold(minValue,maxValue);
	m_sliceViewer->setImage(m_mrcDataModels[m_currentContext].getSlice(m_sliceSlider->value()));
	m_zoomViewer->setImage(m_mrcDataModels[m_currentContext].getSlice(m_sliceSlider->value()));
}

void MainWindow::on_sliceSlider_valueChanged(int value)
{
    if(value>= m_sliceSlider->maximum())
        return;
	m_sliceSlider->setValue(value);
    m_sliceSpinBox->setValue(value);
	m_sliceViewer->setImage(m_mrcDataModels[m_currentContext].getSlice(value));
    //_displayImage(imageSize);
    m_histogramViewer->setImage(m_mrcDataModels[m_currentContext].getSlice(value));
	m_zoomViewer->setImage(m_mrcDataModels[m_currentContext].getSlice(value));
}

void MainWindow::onZoomRegionChanged(QRectF region)
{
	int slice = m_sliceSlider->value();
	//m_mrcs[m_currentContext].images[slice]= QPixmap::fromImage(m_mrcs[m_currentContext].mrcFile.getSlice(slice).copy(QRect(region.x(),region.y(),region.width(),region.height())));
	//_displayImage(imageSize);
	QImage image = m_mrcDataModels[m_currentContext].getSlice(slice);
	QRect reg(region.x(), region.y(), region.width(), region.height());

	QPicture pic;
	QPainter painter(&pic);
	painter.setPen(m_sliceViewer->getMarkColor());
	QPen pen;
	pen.setWidth(5);
	painter.setPen(pen);
	painter.drawEllipse(500, 500, 500, 500);
	painter.drawPoint(500, 500);
	painter.drawRect(0, 0, 500, 500);
	painter.end();

	QPainter p2(&image);
	p2.drawPicture(0, 0, pic);
	//p2.drawRect(0, 0, 500, 500);
	p2.end();
	m_sliceViewer->setImage(image,reg);
}

void MainWindow::onSliceViewerDrawing(const QPoint & point)
{
	int slice = m_sliceSlider->value();
	QPointF start = m_zoomViewer->zoomRegion().topLeft();
	QPoint transformedPoint(point.x()+start.x(), point.y()+start.y());
    QColor color = m_sliceViewer->getMarkColor();
    QPicture mark;
    QPainter p(&mark);
    QPen pen;
    pen.setWidth(5);
    p.setPen(pen);
    p.drawPoint(transformedPoint);
    p.end();
    m_mrcDataModels[m_currentContext].addMark(slice,mark);
    m_sliceViewer->setMarks(m_mrcDataModels[m_currentContext].getMarks(slice));
}

void MainWindow::onColorActionTriggered()
{
	qDebug() << "ColorActionTriggered";
	QColor color = QColorDialog::getColor(Qt::white, this, QStringLiteral("Color Selection"));
	m_sliceViewer->setMarkColor(color);
}
