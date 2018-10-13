#include <QDockWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>
#include <QSettings>
#include <QApplication>
#include <QDesktopWidget>
#include <QVBoxLayout>
#include <QToolButton>
#include <QEvent>
#include <QButtonGroup>

#include "mainwindow.h"
#include "model/mrc.h"
#include "widgets/sliceeditorwidget.h"
#include "widgets/profilewidget.h"
#include "model/mrcdatamodel.h"
#include "model/markmodel.h"
#include "widgets/marktreeviewwidget.h"
#include "widgets/markinfowidget.h"
#include "widgets/renderoptionwidget.h"
#include "widgets/slicetoolwidget.h"
#include "widgets/renderwidget.h"
#include "widgets/slicewidget.h"
#include "widgets/slicecontrolwidget.h"
#include "widgets/pixelwidget.h"
#include "widgets/histogramwidget.h"

//QSize imageSize(500, 500);
MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent)
	,m_toolBar(nullptr)
{
	//These functions need to be called in order.
	setWindowTitle("MRC Marker");
	createActions();
	createMenu();
	createWidget();
	setDefaultLayout();
	createStatusBar();
	resize(1650, 1080);

	//readSettings();
	updateActionsAndControlPanelByWidgetFocus(static_cast<FocusState>(0));
	updateActionsBySelectionInSliceView();
	setUnifiedTitleAndToolBarOnMac(true);
}
MainWindow::~MainWindow()
{
}

void MainWindow::closeEvent(QCloseEvent* event)
{
	const auto model = m_imageView->markModel();
	if (model != nullptr &&model->dirty())
	{
		const auto button = QMessageBox::warning(this, QStringLiteral("Save Mark"),
			QStringLiteral("Marks have not been saved. Do you want to save them before closing?"),
			QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
			QMessageBox::Yes);
		if (QMessageBox::Cancel == button)
		{
			event->ignore();
			return;
		}
		if (button == QMessageBox::Yes)
			saveMark();
		event->accept();
	}
	event->accept();
	//writeSettings();
}

void MainWindow::open(const QString& fileName)
{


	if (fileName.isEmpty())
		return;
	auto name = fileName.mid(fileName.lastIndexOf('/') + 1);
	Q_UNUSED(name);
	const auto markModel = m_imageView->markModel();

	if (markModel != nullptr && markModel->dirty() == true)
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
	auto * sliceModel = new MRCDataModel(mrc);
	const auto infoModel = setupProfileModel(*mrc);
	auto m = m_imageView->markModel();
	m->deleteLater();
	auto t = m_imageView->takeSliceModel(sliceModel);
	m_volumeView->setDataModel(sliceModel);
	m_volumeView->setMarkModel(m_imageView->markModel());		//Add at 2018.09.17
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

	auto model = m_imageView->markModel();
	if (model == nullptr)
	{
		QMessageBox::warning(this,
			QStringLiteral("Warning"),
			QStringLiteral("There is no mark model"), QMessageBox::Ok);
		return false;
	}
	bool ok;
	if (fileName.endsWith(QStringLiteral(".raw")))
		ok = m_imageView->markModel()->save(fileName, MarkModel::MarkFormat::Raw);
	else if (fileName.endsWith(QStringLiteral(".mar")))
		ok = m_imageView->markModel()->save(fileName, MarkModel::MarkFormat::Binary);
	if (ok == false) {
		QMessageBox::critical(this,
			QStringLiteral("Error"),
			QStringLiteral("Can not save this marks"),
			QMessageBox::Ok, QMessageBox::Ok);
	}
	return ok;
}

