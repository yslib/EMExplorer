#include <QDockWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>
#include <QTreeView>
#include <QDebug>


//custom headers
#include "mainwindow.h"
#include "mrc.h"
#include "imageviewer.h"
#include "profileview.h"
#include "mrcdatamodel.h"
#include "markmodel.h"

//QSize imageSize(500, 500);
MainWindow::MainWindow(QWidget *parent):
	QMainWindow(parent)
	
{

	//These functions need to be call in order.
	createActions();

	createMenu();

	createWidget();

	createStatusBar();
	resize(1650,1080);

	setWindowTitle(QStringLiteral("MRC Editor"));
	
}
MainWindow::~MainWindow()
{
}

void MainWindow::open()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("OpenFile"), tr("/Users/Ysl/Downloads/ETdataSegmentation"), tr("mrc Files(*.mrc *mrcs)"));
	if (fileName == "") {
		return;
	}
	QString name = fileName.mid(fileName.lastIndexOf('/') + 1);
	Q_UNUSED(name);
	QSharedPointer<MRC> mrc(new MRC(fileName.toStdString()));
	MRCDataModel * sliceModel = new MRCDataModel(mrc);
	MarkModel * markModel = new MarkModel(this);
	auto infoModel = setupProfileModel(*mrc);
	m_profileView->addModel(fileName, infoModel);
	m_imageView->setSliceModel(sliceModel);
	m_imageView->setMarkModel(markModel);
	m_treeView->setModel(markModel);
	m_models[fileName] = std::make_tuple(infoModel, sliceModel, markModel);
}




void MainWindow::save()
{
	QString fileName = QFileDialog::getSaveFileName(this, QString("Mark Save"),
		"", QString("Raw Files(*.raw);;MRC Files(*.mrc)"));
	if (fileName.isEmpty() == true)
		return;
	if (fileName.endsWith(QString(".raw")) == true) {
		//bool ok = m_mrcDataModels[m_currentContext].saveMarks(fileName, ItemContext::MarkFormat::RAW);
		bool ok = false;
		if (ok == false) {
			QMessageBox::critical(this,
				QStringLiteral("Error"),
				QStringLiteral("Can not save this marks"),
				QMessageBox::Ok, QMessageBox::Ok);
		}
	}
	else if (fileName.endsWith(QString(".mrc")) == true) {
		//bool ok = m_mrcDataModels[m_currentContext].saveMarks(fileName, ItemContext::MarkFormat::MRC);
		bool ok = false;
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
		//m_mrcDataModels[m_currentContext].save(fileName);
	}
	else if (fileName.endsWith(".mrc") == true) {
		//m_mrcDataModels[m_currentContext].save(fileName);
	}
}

void MainWindow::exploererDoubleClicked(const QModelIndex & index)
{
	Q_UNUSED(index);
}
void MainWindow::createWidget()
{
	QDockWidget * dock;
	m_profileView = new ProfileView(this);
	dock = new QDockWidget(QStringLiteral("Profile:"));
	dock->setAllowedAreas(Qt::RightDockWidgetArea);
	dock->setWidget(m_profileView);
	addDockWidget(Qt::RightDockWidgetArea, dock);
	m_viewMenu->addAction(dock->toggleViewAction());

	m_treeView = new QTreeView;
	dock = new QDockWidget(QStringLiteral("Mark Manager"));
	dock->setAllowedAreas(Qt::LeftDockWidgetArea);
	dock->setWidget(m_treeView);
	addDockWidget(Qt::LeftDockWidgetArea, dock);
	m_viewMenu->addAction(dock->toggleViewAction());

	m_imageView = new ImageView;
	setCentralWidget(m_imageView);
}

void MainWindow::createMenu()
{
	//File menu
	m_fileMenu = menuBar()->addMenu(tr("File"));
	m_fileMenu->addAction(m_openAction);
	m_fileMenu->addAction(m_saveAsAction);
	//View menu
	m_viewMenu = menuBar()->addMenu(tr("View"));

}

void MainWindow::createActions()
{

	m_openAction= new QAction(this);
	m_openAction->setText(tr("Open"));
	QToolBar * toolBar = addToolBar(tr("Tools"));
	toolBar->addAction(m_openAction);
	connect(m_openAction, &QAction::triggered, this, &MainWindow::open);

	//color action
	m_colorAction = new QAction(this);
	m_colorAction->setText(QStringLiteral("Color"));
	toolBar = addToolBar(tr("Tools"));
	toolBar->addAction(m_colorAction);

	//save mark action
	 m_saveAction = new QAction(this);
	 m_saveAction->setText(QStringLiteral("Save Mark"));
	toolBar->addAction(m_saveAction);
	connect(m_saveAction, &QAction::triggered, this, &MainWindow::save);

	//save data as action
	m_saveAsAction= new QAction(this);
	m_saveAsAction->setText(QStringLiteral("Save Data As"));
	toolBar->addAction(m_saveAsAction);
	connect(m_saveAsAction, &QAction::triggered, this, &MainWindow::saveAs);
}

void MainWindow::createStatusBar()
{
	m_statusBar = statusBar();
	m_statusBar->showMessage(QStringLiteral("Ready"));
}
QAbstractTableModel * MainWindow::setupProfileModel(const MRC & mrc)
{
	QAbstractTableModel * model = nullptr;
	model = new MRCInfoTableModel(mrc.propertyCount(), 2, this);
	for(int i=0;i<mrc.propertyCount();i++)
	{
        model->setData(model->index(i,0),QVariant::fromValue(QString::fromStdString(mrc.propertyName(i))),Qt::DisplayRole);
        //qDebug()<<QString::fromStdString(mrc.propertyName(i));
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
        //qDebug()<<value;
        model->setData(model->index(i, 1), value,Qt::DisplayRole);
	}

	return model;
}
