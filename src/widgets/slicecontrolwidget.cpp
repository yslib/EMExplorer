#include "slicecontrolwidget.h"

#include <QCheckBox>
#include <QVBoxLayout>
#include <QToolButton>
#include <QGroupBox>
#include <QTimer>
#include <QButtonGroup>
#include <QLabel>
#include <QComboBox>

#include "globals.h"
#include "widgets/titledsliderwithspinbox.h"
#include "abstract/abstractslicedatamodel.h"
#include "widgets/sliceeditorwidget.h"
#include "widgets/renderwidget.h"


/**
 * \brief Constructs a widget by the given \a sliceWidget and \a volumeWidget
 */
SliceControlWidget::SliceControlWidget(SliceEditorWidget* sliceWidget, RenderWidget* volumeWidget, QWidget* parent) :
	QWidget(parent)
	, m_volumeWidget(nullptr)
	, m_sliceWidget(nullptr)
	, m_sliceTimer(nullptr)
	, m_topSlicePlayDirection(PlayDirection::Forward)
	, m_rightSlicePlayDirection(PlayDirection::Forward)
	, m_frontSlicePlayDirection(PlayDirection::Forward)
{

	// Widgets

	auto * group = new QGroupBox(QStringLiteral("Axis-aligned Slice"), this);
	auto * vLayout = new QVBoxLayout;
	//top slider
	m_topSliceCheckBox = new QCheckBox;
	m_topSliceCheckBox->setChecked(true);


	m_topSlider = new TitledSliderWithSpinBox(this, tr("Z:"));
	m_topSlicePlayAction = new QToolButton(this);
	m_topSlicePlayAction->setStyleSheet("QToolButton::menu-indicator{image: none;}");
	m_topSlicePlayAction->setIcon(QIcon(":icons/resources/icons/play.png"));
	m_topSlicePlayAction->setToolTip(QStringLiteral("PlayZ"));
	m_topSlicePlayAction->setCheckable(true);

	auto * hTopLayout = new QHBoxLayout;

	hTopLayout->addWidget(m_topSliceCheckBox);
	hTopLayout->addWidget(m_topSlider);
	hTopLayout->addWidget(m_topSlicePlayAction);
	vLayout->addLayout(hTopLayout);

	//right slider 
	m_rightSliceCheckBox = new QCheckBox;
	m_rightSliceCheckBox->setChecked(true);
	m_rightSlider = new TitledSliderWithSpinBox(this, tr("Y:"));
	m_rightSlicePlayAction = new QToolButton(this);

	m_rightSlicePlayAction->setToolTip(QStringLiteral("PlayY"));
	m_rightSlicePlayAction->setCheckable(true);
	m_rightSlicePlayAction->setIcon(QIcon(":icons/resources/icons/play.png"));
	m_rightSlicePlayAction->setStyleSheet("QToolButton::menu-indicator{image: none;}");
	auto hRightLayout = new QHBoxLayout;
	hRightLayout->addWidget(m_rightSliceCheckBox);
	hRightLayout->addWidget(m_rightSlider);
	hRightLayout->addWidget(m_rightSlicePlayAction);
	vLayout->addLayout(hRightLayout);

	//front slider
	m_frontSliceCheckBox = new QCheckBox;
	m_frontSliceCheckBox->setChecked(true);
	m_frontSlider = new TitledSliderWithSpinBox(this, tr("X:"));
	m_frontSlicePlayAction = new QToolButton(this);
	m_frontSlicePlayAction->setToolTip(QStringLiteral("PlayZ"));
	m_frontSlicePlayAction->setCheckable(true);
	m_frontSlicePlayAction->setIcon(QIcon(":icons/resources/icons/play.png"));
	m_frontSlicePlayAction->setStyleSheet("QToolButton::menu-indicator{image: none;}");
	auto hFrontLayout = new QHBoxLayout;
	hFrontLayout->addWidget(m_frontSliceCheckBox);
	hFrontLayout->addWidget(m_frontSlider);
	hFrontLayout->addWidget(m_frontSlicePlayAction);
	vLayout->addLayout(hFrontLayout);
	// Stop Action
	m_stopAction = new QToolButton(this);
	m_stopAction->setToolTip(QStringLiteral("Stop"));
	m_stopAction->setCheckable(true);
	m_stopAction->setChecked(true);
	m_stopAction->setIcon(QIcon(":icons/resources/icons/pause.png"));
	m_stopAction->setStyleSheet("QToolButton::menu-indicator{image: none;}");
	connect(m_stopAction, &QToolButton::toggled, [this](bool enable) {m_sliceTimer->stop(); });
	auto hStopActionLayout = new QHBoxLayout;


	// Interval widget
	m_intervalLabel = new QLabel(QStringLiteral("Interval:"), this);
	m_intervalCBBox = new QComboBox(this);
	for (int i = 1; i <= 10; i++)
		m_intervalCBBox->addItem(QString::number(i * 10) + QStringLiteral("ms"), i * 10);
	m_intervalCBBox->setCurrentText(QStringLiteral("50ms"));
	connect(m_intervalCBBox, &QComboBox::currentTextChanged, [this](const QString & text) {m_sliceTimer->setInterval(m_intervalCBBox->currentData().toInt()); });
	hStopActionLayout->addWidget(m_intervalLabel, Qt::AlignLeft);
	hStopActionLayout->addWidget(m_intervalCBBox, Qt::AlignLeft);
	hStopActionLayout->addWidget(m_stopAction, Qt::AlignRight);
	vLayout->addLayout(hStopActionLayout);


	group->setLayout(vLayout);

	auto mainLayout = new QVBoxLayout;
	mainLayout->addWidget(group);

	setLayout(mainLayout);

	// Timer
	m_sliceTimer = new QTimer(this);
	m_sliceTimer->setInterval(100);
	connect(m_sliceTimer, &QTimer::timeout, this, &SliceControlWidget::onSliceTimer);

	// Play action connections
	connect(m_topSlicePlayAction, &QToolButton::toggled, [this](bool enable) {
		m_sliceTimer->stop();
		m_playSliceType = SliceType::Top;
		m_sliceTimer->start();
	});
	connect(m_rightSlicePlayAction, &QToolButton::toggled, [this](bool enable) {
		m_sliceTimer->stop();
		m_playSliceType = SliceType::Right;
		m_sliceTimer->start();
	});
	connect(m_frontSlicePlayAction, &QToolButton::toggled, [this](bool enable) {
		m_sliceTimer->stop();
		m_playSliceType = SliceType::Front;
		m_sliceTimer->start();
	});


	m_playButtonGroup = new QButtonGroup(this);

	m_playButtonGroup->setExclusive(true);
	m_playButtonGroup->addButton(m_topSlicePlayAction);
	m_playButtonGroup->addButton(m_rightSlicePlayAction);
	m_playButtonGroup->addButton(m_frontSlicePlayAction);
	m_playButtonGroup->addButton(m_stopAction);


	setControlledWidget(sliceWidget, volumeWidget);

}