void MainWindow::openMark(const QString & fileName)
{

	if (fileName.isEmpty() == true)
		return;
	if (fileName.endsWith(QStringLiteral(".mar")) == true)
	{
		const auto model = m_imageView->markModel();
		if (model == nullptr || model->dirty() == false)
		{
			auto newModel = new MarkModel(fileName);
			bool success;

			auto t = m_imageView->takeMarkModel(newModel, &success);
			m_treeView->setModel(newModel);
			//add mark model to 3d renderwidget
			m_volumeView->setMarkModel(newModel);	//Add at 2018.09.17
			t->deleteLater();
			if (success == false)
			{
				QMessageBox::critical(this,
					QStringLiteral("Error"),
					QStringLiteral("Open failed."), QMessageBox::Ok);
				return;
			}
		}
		else if (model->dirty() == true)
		{
			auto button = QMessageBox::question(this, QStringLiteral("Save"),
				QStringLiteral("The mark model has been modified. Do you want to save it before opening a new one?"),
				QMessageBox::Save | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Save);
			if (button == QMessageBox::Cancel)
				return;
			if (button == QMessageBox::Save)
			{
				if (MainWindow::saveMark() == false)
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
			m_volumeView->setMarkModel(newModel);		//Add at 2018.09.17
			t->deleteLater();
			if (success == false)
			{
				QMessageBox::critical(this, QStringLiteral("Error"), QStringLiteral("Open failed."), QMessageBox::Ok);
				return;
			}
		}
	}

}

bool MainWindow::eventFilter(QObject* watched, QEvent* event) {
	if(watched == m_volumeView) {
		if(event->type() == QEvent::FocusIn) {
			updateActionsAndControlPanelByWidgetFocus(FocusInVolumeView);
			event->accept();
			return true;
		}
	}else if(watched == m_imageView) {
		if(event->type() == QEvent::FocusIn) {
			updateActionsAndControlPanelByWidgetFocus(FocusInSliceWidget);
			event->accept();
			return true;
		}
	}
	return QMainWindow::eventFilter(watched, event);
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
	const auto floating = settings->value(QStringLiteral("floating"), false).toBool();
	const auto visible = settings->value(QStringLiteral("visible"), true).toBool();
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

void MainWindow::writeSettingsForImageView(SliceEditorWidget * view, QSettings * settings)
{
	if (settings == nullptr)
	{
		QScopedPointer<QSettings> ptr(new QSettings);
		settings = ptr.data();
	}
	settings->beginGroup(view->windowTitle());
	settings->setValue(QStringLiteral("topvisible"), view->topSliceVisible());
	settings->setValue(QStringLiteral("rightvisible"), view->rightSliceVisible());
	settings->setValue(QStringLiteral("frontvisible"), view->frontSliceVisible());
	settings->setValue(QStringLiteral("pen"), view->pen());
	settings->endGroup();
}

void MainWindow::readSettingsForImageView(SliceEditorWidget * view, QSettings * settings)
{
	if (settings == nullptr)
	{
		QScopedPointer<QSettings> ptr(new QSettings);
		settings = ptr.data();
	}
	settings->beginGroup(view->windowTitle());
	view->setTopSliceVisible(settings->value(QStringLiteral("topvisible"), true).toBool());
	view->setRightSliceVisible(settings->value(QStringLiteral("rightvisible"), true).toBool());
	view->setFrontSliceVisible(settings->value(QStringLiteral("frontvisible"), true).toBool());
	view->setPen(settings->value(QStringLiteral("pen"), QVariant::fromValue(QPen(Qt::black, 5, Qt::SolidLine))).value<QPen>());
	settings->endGroup();

}

void MainWindow::readSettings()
{
	QSettings settings;
	//
	settings.beginGroup(this->windowTitle());
	auto d = (QApplication::desktop()->screenGeometry()).size();
	setGeometry(settings.value(QStringLiteral("geometry"), QRect(QPoint(d.width() / 2 - 1680 / 2, d.height() / 2 - 1050 / 2), QSize(1680, 1050))).toRect());
	settings.endGroup();


	readSettingsForDockWidget(m_profileViewDockWidget, &settings);
	//readSettingsForDockWidget(m_treeViewDockWidget, &settings);
	readSettingsForDockWidget(m_markInfoDockWidget, &settings);
	readSettingsForImageView(m_imageView, &settings);
	readSettingsForDockWidget(m_volumeViewDockWidget, &settings);

}

void MainWindow::writeSettings()
{
	QSettings settings;
	settings.beginGroup(this->windowTitle());
	settings.setValue(QStringLiteral("geometry"), geometry());
	settings.endGroup();

	writeSettingsForDockWidget(m_profileViewDockWidget, &settings);
	//writeSettingsForDockWidget(m_treeViewDockWidget, &settings);
	writeSettingsForDockWidget(m_markInfoDockWidget, &settings);
	writeSettingsForImageView(m_imageView, &settings);
	writeSettingsForDockWidget(m_volumeViewDockWidget, &settings);
}

void MainWindow::setDefaultLayout()
{
	addDockWidget(Qt::LeftDockWidgetArea, m_profileViewDockWidget);
	addDockWidget(Qt::RightDockWidgetArea,m_controlDockWidget);
	splitDockWidget(m_profileViewDockWidget,m_imageViewDockWidget,Qt::Horizontal);
	tabifyDockWidget(m_imageViewDockWidget,m_volumeViewDockWidget);
	//splitDockWidget(m_treeViewDockWidget, m_markInfoDockWidget,Qt::Vertical);
	splitDockWidget(m_profileViewDockWidget, m_markInfoDockWidget, Qt::Vertical);
}

void MainWindow::setParallelLayout() {
	addDockWidget(Qt::LeftDockWidgetArea, m_profileViewDockWidget);
	addDockWidget(Qt::RightDockWidgetArea, m_controlDockWidget);
	splitDockWidget(m_profileViewDockWidget, m_imageViewDockWidget, Qt::Horizontal);
	splitDockWidget(m_imageViewDockWidget, m_volumeViewDockWidget,Qt::Horizontal);
//	splitDockWidget(m_treeViewDockWidget, m_markInfoDockWidget, Qt::Vertical);
	splitDockWidget(m_profileViewDockWidget, m_markInfoDockWidget, Qt::Vertical);
}

void MainWindow::pixelViewActionTriggered()
{
	PixelWidget * pixelViewDlg = nullptr;
	SliceType type;
	QString windowTitle;
	std::function<void(const QPoint&)> selectSignal;
	auto a = &SliceEditorWidget::topSliceSelected;
	

	if(m_currentFocus == FocusInTopSliceView) {
		type = SliceType::Top;
		windowTitle = QStringLiteral("TopSlice PixelView");
	}else if(m_currentFocus == FocusInRightSliceView) {
		type = SliceType::Right;
		windowTitle = QStringLiteral("RightSlice PixelView");
	}else if(m_currentFocus == FocusInFrontSliceView) {
		type = SliceType::Front;
		windowTitle = QStringLiteral("FrontSlice PixelView");
	}
	
	pixelViewDlg = new PixelWidget(type, m_imageView, this);
	pixelViewDlg->setWindowFlag(Qt::Window);
	pixelViewDlg->setWindowTitle(windowTitle);
	pixelViewDlg->show();
	pixelViewDlg->setAttribute(Qt::WA_DeleteOnClose);

	if(type == SliceType::Top) {
		connect(m_imageView,&SliceEditorWidget::topSliceSelected , pixelViewDlg, &PixelWidget::setPosition);
	}else if(type == SliceType::Right) {
		connect(m_imageView,&SliceEditorWidget::rightSliceSelected , pixelViewDlg, &PixelWidget::setPosition);
	}else if(type == SliceType::Front) {
		connect(m_imageView,&SliceEditorWidget::frontSliceSelected , pixelViewDlg, &PixelWidget::setPosition);
	}


}

void MainWindow::histogramViewActionTriggered() {
	HistogramWidget * histogramView = nullptr;
	SliceType type;
	QString windowTitle;
	if (m_currentFocus == FocusInTopSliceView) {
		type = SliceType::Top;
		windowTitle = QStringLiteral("TopSlice HistogramView");
	}
	else if (m_currentFocus == FocusInRightSliceView) {
		type = SliceType::Right;
		windowTitle = QStringLiteral("RightSlice HistogramView");
	}
	else if (m_currentFocus == FocusInFrontSliceView) {
		type = SliceType::Front;
		windowTitle = QStringLiteral("FrontSlice HistogramView");
	}
	histogramView = new HistogramWidget(type, m_imageView, this);
	histogramView->setWindowTitle(windowTitle);
	histogramView->setWindowFlag(Qt::Window);
	histogramView->show();
	histogramView->setAttribute(Qt::WA_DeleteOnClose);
}



void MainWindow::toolBarActionsTriggered(QAction* action) {
	///TODO::
	return;
}

void MainWindow::updateActionsAndControlPanelByWidgetFocus(FocusState state) {

	//static FocusState s_state = static_cast<FocusState>(0);
	m_currentFocus = state;

	m_zoomInAction->setEnabled(state & (FocusInSliceView));
	m_zoomOutAction->setEnabled(state & (FocusInSliceView));
	m_resetAction->setEnabled(state & (FocusInSliceView | FocusInSliceWidget));
	m_markAction->setEnabled(state & (FocusInTopSliceView));
	m_markSelectionAction->setEnabled(state & (FocusInTopSliceView));		// FocusInRightSliceView FocusInFrontSliceView would be added in the future
	m_anchorAction->setEnabled(state &(FocusInSliceView));

	m_sliceMoveAction->setEnabled(state & (FocusInSliceView));
	m_pixelViewAction->setEnabled(state & (FocusInSliceView));
	m_histogramAction->setEnabled(state & (FocusInSliceView));

	m_volumeControlWidget->setVisible(state & (FocusInVolumeView));
	m_sliceToolControlWidget->setVisible(state & (FocusInSliceWidget | FocusInSliceView));
	m_sliceControlWidget->setVisible(state&(FocusInVolumeView|FocusInSliceWidget|FocusInSliceView));
	m_treeView->setVisible(state&(FocusInSliceWidget | FocusInSliceView));
}

void MainWindow::updateActionsBySelectionInSliceView(){
	Q_ASSERT_X(m_imageView, "MainWindow::updateActionsBySelectionInSliceView", "null pointer");
	const auto topView = m_imageView->topView();
	//const auto rightView = m_imageView->rightView();
	//const auto frontView = m_imageView->frontView();
	const bool enable = topView->selectedItemCount();	// rightView and frontView would be considered in the future
	m_markDeletionAction->setEnabled(enable);
}

void MainWindow::sliceViewSelected(SliceType type) {
	/// TODO::update histogram and pixel view context
	if(type == SliceType::Top){
		updateActionsAndControlPanelByWidgetFocus(FocusInTopSliceView);
	}else if(type == SliceType::Right) {
		updateActionsAndControlPanelByWidgetFocus(FocusInRightSliceView);
	}else if(type == SliceType::Front) {
		updateActionsAndControlPanelByWidgetFocus(FocusInFrontSliceView);
	}
}

void MainWindow::createWidget()
{
	setDockOptions(QMainWindow::AnimatedDocks);
	setDockOptions(QMainWindow::AllowNestedDocks);
	setDockOptions(QMainWindow::AllowTabbedDocks);
	setDockNestingEnabled(true);	

	auto w = takeCentralWidget();
	if (w)
		w->deleteLater();

	// ProfileView
	m_profileView = new ProfileWidget(this);

	m_profileViewDockWidget = new QDockWidget(QStringLiteral("MRC Info"));
	m_profileViewDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
	m_profileViewDockWidget->setWidget(m_profileView);
	m_profileViewDockWidget->setMinimumSize(300, 0);
	m_profileViewDockWidget->setMaximumSize(300, 10000);
	m_viewMenu->addAction(m_profileViewDockWidget->toggleViewAction());

	// MarkInfoWIdget
	m_markInfoWidget = new MarkInfoWidget(this);

	m_markInfoDockWidget = new QDockWidget(QStringLiteral("Mark Info"));
	m_markInfoDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
	m_markInfoDockWidget->setWidget(m_markInfoWidget);
	m_markInfoDockWidget->setMinimumSize(300, 0);
	m_markInfoDockWidget->setMaximumSize(300, 10000);
	m_viewMenu->addAction(m_markInfoDockWidget->toggleViewAction());

	// MarkTreeView
	m_treeView = new MarkManagerWidget;


	//ImageCanvas  centralWidget
	m_imageViewDockWidget = new QDockWidget(QStringLiteral("Slice View"));
	m_imageView = new SliceEditorWidget;
	m_imageView->installEventFilter(this);
	m_imageView->setFocusPolicy(Qt::ClickFocus);
	m_imageViewDockWidget->setWidget(m_imageView);
	m_imageViewDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
	connect(m_imageViewDockWidget, &QDockWidget::visibilityChanged, [this](bool enable) {if(enable)updateActionsAndControlPanelByWidgetFocus(FocusInSliceWidget); });


	connect(m_imageView, &SliceEditorWidget::markModified, [this]() {setWindowTitle(QStringLiteral("MRC Marker*")); });
	connect(m_imageView, &SliceEditorWidget::markSaved, [this]() {setWindowTitle(QStringLiteral("MRC Marker")); });
	connect(m_imageView, &SliceEditorWidget::viewFocus, this, &MainWindow::sliceViewSelected);
	connect(m_imageView->topView(), &SliceWidget::selectionChanged, this, &MainWindow::updateActionsBySelectionInSliceView);

	// VolumeWidget
	m_volumeView = new RenderWidget(nullptr, nullptr,this);
	m_volumeView->installEventFilter(this);
	m_volumeView->setFocusPolicy(Qt::ClickFocus);
	m_volumeViewDockWidget = new QDockWidget(QStringLiteral("Volume View"));
	m_volumeViewDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
	m_volumeViewDockWidget->setWidget(m_volumeView);
	connect(m_volumeViewDockWidget, &QDockWidget::visibilityChanged, [this](bool enable) {if(enable)updateActionsAndControlPanelByWidgetFocus(FocusInVolumeView); });
	m_viewMenu->addAction(m_volumeViewDockWidget->toggleViewAction());

	// Control Widget
	m_volumeControlWidget = new RenderParameterWidget(m_volumeView, this);
	m_sliceToolControlWidget = new SliceToolWidget(m_imageView, this);
	m_sliceControlWidget = new SliceControlWidget(m_imageView, m_volumeView, this);


	auto layout = new QVBoxLayout;
	layout->addWidget(m_sliceControlWidget);
	layout->addWidget(m_volumeControlWidget);
	layout->addWidget(m_sliceToolControlWidget);
	layout->addWidget(m_treeView);
	layout->addStretch(1);
	m_scrollAreaWidget = new QScrollArea(this);
	m_scrollAreaWidget->setLayout(layout);

	m_controlDockWidget = new QDockWidget(QStringLiteral("Control Pannel"),this);
	m_controlDockWidget->setWidget(m_scrollAreaWidget);
	m_controlDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
	m_controlDockWidget->setMinimumSize(300, 0);
	m_controlDockWidget->setMaximumSize(300,10000);
	m_viewMenu->addAction(m_controlDockWidget->toggleViewAction());


	connect(m_imageView, &SliceEditorWidget::markSeleteced, m_markInfoWidget, &MarkInfoWidget::setMark);


	Q_ASSERT_X(m_toolBar, "MainWindow::createWidget", "null pointer");
	m_toolBar->addSeparator();
	// Zoom In Action
	m_zoomInAction = new QToolButton(this);
	m_zoomInAction->setToolTip(QStringLiteral("Zoom In"));
	m_zoomInAction->setStyleSheet("QToolButton::menu-indicator{image: none;}");
	m_zoomInAction->setIcon(QIcon(":icons/resources/icons/zoom_in.png"));
	connect(m_zoomInAction, &QToolButton::clicked, m_imageView, &SliceEditorWidget::zoomIn);
	m_toolBar->addWidget(m_zoomInAction);

	// Zoom Out Action
	m_zoomOutAction = new QToolButton(this);
	m_zoomOutAction->setToolTip(QStringLiteral("Zoom Out"));
	m_zoomOutAction->setStyleSheet("QToolButton::menu-indicator{image: none;}");
	m_zoomOutAction->setIcon(QIcon(":icons/resources/icons/zoom_out.png"));
	connect(m_zoomOutAction, &QToolButton::clicked, m_imageView, &SliceEditorWidget::zoomOut);
	m_toolBar->addWidget(m_zoomOutAction);

	// Reset Zoom Action
	m_resetAction = new QToolButton(this);
	m_resetAction->setToolTip(QStringLiteral("Reset"));
	m_resetAction->setStyleSheet("QToolButton::menu-indicator{image: none;}");
	m_resetAction->setIcon(QIcon(":icons/resources/icons/reset.png"));
	connect(m_resetAction, &QToolButton::clicked, m_imageView, &SliceEditorWidget::resetZoom);
	m_toolBar->addWidget(m_resetAction);

	m_toolBar->addSeparator();


	m_markButtonGroup = new QButtonGroup(this);

	// Mark Pen Action
	m_markAction = new QToolButton(this);
	m_markAction->setToolTip(QStringLiteral("Mark"));
	m_markAction->setCheckable(true);
	m_markAction->setStyleSheet("QToolButton::menu-indicator{image: none;}");
	m_markAction->setIcon(QIcon(":icons/resources/icons/mark.png"));
	m_markAction->setCheckable(true);

	m_markButtonGroup->addButton(m_markAction);

	connect(m_markAction, &QToolButton::toggled, [this](bool enable)
	{
			m_imageView->topView()->setOperation(SliceWidget::Paint);
			m_imageView->rightView()->setOperation(SliceWidget::Paint);
			m_imageView->frontView()->setOperation(SliceWidget::Paint);
	});
	m_toolBar->addWidget(m_markAction);

	// Selection Tool Button
	m_markSelectionAction = new QToolButton(this);
	m_markSelectionAction->setToolTip(QStringLiteral("Select Mark"));
	m_markSelectionAction->setCheckable(true);
	m_markSelectionAction->setStyleSheet("QToolButton::menu-indicator{image: none;}");
	m_markSelectionAction->setIcon(QIcon(":icons/resources/icons/select.png"));
	m_markButtonGroup->addButton(m_markSelectionAction);
	connect(m_markSelectionAction, &QToolButton::toggled, [this](bool enable)
	{
			m_imageView->topView()->setOperation(SliceWidget::Selection);
			m_imageView->rightView()->setOperation(SliceWidget::Selection);
			m_imageView->frontView()->setOperation(SliceWidget::Selection);
	});

	m_toolBar->addWidget(m_markSelectionAction);

	m_sliceMoveAction = new QToolButton(this);
	m_sliceMoveAction->setToolTip(QStringLiteral("Move"));
	m_sliceMoveAction->setCheckable(true);
	m_sliceMoveAction->setStyleSheet("QToolButton::menu-indicator{image: none;}");
	m_sliceMoveAction->setIcon(QIcon(":icons/resources/icons/select.png"));
	m_markButtonGroup->addButton(m_sliceMoveAction);
	connect(m_sliceMoveAction, &QToolButton::toggled, [this](bool enable)
	{
		m_imageView->topView()->setOperation(SliceWidget::Move);
		m_imageView->rightView()->setOperation(SliceWidget::Move);
		m_imageView->frontView()->setOperation(SliceWidget::Move);
	});

	m_toolBar->addWidget(m_sliceMoveAction);

	m_anchorAction = new QToolButton(this);
	m_anchorAction->setToolTip(QStringLiteral("Anchor"));
	m_anchorAction->setCheckable(true);
	m_anchorAction->setStyleSheet("QToolButton::menu-indicator{image: none;}");
	m_anchorAction->setIcon(QIcon(":icons/resources/icons/voteModel.png"));
	m_markButtonGroup->addButton(m_anchorAction);
	m_toolBar->addWidget(m_anchorAction);
	connect(m_anchorAction, &QToolButton::toggled, [this](bool enable) {
		m_imageView->topView()->setOperation(SliceWidget::None);
		m_imageView->rightView()->setOperation(SliceWidget::None);
		m_imageView->frontView()->setOperation(SliceWidget::None);
	});
	m_toolBar->addSeparator();

	// Deletion ToolButton
	m_markDeletionAction = new QToolButton(this);
	m_markDeletionAction->setToolTip(QStringLiteral("Delete Mark"));
	m_markDeletionAction->setStyleSheet("QToolButton::menu-indicator{image: none;}");
	m_markDeletionAction->setIcon(QIcon(":icons/resources/icons/delete.png"));
	connect(m_markDeletionAction, &QToolButton::clicked, [this](bool enable) {Q_UNUSED(enable); m_imageView->deleteSelectedMarks(); });
	m_toolBar->addWidget(m_markDeletionAction);

	m_toolBar->addSeparator();

	// Pixel View ToolButton
	m_pixelViewAction = new QToolButton(this);
	m_pixelViewAction->setToolTip(QStringLiteral("Pixel View"));
	m_pixelViewAction->setStyleSheet("QToolButton::menu-indicator{image:none;}");
	m_pixelViewAction->setIcon(QIcon(":icons/resources/icons/VPSelect.png"));
	/// TODO:: connect
	connect(m_pixelViewAction, &QToolButton::clicked, this, &MainWindow::pixelViewActionTriggered);
	m_toolBar->addWidget(m_pixelViewAction);

	// Histogram ToolButton
	m_histogramAction = new QToolButton(this);
	m_histogramAction->setToolTip(QStringLiteral("Histogram"));
	m_histogramAction->setStyleSheet("QToolButton::menu-indicator{image:none;}");
	m_histogramAction->setIcon(QIcon(":icons/resources/icons/histogram.png"));
	/// TODO:: connect
	connect(m_histogramAction, &QToolButton::clicked, this, &MainWindow::histogramViewActionTriggered);
	m_toolBar->addWidget(m_histogramAction);

}

void MainWindow::createMenu()
{
	//File menu
	m_fileMenu = menuBar()->addMenu(QStringLiteral("File"));
	m_fileMenu->addAction(m_openAction);
	m_fileMenu->addAction(m_saveAction);
	m_fileMenu->addAction(QStringLiteral("Close"),this,&MainWindow::close);

	//View menu
	m_viewMenu = menuBar()->addMenu(QStringLiteral("View"));

}

void MainWindow::createActions()
{
	m_openAction = new QAction(QIcon(":/icons/resources/icons/open.png"), QStringLiteral("Open"), this);
	m_openAction->setToolTip(QStringLiteral("Open MRC file"));

	m_toolBar = addToolBar(QStringLiteral("Tools"));

	m_toolBar->addAction(m_openAction);

	connect(m_openAction, &QAction::triggered, [this]() {
		open(QFileDialog::getOpenFileName(this,
		QStringLiteral("OpenFile"),
		QStringLiteral("/Users/Ysl/Downloads/ETdataSegmentation"),
		QStringLiteral("mrc Files(*.mrc *mrcs)")));	
	});

	//save mark action
	m_saveAction = new QAction(QIcon(":/icons/resources/icons/save_as.png"), QStringLiteral("Save Mark"), this);
	m_saveAction->setToolTip(QStringLiteral("Save Mark"));
	m_toolBar->addAction(m_saveAction);
	connect(m_saveAction, &QAction::triggered, this, &MainWindow::saveMark);

	//open mark action
	m_openMarkAction = new QAction(QIcon(":/icons/resources/icons/open_mark.png"), QStringLiteral("Open Mark"), this);
	m_openMarkAction->setToolTip(QStringLiteral("Open Mark"));
	m_toolBar->addAction(m_openMarkAction);
	connect(m_openMarkAction, &QAction::triggered,[this]() {
		openMark(QFileDialog::getOpenFileName(this,
			QStringLiteral("Mark Open"),
			QStringLiteral(""),
			QStringLiteral("Mark File(*.mar)")));
	});

	//set default action
	m_setDefaultLayoutAction = new QAction(QIcon(":/icons/resources/icons/grid.png"),QStringLiteral("Default Layout"),this);
	m_setDefaultLayoutAction->setToolTip(QStringLiteral("Default Layout"));
	m_toolBar->addAction(m_setDefaultLayoutAction);
	connect(m_setDefaultLayoutAction, &QAction::triggered, this, &MainWindow::setDefaultLayout);

	// Set Parallel Layout Action
	m_parallelLayoutAction = new QAction(QIcon(":/icons/resources/icons/arrowLeftRight.png"), QStringLiteral("Parallel Layout"), this);
	m_parallelLayoutAction->setToolTip(QStringLiteral("Parallel Layout"));
	m_toolBar->addAction(m_parallelLayoutAction);
	connect(m_parallelLayoutAction, &QAction::triggered, this, &MainWindow::setParallelLayout);
}

void MainWindow::createStatusBar()
{
	m_statusBar = statusBar();
	m_statusBar->showMessage(QStringLiteral("Ready"));
}

void MainWindow::createMarkTreeView()
{
	//m_treeView = new MarkTreeView;
	//m_treeViewDockWidget = new QDockWidget(QStringLiteral("MarkManager"));
	//m_treeViewDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
	//m_treeViewDockWidget->setWidget(m_treeView);
	//addDockWidget(Qt::LeftDockWidgetArea, m_treeViewDockWidget);
	//m_viewMenu->addAction(m_treeViewDockWidget->toggleViewAction());

}

void MainWindow::createSliceEditorPlugins() {
	
}


AbstractSliceDataModel * MainWindow::replaceSliceModel(AbstractSliceDataModel * model)
{
	auto old = m_imageView->sliceModel();
	m_imageView->takeSliceModel(model);
	return old;
}

MarkModel * MainWindow::replaceMarkModel(MarkModel * model)
{
	return m_imageView->takeMarkModel(model, nullptr);

}

QAbstractTableModel * MainWindow::replaceProfileModel(QAbstractTableModel * model)
{
	return nullptr;
}

QAbstractTableModel * MainWindow::setupProfileModel(const MRC & mrc)
{
	QAbstractTableModel * model = nullptr;
	model = new MRCInfoTableModel(mrc.propertyCount(), 2, this);
	for (int i = 0; i < mrc.propertyCount(); i++)
	{
		model->setData(model->index(i, 0), QVariant::fromValue(QString::fromStdString(mrc.propertyName(i))), Qt::DisplayRole);
		//qDebug()<<QString::fromStdString(mrc.propertyName(i));
		MRC::DataType type = mrc.propertyType(i);
		QVariant value;
		if (type == MRC::DataType::Integer32)
		{
			value.setValue(mrc.property<MRC::MRCInt32>(i));
		}
		else if (type == MRC::DataType::Real32)
		{
			value.setValue(mrc.property<MRC::MRCFloat>(i));
		}
		else if (type == MRC::DataType::Integer8)
		{
			value.setValue(mrc.property<MRC::MRCInt8>(i));
		}
		//qDebug()<<value;
		model->setData(model->index(i, 1), value, Qt::DisplayRole);
	}
	return model;
}
