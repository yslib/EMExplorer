#include <QRect>
#include <QDockWidget>
#include <qglobal.h>
#include <QMessageBox>
#include <QTableView>

#include "imageviewer.h"
#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "profileview.h"

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
	connect(m_openFileAction, &QAction::triggered, this, &MainWindow::open);

	QAction *m_saveDataAsAction = m_fileMenu->addAction("Save As..");
	connect(m_saveDataAsAction, &QAction::triggered, this, &MainWindow::save);

	//View menu
	QMenu * viewMenu = menuBar()->addMenu(tr("View"));

	
	QDockWidget *dock;
	//m_treeView = new QTreeView(this);
	//dock = new QDockWidget(tr("File Information View"), this);
	//dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	//dock->setWidget(m_treeView);
	//addDockWidget(Qt::LeftDockWidgetArea, dock);
	//viewMenu->addAction(dock->toggleViewAction());
	//m_treeView->setItemDelegate(new DataItemModelDelegate(m_treeView));

	m_treeViewModel = new DataItemModel(QString(), this);
	///TODO::
	//connect(m_treeView, &QTreeView::doubleClicked, this, &MainWindow::onTreeViewDoubleClicked);
	//m_treeView->setModel(m_treeViewModel);
	

	
	//m_histogramView = new HistogramViewer(this);
	//m_histogramView->setModel(m_treeViewModel);
	//dock = new QDockWidget(tr("Histgoram"), this);
	//dock->setAllowedAreas(Qt::RightDockWidgetArea);
	//dock->setWidget(m_histogramView);
	//addDockWidget(Qt::RightDockWidgetArea, dock);
	//viewMenu->addAction(dock->toggleViewAction());


	//m_filesComboBox = new QComboBox(this);
	//dock = new QDockWidget(QStringLiteral("Files:"), this);
	//dock->setAllowedAreas(Qt::RightDockWidgetArea);
	//dock->setWidget(m_filesComboBox);
	//addDockWidget(Qt::RightDockWidgetArea, dock);
	//viewMenu->addAction(dock->toggleViewAction());
	//connect(m_filesComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index)
	//{
	//	m_infoView->setModel(m_infoModels.value(index));
	//});

	m_profileView = new ProfileView(this);
	dock = new QDockWidget(QStringLiteral("Profile:"), this);
	dock->setAllowedAreas(Qt::RightDockWidgetArea);
	dock->setWidget(m_profileView);
	addDockWidget(Qt::RightDockWidgetArea, dock);
	viewMenu->addAction(dock->toggleViewAction());

	///TODO:: m_histogramView
	//connect(m_histogramView,SIGNAL(minValueChanged(int)),this,SLOT(onMinGrayValueChanged(int)));
	//connect(m_histogramView,SIGNAL(maxValueChanged(int)),this,SLOT(onMaxGrayValueChanged(int)));

	//Test ImageView
	m_imageView = new ImageView(this);
	m_imageView->setModel(m_treeViewModel);
	setCentralWidget(m_imageView);



	//pixel viewer dock widget
	//m_pixelViewer = new PixelViewer(this);
	//dock = new QDockWidget(tr("PixelViewer"), this);
	//dock->setAllowedAreas(Qt::LeftDockWidgetArea);
	//dock->setWidget(m_pixelViewer);
	//addDockWidget(Qt::LeftDockWidgetArea, dock);
	//viewMenu->addAction(dock->toggleViewAction());
	//m_pixelViewer->setModel(m_treeViewModel);

	//ToolBar and Actions [3]
	//open action
	m_actionOpen = new QAction(this);
	m_actionOpen->setText(tr("Open"));
	QToolBar * toolBar = addToolBar(tr("Tools"));
	toolBar->addAction(m_actionOpen);
	connect(m_actionOpen, &QAction::triggered, this, &MainWindow::open);

	///TODO::
	//connect(m_imageView, SIGNAL(zSliceSelected(const QPoint &)), m_pixelViewer, SLOT(setPosition(const QPoint &)));

	//color action
	m_actionColor = new QAction(this);
	m_actionColor->setText(QStringLiteral("Color"));
	toolBar = addToolBar(tr("Tools"));
	toolBar->addAction(m_actionColor);
	connect(m_actionColor, SIGNAL(triggered(bool)), this, SLOT(onColorActionTriggered()));

	//save mark action
	QAction * actionSaveMark = new QAction(this);
	actionSaveMark->setText(QStringLiteral("Save Mark"));
	toolBar->addAction(actionSaveMark);
	connect(actionSaveMark, SIGNAL(triggered()), this, SLOT(save()));

	//save data as action
	QAction * actionSaveDataAs = new QAction(this);
	actionSaveDataAs->setText(QStringLiteral("Save Data As"));
	toolBar->addAction(actionSaveDataAs);
	connect(actionSaveDataAs, SIGNAL(triggered()), this, SLOT(saveAs()));

	//Status bar

	m_currentContext = -1;
	allControlWidgetsEnable(false);

}
MainWindow::~MainWindow()
{
	_destroy();
	delete ui;
}

