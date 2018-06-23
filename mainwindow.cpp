#include <QDockWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>
#include <QTreeView>
#include <QDebug>

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
	//MarkModel * markModel = new MarkModel(this);
	auto infoModel = setupProfileModel(*mrc);
	m_profileView->addModel(fileName, infoModel);
	m_imageView->setSliceModel(sliceModel);
	//m_imageView->setMarkModel(markModel);
	m_treeView->setModel(m_imageView->markModel());
	//m_models[fileName] = std::make_tuple(infoModel, sliceModel, markModel);
}

bool MainWindow::save()
{
	QString fileName = QFileDialog::getSaveFileName(this, QStringLiteral("Mark Save"),
		"", QStringLiteral("Mark Files(*.mar);;Raw Files(*.raw)"));
	if (fileName.isEmpty() == true)
		return false;
	if (fileName.endsWith(QStringLiteral(".raw")) == true) {
		//bool ok = m_mrcDataModels[m_currentContext].saveMarks(fileName, ItemContext::MarkFormat::RAW);
		bool ok = false;
		if (ok == false) {
			QMessageBox::critical(this,
				QStringLiteral("Error"),
				QStringLiteral("Can not save this marks"),
				QMessageBox::Ok, QMessageBox::Ok);
		}
	}
	else if (fileName.endsWith(QStringLiteral(".mar")) == true) {
		//bool ok = m_mrcDataModels[m_currentContext].saveMarks(fileName, ItemContext::MarkFormat::MRC);
		auto model = m_imageView->markModel();
		if(model == nullptr)
		{
			QMessageBox::warning(this, QStringLiteral("Warning"), QStringLiteral("There is no mark model"), QMessageBox::Ok);
			return false;
		}
		bool ok = m_imageView->markModel()->save(fileName);
		if (ok == false) {
			QMessageBox::critical(this,
				QStringLiteral("Error"),
				QStringLiteral("Can not save this marks"),
				QMessageBox::Ok, QMessageBox::Ok);
		}
		return ok;
	}
}

void MainWindow::openMark()
{
	QString fileName = QFileDialog::getOpenFileName(this, 
		QStringLiteral("Mark Open"),
		QStringLiteral(""),
		QStringLiteral("Mark File(*.mar)"));
	if (fileName.isEmpty() == true)
		return;
	if (fileName.endsWith(QStringLiteral(".mar")) == true)
	{
		const auto model = m_imageView->markModel();
		if(model == nullptr || model->dirty() == false)
		{
			auto newModel = new MarkModel(fileName);
			bool success;
			
			auto t = m_imageView->replaceMarkModel(newModel, &success);
			m_treeView->setModel(newModel);
			t->deleteLater();
			if(success == false)
			{
				QMessageBox::critical(this, QStringLiteral("Error"), QStringLiteral("Open failed."), QMessageBox::Ok);
				return;
			}
		}else if(model->dirty() == true)
		{
			auto button = QMessageBox::question(this, QStringLiteral("Save"),
				QStringLiteral("The mark model has been modified.Do you want to save it before opening a new one?"),
				QMessageBox::Save | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Save);
			if (button == QMessageBox::Cancel)
				return;
			if(button == QMessageBox::Save)
			{
				if(MainWindow::save() == false)
				{
					auto button = QMessageBox::warning(this, QStringLiteral("Wrong"), 
						QStringLiteral("Saving mark model failed for some reasons.Open the new one anyway?"), 
						QMessageBox::No | QMessageBox::Yes, QMessageBox::No);
					if (button = QMessageBox::No)
						return;
				}
				//open a new one
				auto newModel = new MarkModel(fileName);
				bool success;
				auto t =  m_imageView->replaceMarkModel(newModel, nullptr);
				m_treeView->setModel(newModel);
				t->deleteLater();
				if (success == false)
				{
					QMessageBox::critical(this, QStringLiteral("Error"), QStringLiteral("Open failed."), QMessageBox::Ok);
					return;
				}
			}
		}
	}

}

void MainWindow::saveAs()
{
	QString fileName = QFileDialog::getSaveFileName(this,
		QStringLiteral("Save Data As"),
		".", QStringLiteral("mrc File(*.mrc);;raw File(*.raw)"));
	if (fileName.isEmpty() == true)
		return;
	if (fileName.endsWith(QStringLiteral(".raw")) == true) {
		//m_mrcDataModels[m_currentContext].save(fileName);
	}
	else if (fileName.endsWith(QStringLiteral(".mrc")) == true) {
		//m_mrcDataModels[m_currentContext].save(fileName);
	}
}

void MainWindow::exploererDoubleClicked(const QModelIndex & index)
{
	Q_UNUSED(index);
}
void MainWindow::createWidget()
{
	setDockOptions(QMainWindow::AnimatedDocks);
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

	//open mark action
	m_openMarkAction = new QAction(this);
	m_openMarkAction->setText(QString("Open Mark"));
	toolBar->addAction(m_openMarkAction);
	connect(m_openMarkAction, &QAction::triggered, this, &MainWindow::openMark);

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
