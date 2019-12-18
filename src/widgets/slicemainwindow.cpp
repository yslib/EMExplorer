#include "slicemainwindow.h"
#include "widgets/sliceeditorwidget.h"
#include "widgets/slicecontrolwidget.h"

#include <QDockWidget>
#include <QToolBar>
#include <QToolButton>
#include <QMessageBox>

#include "model/mrcdatamodel.h"
#include "model/mrc.h"
#include "model/markmodel.h"

SliceMainWindow::SliceMainWindow(QWidget *parent) : QMainWindow(parent)
{
	setWindowTitle("Slice View");

	m_sliceEditorWidget = new SliceEditorWidget(this, true, true, true, nullptr);

	m_sliceControlWidget = new SliceControlWidget(m_sliceEditorWidget, nullptr, this);
	m_controlDockWidget = new QDockWidget(QStringLiteral("Control Pannel"), this);
	m_controlDockWidget->setWidget(m_sliceControlWidget);
	m_controlDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	m_controlDockWidget->setMinimumSize(300, 0);
	m_controlDockWidget->setMaximumSize(300, 10000);
	m_controlDockWidget->setFloating(true);

	m_toolBar = addToolBar(QStringLiteral("toolbar1"));

	m_sliceControlViewToggleAction = m_controlDockWidget->toggleViewAction();
	m_sliceControlViewToggleAction->setIcon(QIcon(":icons/resources/icons/control_panel.png"));

	// Zoom In Action
	m_zoomInAction = new QToolButton(this);
	m_zoomInAction->setToolTip(QStringLiteral("Zoom In"));
	m_zoomInAction->setStyleSheet("QToolButton::menu-indicator{image: none;}");
	m_zoomInAction->setIcon(QIcon(":icons/resources/icons/zoom_in.png"));
	connect(m_zoomInAction, &QToolButton::clicked, m_sliceEditorWidget, &SliceEditorWidget::zoomIn);

	// Zoom Out Action
	m_zoomOutAction = new QToolButton(this);
	m_zoomOutAction->setToolTip(QStringLiteral("Zoom Out"));
	m_zoomOutAction->setStyleSheet("QToolButton::menu-indicator{image: none;}");
	m_zoomOutAction->setIcon(QIcon(":icons/resources/icons/zoom_out.png"));
	connect(m_zoomOutAction, &QToolButton::clicked, m_sliceEditorWidget, &SliceEditorWidget::zoomOut);

	// Reset Zoom Action
	m_resetAction = new QToolButton(this);
	m_resetAction->setToolTip(QStringLiteral("Reset"));
	m_resetAction->setStyleSheet("QToolButton::menu-indicator{image: none;}");
	m_resetAction->setIcon(QIcon(":icons/resources/icons/reset.png"));
	connect(m_resetAction, &QToolButton::clicked, m_sliceEditorWidget, &SliceEditorWidget::resetZoom);

	m_toolBar->addWidget(m_zoomInAction);
	m_toolBar->addWidget(m_zoomOutAction);
	m_toolBar->addWidget(m_resetAction);
	m_toolBar->addSeparator();
	m_toolBar->addAction(m_sliceControlViewToggleAction);


	// Pixel View ToolButton
	m_pixelViewAction = new QToolButton(this);
	m_pixelViewAction->setToolTip(QStringLiteral("Pixel View"));
	m_pixelViewAction->setStyleSheet("QToolButton::menu-indicator{image:none;}");
	m_pixelViewAction->setIcon(QIcon(":icons/resources/icons/picker.png"));
	connect(m_pixelViewAction, &QToolButton::clicked, this, &SliceMainWindow::pixelViewActionTriggered);
	m_toolBar->addWidget(m_pixelViewAction);

	// Histogram ToolButton
	m_histogramAction = new QToolButton(this);
	m_histogramAction->setToolTip(QStringLiteral("Histogram"));
	m_histogramAction->setStyleSheet("QToolButton::menu-indicator{image:none;}");
	m_histogramAction->setIcon(QIcon(":icons/resources/icons/histogram.png"));
	connect(m_histogramAction, &QToolButton::clicked, this, &SliceMainWindow::histogramViewActionTriggered);
	m_toolBar->addWidget(m_histogramAction);

	setCentralWidget(m_sliceEditorWidget);

}