/**
 * \brief Sets the controlled widget \a sliceWidge and \a volumeWidget to present
 * 
 * \note Only when the slice data models of the two widgets i.e. \a SliceEditorWidget and \a RenderWidget are the same,
 *		 the widget state will be set as \a enabled 
 * \sa SliceEditorWidget RenderWidget AbstractSliceDataModel
 */
void SliceControlWidget::setControlledWidget(SliceEditorWidget* sliceWidget, RenderWidget* volumeWidget) {

	if (m_sliceWidget == sliceWidget && m_volumeWidget == volumeWidget)
		return;

	// disconnect all signals related to m_sliceWidget
	if (m_sliceWidget != nullptr) {

		disconnect(m_sliceWidget, 0, this, 0);
		disconnect(m_topSliceCheckBox, 0, m_sliceWidget, 0);
		disconnect(m_rightSliceCheckBox, 0, m_sliceWidget, 0);
		disconnect(m_frontSliceCheckBox, 0, m_sliceWidget, 0);
		disconnect(m_topSlider, 0, m_sliceWidget, 0);
		disconnect(m_rightSlider, 0, m_sliceWidget, 0);
		disconnect(m_frontSlider, 0, m_sliceWidget, 0);
	}
	// disconnect all signals related to m_volumeWidget
	if (m_volumeWidget != nullptr) {

		disconnect(m_volumeWidget, 0, this, 0);
		disconnect(m_topSliceCheckBox, 0, m_volumeWidget, 0);
		disconnect(m_rightSliceCheckBox, 0, m_volumeWidget, 0);
		disconnect(m_frontSliceCheckBox, 0, m_volumeWidget, 0);
		disconnect(m_topSlider, 0, m_volumeWidget, 0);
		disconnect(m_rightSlider, 0, m_volumeWidget, 0);
		disconnect(m_frontSlider, 0, m_volumeWidget, 0);
	}

	// Connect all signals related to slice widget
	m_sliceWidget = sliceWidget;
	if (m_sliceWidget != nullptr) {
		connect(m_sliceWidget, &SliceEditorWidget::dataModelChanged, this, &SliceControlWidget::updateDataModel);
		connect(m_topSliceCheckBox, &QCheckBox::toggled, m_sliceWidget, &SliceEditorWidget::setTopSliceVisibility);
		connect(m_rightSliceCheckBox, &QCheckBox::toggled, m_sliceWidget, &SliceEditorWidget::setRightSliceVisibility);
		connect(m_frontSliceCheckBox, &QCheckBox::toggled, m_sliceWidget, &SliceEditorWidget::setFrontSliceVisibility);
		connect(m_topSlider, &TitledSliderWithSpinBox::valueChanged, [this](int value) {m_sliceWidget->setSliceIndex(SliceType::Top, value); });
		connect(m_rightSlider, &TitledSliderWithSpinBox::valueChanged, [this](int value) {m_sliceWidget->setSliceIndex(SliceType::Right, value); });
		connect(m_frontSlider, &TitledSliderWithSpinBox::valueChanged, [this](int value) {m_sliceWidget->setSliceIndex(SliceType::Front, value); });
	}

	// Connect all signals related to volume widget
	m_volumeWidget = volumeWidget;
	if (m_volumeWidget != nullptr) {
		connect(volumeWidget, &RenderWidget::dataModelChanged, this, &SliceControlWidget::updateDataModel);
		connect(m_topSliceCheckBox, &QCheckBox::toggled, m_volumeWidget, &RenderWidget::setTopSliceVisible);
		connect(m_rightSliceCheckBox, &QCheckBox::toggled, m_volumeWidget, &RenderWidget::setRightSliceVisible);
		connect(m_frontSliceCheckBox, &QCheckBox::toggled, m_volumeWidget, &RenderWidget::setFrontSliceVisible);
		connect(m_topSlider, &TitledSliderWithSpinBox::valueChanged, m_volumeWidget, &RenderWidget::setTopSlice);
		connect(m_rightSlider, &TitledSliderWithSpinBox::valueChanged, m_volumeWidget, &RenderWidget::setFrontSlice);
		connect(m_frontSlider, &TitledSliderWithSpinBox::valueChanged, m_volumeWidget, &RenderWidget::setRightSlice);

	}

	updateDataModel();
}

