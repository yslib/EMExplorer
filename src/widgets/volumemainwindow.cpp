#include "volumemainwindow.h"

#include "widgets/renderwidget.h"
#include "widgets/renderoptionwidget.h"
#include "widgets/slicecontrolwidget.h"

#include <QDockWidget>
#include <QVBoxLayout>
#include <QToolBar>
#include "model/mrc.h"
#include "model/mrcdatamodel.h"
#include "model/markmodel.h"

VolumeMainWindow::VolumeMainWindow(QWidget *parent) : QMainWindow(parent)
{
	m_volumeWidget = new RenderWidget(nullptr, nullptr, this);
	m_volumeControlWidget = new RenderParameterWidget(m_volumeWidget, this);
	m_sliceControlWidget = new SliceControlWidget(nullptr, m_volumeWidget, this);

	auto  controlWidget = new QWidget(this);
	auto  vLayout = new QVBoxLayout;
	vLayout->addWidget(m_sliceControlWidget);
	vLayout->addWidget(m_volumeControlWidget);
	controlWidget->setLayout(vLayout);
	m_controlDockWidget = new QDockWidget(QStringLiteral("Volume View"), this);
	m_controlDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	m_controlDockWidget->setWidget(controlWidget);
	m_controlDockWidget->setFloating(true);

	m_toolBar = addToolBar(QStringLiteral("toolbar1"));
	auto action = m_controlDockWidget->toggleViewAction();
	action->setIcon(QIcon(":icons/resources/icons/control_panel.png"));
	action->setToolTip(QStringLiteral("Display Contorl Panel"));
	m_toolBar->addAction(action);
	setCentralWidget(m_volumeWidget);
}

void VolumeMainWindow::open(const QString& fileName)
{
	if (fileName.isEmpty())
		return;
	QSharedPointer<MRC> mrc(new MRC(fileName.toStdString()));
	auto * sliceModel = new MRCDataModel(mrc);
	m_volumeWidget->setDataModel(sliceModel);
}

void VolumeMainWindow::openMark(const QString& fileName)
{
	if (fileName.isEmpty())
		return;

	const auto newModel = new MarkModel(fileName);
	m_volumeWidget->setMarkModel(newModel);
}