void SliceMainWindow::open(const QString& fileName) {
	if (fileName.isEmpty())
		return;
	const QSharedPointer<MRC> mrc(new MRC(fileName.toStdString()));
	auto * sliceModel = new MRCDataModel(mrc);
	auto m = m_sliceEditorWidget->markModel();
	m->deleteLater();
	const auto t = m_sliceEditorWidget->takeSliceModel(sliceModel);
	delete t;
}

void SliceMainWindow::openMark(const QString & fileName)
{

	if (fileName.isEmpty() == true)
		return;

	if (fileName.endsWith(QStringLiteral(".mar")) == true)
	{
		const auto newModel = new MarkModel(fileName);
		bool success;
		auto t = m_sliceEditorWidget->takeMarkModel(newModel, &success);
		t->deleteLater();
		if (success == false)
		{
			QMessageBox::critical(this, QStringLiteral("Error"), QStringLiteral("Open failed."), QMessageBox::Ok);
			return;
		}
	}

}





void SliceMainWindow::pixelViewActionTriggered()
{
	//PixelWidget * pixelViewDlg = nullptr;
	//SliceType type;
	//QString windowTitle;
	//std::function<void(const QPoint&)> selectSignal;
	//auto a = &SliceEditorWidget::topSliceSelected;


	//if (m_currentFocus == FocusInTopSliceView) {
	//	type = SliceType::Top;
	//	windowTitle = QStringLiteral("TopSlice PixelView");
	//}
	//else if (m_currentFocus == FocusInRightSliceView) {
	//	type = SliceType::Right;
	//	windowTitle = QStringLiteral("RightSlice PixelView");
	//}
	//else if (m_currentFocus == FocusInFrontSliceView) {
	//	type = SliceType::Front;
	//	windowTitle = QStringLiteral("FrontSlice PixelView");
	//}

	//pixelViewDlg = new PixelWidget(type, m_imageView, this);
	//pixelViewDlg->setWindowFlag(Qt::Window);
	//pixelViewDlg->setWindowTitle(windowTitle);
	//pixelViewDlg->show();
	//pixelViewDlg->setAttribute(Qt::WA_DeleteOnClose);

	//if (type == SliceType::Top) {
	//	connect(m_imageView, &SliceEditorWidget::topSliceSelected, pixelViewDlg, &PixelWidget::setPosition);
	//}
	//else if (type == SliceType::Right) {
	//	connect(m_imageView, &SliceEditorWidget::rightSliceSelected, pixelViewDlg, &PixelWidget::setPosition);
	//}
	//else if (type == SliceType::Front) {
	//	connect(m_imageView, &SliceEditorWidget::frontSliceSelected, pixelViewDlg, &PixelWidget::setPosition);
	//}


}

void SliceMainWindow::histogramViewActionTriggered() {
	//HistogramWidget * histogramView = nullptr;
	//SliceType type;
	//QString windowTitle;
	//if (m_currentFocus == FocusInTopSliceView) {
	//	type = SliceType::Top;
	//	windowTitle = QStringLiteral("TopSlice HistogramView");
	//}
	//else if (m_currentFocus == FocusInRightSliceView) {
	//	type = SliceType::Right;
	//	windowTitle = QStringLiteral("RightSlice HistogramView");
	//}
	//else if (m_currentFocus == FocusInFrontSliceView) {
	//	type = SliceType::Front;
	//	windowTitle = QStringLiteral("FrontSlice HistogramView");
	//}
	//histogramView = new HistogramWidget(type, m_imageView, this);
	//histogramView->setWindowTitle(windowTitle);
	//histogramView->setWindowFlag(Qt::Window);
	//histogramView->show();
	//histogramView->setAttribute(Qt::WA_DeleteOnClose);
}