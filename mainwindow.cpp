#include <QDockWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>
#include <QSettings>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>

#include "mainwindow.h"
#include "mrc.h"
#include "imageviewer.h"
#include "profileview.h"
#include "mrcdatamodel.h"
#include "markmodel.h"
#include "marktreeview.h"

//QSize imageSize(500, 500);
MainWindow::MainWindow(QWidget *parent):
	QMainWindow(parent)
	
{
	//These functions need to be call in order.
	setWindowTitle("MRC Marker");
	createActions();
	createMenu();
	createWidget();
	createMarkTreeView();
	createStatusBar();
	resize(1650,1080);
	
	readSettings();
	setUnifiedTitleAndToolBarOnMac(true);
}
MainWindow::~MainWindow()
{
}

void MainWindow::closeEvent(QCloseEvent* event)
{
	const auto model = m_imageView->markModel();
	if(model != nullptr &&model->dirty())
	{
		auto button = QMessageBox::warning(this, QStringLiteral("Save Mark"),
			QStringLiteral("Marks have not been saved. Do you want to save them before closing?"),
			QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
			QMessageBox::Yes);
		if(QMessageBox::Cancel == button)
		{
			event->ignore();
			return;
		}
		if (button == QMessageBox::Yes)
			saveMark();
		event->accept();
	}
	event->accept();
	writeSettings();
}