void SliceControlWidget::onSliceTimer() {
	int maxSliceCount = 0;
	int curIndex;
	PlayDirection * direction = nullptr;

	switch (m_playSliceType) {
	case SliceType::Top:
		maxSliceCount = m_topSlider->maximum();
		curIndex = m_topSlider->value();
		direction = &m_topSlicePlayDirection;
		break;
	case SliceType::Right:
		maxSliceCount = m_rightSlider->maximum();
		curIndex = m_rightSlider->value();
		direction = &m_rightSlicePlayDirection;
		break;
	case SliceType::Front:
		maxSliceCount = m_frontSlider->maximum();
		curIndex = m_frontSlider->value();
		direction = &m_frontSlicePlayDirection;
		break;
	default:
		return;
	}

	if ((*direction) == PlayDirection::Forward)
		(curIndex)++;
	else
		(curIndex)--;
	if (curIndex == maxSliceCount)
		(*direction) = PlayDirection::Backward;
	else if ((curIndex) == 0)
		(*direction) = PlayDirection::Forward;

	switch (m_playSliceType) {
	case SliceType::Top:
		m_topSlider->setValue(curIndex);			// This will emit signal to change index
		break;
	case SliceType::Right:
		m_rightSlider->setValue(curIndex);
		break;
	case SliceType::Front:
		m_frontSlider->setValue(curIndex);
		break;
	}
}

/**
 * \brief This is a private function for updating widget state after changing the controlled widget
 * 
 * Only when the slice data models of the tow widgets i.e. \a SliceEditorWidget and \a RenderWidget are same,
 * the widget state will be set as \a enabled
 */
void SliceControlWidget::updateDataModel() {

	AbstractSliceDataModel * model = nullptr;
	bool valid = false;

	if (m_sliceWidget == nullptr && m_volumeWidget == nullptr) {
		valid = false;
		model = nullptr;
	}
	else if (m_sliceWidget != nullptr && m_volumeWidget != nullptr) {
		valid = m_sliceWidget->sliceModel() == m_volumeWidget->dataModel();
		model = m_sliceWidget->sliceModel();
	}
	else if (m_sliceWidget != nullptr && m_volumeWidget == nullptr) {
		valid = true;
		model = m_sliceWidget->sliceModel();
	}
	else if (m_sliceWidget == nullptr && m_volumeWidget != nullptr) {
		valid = true;
		model = m_volumeWidget->dataModel();
	}
	setEnabled(valid && model);
	if (!valid || !model)
		return;

	const auto miz = model->topSliceCount();
	const auto miy = model->rightSliceCount();
	const auto mix = model->frontSliceCount();

	m_topSlider->setMaximum(miz - 1);
	m_rightSlider->setMaximum(miy - 1);
	m_frontSlider->setMaximum(mix - 1);
}