void MainWindow::open()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("OpenFile"), tr("/Users/Ysl/Downloads/ETdataSegmentation"), tr("mrc Files(*.mrc *mrcs)"));
	if (fileName == "") {
		return;
	}
	QString name = fileName.mid(fileName.lastIndexOf('/') + 1);
	QSharedPointer<ItemContext> sharedItem(new ItemContext(fileName));
	m_treeViewModel->addItem(sharedItem);

    auto model = setupProfileModel(sharedItem->getMRCFile());
    m_profileView->addModel(fileName,model);

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

	const QImage & image = model.getTopSlice(currentSliceIndex);

	int topSliceCount = model.getTopSliceCount();
	int rightSliceCount = model.getRightSliceCount();
	int frontSliceCount = model.getFrontSliceCount();

	/*Histogram*/


	QRect region = model.getZoomRegion();

	allControlWidgetsEnable(true);


}

void MainWindow::saveMRCDataModel()
{
	if (m_currentContext == -1)
		return;

	//Save previous context
	ItemContext & model = m_mrcDataModels[m_currentContext];



}

void MainWindow::deleteMRCDataModel(int index)
{

}

void MainWindow::allControlWidgetsEnable(bool enable)
{
	//TODO::m_histogramView
	//m_histogramView->setEnabled(enable);
}


void MainWindow::_destroy()
{
	//Nothing need to be destroyed
}



/*
 *
*/





void MainWindow::save()
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

void MainWindow::saveAs()
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
	//m_histogramView->activateItem(rootItem);
	m_imageView->activateItem(rootItem);
	//m_pixelViewer->activateItem(rootItem);
}
void MainWindow::createDockWindows()
{

}

void MainWindow::setupInfo(const QString& text)
{

}

QAbstractTableModel * MainWindow::setupProfileModel(const MRC & mrc)
{
	QAbstractTableModel * model = nullptr;
	model = new MRCInfoTableModel(mrc.propertyCount(), 2, this);
	for(int i=0;i<mrc.propertyCount();i++)
	{
        model->setData(model->index(i,0),QVariant::fromValue(QString::fromStdString(mrc.propertyName(i))),Qt::DisplayRole);
        qDebug()<<QString::fromStdString(mrc.propertyName(i));
		MRC::DataType type = mrc.propertyType(i);
		QVariant value;
		if(type == MRC::DataType::Integer32)
		{
			value.setValue(mrc.property<MRC::MRCInt32>(i));
		}else if(type == MRC::DataType::Real32)
		{
			value.setValue(mrc.property<MRC::MRCFloat>(i));
		}else if(type == MRC::DataType::Integer8)
		{
			value.setValue(mrc.property<MRC::MRCInt8>(i));
		}
        qDebug()<<value;
        model->setData(model->index(i, 1), value,Qt::DisplayRole);
	}

	return model;
}