void MainWindow::open()
{
	QString fileName = QFileDialog::getOpenFileName(this, 
		QStringLiteral("OpenFile"), 
		QStringLiteral("/Users/Ysl/Downloads/ETdataSegmentation"),
		QStringLiteral("mrc Files(*.mrc *mrcs)"));
	if (fileName.isEmpty()) 
		return;
	QString name = fileName.mid(fileName.lastIndexOf('/') + 1);
	Q_UNUSED(name);
	auto markModel = m_imageView->markModel();

	if(markModel != nullptr && markModel->dirty() == true)
	{
		auto button = QMessageBox::warning(this, 
			QStringLiteral("Warning"), 
			QStringLiteral("Marks have not been saved.Do you want to save them before open a new slice data?"),
			QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
			QMessageBox::Yes);
		if (button == QMessageBox::Yes)
			saveMark();
		else if (button == QMessageBox::Cancel)
			return;
	}
	QSharedPointer<MRC> mrc(new MRC(fileName.toStdString()));
	MRCDataModel * sliceModel = new MRCDataModel(mrc);
	auto infoModel = setupProfileModel(*mrc);
	auto m = m_imageView->markModel();
	m->deleteLater();
	auto t =  m_imageView->takeSliceModel(sliceModel);
	delete t;
	m_treeView->setModel(m_imageView->markModel());
	auto d = m_profileView->takeModel(infoModel);
		d->deleteLater();
}
bool MainWindow::saveMark()
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
			QMessageBox::warning(this, 
				QStringLiteral("Warning"), 
				QStringLiteral("There is no mark model"), QMessageBox::Ok);
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
	return false;
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
			
			auto t = m_imageView->takeMarkModel(newModel, &success);
			m_treeView->setModel(newModel);
			t->deleteLater();
			if(success == false)
			{
				QMessageBox::critical(this, 
					QStringLiteral("Error"), 
					QStringLiteral("Open failed."), QMessageBox::Ok);
				return;
			}
		}else if(model->dirty() == true)
		{
			auto button = QMessageBox::question(this, QStringLiteral("Save"),
				QStringLiteral("The mark model has been modified. Do you want to save it before opening a new one?"),
				QMessageBox::Save | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Save);
			if (button == QMessageBox::Cancel)
				return;
			if(button == QMessageBox::Save)
			{
				if(MainWindow::saveMark() == false)
				{
					auto button = QMessageBox::warning(this, QStringLiteral("Wrong"), 
						QStringLiteral("Saving mark model failed for some reasons. Open the new one anyway?"), 
						QMessageBox::No | QMessageBox::Yes, QMessageBox::No);
					if (button = QMessageBox::No)
						return;
				}
				//open a new one
			}
			auto newModel = new MarkModel(fileName);
			bool success;
			auto t = m_imageView->takeMarkModel(newModel, &success);
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

void MainWindow::writeSettingsForDockWidget(QDockWidget * dock, QSettings* settings)
{
	if (settings == nullptr)
	{
		QScopedPointer<QSettings> ptr(new QSettings);
		settings = ptr.data();
	}
	settings->beginGroup(dock->windowTitle());
	settings->setValue(QStringLiteral("floatarea"), dockWidgetArea(dock));
	settings->setValue(QStringLiteral("floating"), dock->isFloating());
	settings->setValue(QStringLiteral("visible"), dock->isVisible());
	settings->setValue(QStringLiteral("pos"), dock->pos());
	settings->setValue(QStringLiteral("size"), dock->size());
	settings->endGroup();
}

void MainWindow::readSettingsForDockWidget(QDockWidget* dock, QSettings* settings)
{
	if (settings == nullptr)
	{
		QScopedPointer<QSettings> ptr(new QSettings);
		settings = ptr.data();
	}
	settings->beginGroup(dock->windowTitle());
	bool floating = settings->value(QStringLiteral("floating"), false).toBool();
	bool visible = settings->value(QStringLiteral("visible"), true).toBool();
	dock->move(settings->value(QStringLiteral("pos"), QPoint(0, 0)).toPoint());
	dock->resize(settings->value(QStringLiteral("size"), QSize(100, 500)).toSize());
	dock->setVisible(visible);
	if (floating == true)
	{
		dock->setFloating(true);
	}
	else
	{
		addDockWidget(Qt::DockWidgetArea(settings->value(QStringLiteral("floatarea"),
			Qt::LeftDockWidgetArea).toInt()),
			dock);
	}
	settings->endGroup();
}

void MainWindow::writeSettingsForImageView(ImageView * view, QSettings * settings)
{
	if (settings == nullptr)
	{
		QScopedPointer<QSettings> ptr(new QSettings);
		settings = ptr.data();
	}
	settings->beginGroup(view->windowTitle());
	settings->setValue(QStringLiteral("topvisible"), view->isTopSliceEnabled());
	settings->setValue(QStringLiteral("rightvisible"), view->isRightSliceEnabled());
	settings->setValue(QStringLiteral("frontvisible"), view->isFrontSliceEnabled());
	settings->setValue(QStringLiteral("pen"), view->pen());
	settings->endGroup();
}

void MainWindow::readSettingsForImageView(ImageView * view, QSettings * settings)
{
	if (settings == nullptr)
	{
		QScopedPointer<QSettings> ptr(new QSettings);
		settings = ptr.data();
	}
	settings->beginGroup(view->windowTitle());
	view->topSliceEnable(settings->value(QStringLiteral("topvisible"), true).toBool());
	view->rightSliceEnable(settings->value(QStringLiteral("rightvisible"), true).toBool());
	view->frontSliceEnable(settings->value(QStringLiteral("frontvisible"), true).toBool());
	view->setPen(settings->value(QStringLiteral("pen"), QVariant::fromValue(QPen(Qt::black, 5, Qt::SolidLine))).value<QPen>());
	settings->endGroup();

}

void MainWindow::readSettings()
{
	QSettings settings;
	//
	settings.beginGroup(this->windowTitle());
	auto d = (QApplication::desktop()->screenGeometry()).size();
	setGeometry(settings.value(QStringLiteral("geometry"),QRect(QPoint(d.width()/2-1680/2,d.height()/2-1050/2),QSize(1680,1050))).toRect());
	settings.endGroup();

	readSettingsForDockWidget(m_profileViewDockWidget, &settings);
	readSettingsForDockWidget(m_treeViewDockWidget, &settings);
	readSettingsForImageView(m_imageView,&settings);

}

void MainWindow::writeSettings()
{
	QSettings settings;
	settings.beginGroup(this->windowTitle());
	settings.setValue(QStringLiteral("geometry"), geometry());
	settings.endGroup();

	writeSettingsForDockWidget(m_profileViewDockWidget, &settings);
	writeSettingsForDockWidget(m_treeViewDockWidget, &settings);
	writeSettingsForImageView(m_imageView, &settings);
}

void MainWindow::createWidget()
{
	setDockOptions(QMainWindow::AnimatedDocks);
	m_profileView = new ProfileView(this);
	m_profileViewDockWidget = new QDockWidget(QStringLiteral("Profile"));
	m_profileViewDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	m_profileViewDockWidget->setWidget(m_profileView);
	addDockWidget(Qt::RightDockWidgetArea, m_profileViewDockWidget);

	m_profileViewDockWidget->installEventFilter(this);
	m_profileViewDockWidget->setObjectName(QStringLiteral("profileviewdockwidget"));

	m_viewMenu->addAction(m_profileViewDockWidget->toggleViewAction());
	m_imageView = new ImageView;
	connect(m_imageView, &ImageView::markModified, [this](){setWindowTitle(QStringLiteral("MRC Marker*"));});
	connect(m_imageView, &ImageView::markSaved, [this](){setWindowTitle(QStringLiteral("MRC Marker"));});

	setCentralWidget(m_imageView);
}

void MainWindow::createMenu()
{
	//File menu
	m_fileMenu = menuBar()->addMenu(QStringLiteral("File"));
	m_fileMenu->addAction(m_openAction);
	m_fileMenu->addAction(m_saveAction);
	//m_fileMenu->addAction(m_saveAsAction);
	//View menu
	m_viewMenu = menuBar()->addMenu(QStringLiteral("View"));

}

void MainWindow::createActions()
{

	m_openAction= new QAction(QIcon(":/icons/resources/icons/open.png"), QStringLiteral("Open"),this);
	m_openAction->setToolTip(QStringLiteral("Open MRC file"));
	QToolBar * toolBar = addToolBar(QStringLiteral("Tools"));
	toolBar->addAction(m_openAction);
	connect(m_openAction, &QAction::triggered, this, &MainWindow::open);



	//save mark action
	 m_saveAction = new QAction(QIcon(":/icons/resources/icons/save_as.png"), QStringLiteral("Save Mark"),this);
	 m_saveAction->setToolTip(QStringLiteral("Save Mark"));
	toolBar->addAction(m_saveAction);
	connect(m_saveAction, &QAction::triggered, this, &MainWindow::saveMark);

	//save data as action
	//m_saveAsAction= new QAction(this);
	//m_saveAsAction->setText(QStringLiteral("Save Data As"));
	//toolBar->addAction(m_saveAsAction);
	//connect(m_saveAsAction, &QAction::triggered, this, &MainWindow::saveAs);

	//open mark action
	m_openMarkAction = new QAction(QIcon(":/icons/resources/icons/open_mark.png"),QStringLiteral("Open Mark"),this);
	m_openMarkAction->setToolTip(QStringLiteral("Open Mark"));
	toolBar->addAction(m_openMarkAction);
	connect(m_openMarkAction, &QAction::triggered, this, &MainWindow::openMark);

}

void MainWindow::createStatusBar()
{
	m_statusBar = statusBar();
	m_statusBar->showMessage(QStringLiteral("Ready"));
}

void MainWindow::createMarkTreeView()
{
	m_treeView = new MarkTreeView;
	m_treeViewDockWidget = new QDockWidget(QStringLiteral("MarkManager"));
	m_treeViewDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	m_treeViewDockWidget->setWidget(m_treeView);
	addDockWidget(Qt::LeftDockWidgetArea, m_treeViewDockWidget);
	m_viewMenu->addAction(m_treeViewDockWidget->toggleViewAction());

	//m_treeView->setContextMenuPolicy(Qt::ActionsContextMenu);
	//QAction * m_markDeleteAction = new QAction(QStringLiteral("Delete"),m_treeView);
	//QAction * m_markRenameAction = new QAction(QStringLiteral("Delete"),m_treeView);
	//QAction * m_markUnionAction = new QAction(QStringLiteral("Union"),m_treeView);
	//m_treeView->addAction(m_markDeleteAction);
	//m_treeView->addAction(m_markRenameAction);
	//m_treeView->addAction(m_markUnionAction);

}

AbstractSliceDataModel * MainWindow::replaceSliceModel(AbstractSliceDataModel * model)
{
	auto old = m_imageView->sliceModel();
	m_imageView->takeSliceModel(model);
	return old;
}

MarkModel * MainWindow::replaceMarkModel(MarkModel * model)
{
	return m_imageView->takeMarkModel(model,nullptr);

}

QAbstractTableModel * MainWindow::replaceProfileModel( QAbstractTableModel * model)
{
	return nullptr;
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
