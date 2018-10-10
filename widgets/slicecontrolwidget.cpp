#include "slicecontrolwidget.h"

#include <QCheckBox>
#include <QVBoxLayout>
#include <QToolButton>
#include <QGroupBox>
#include <QTimer>
#include <QButtonGroup>

#include "globals.h"
#include "widgets/titledsliderwithspinbox.h"
#include "abstract/abstractslicedatamodel.h"
#include "widgets/sliceeditorwidget.h"
#include "widgets/renderwidget.h"


SliceControlWidget::SliceControlWidget(SliceEditorWidget* sliceWidget, RenderWidget* volumeWidget,QWidget* parent):
QWidget(parent)
,m_volumeWidget(nullptr)
,m_sliceWidget(nullptr)
, m_sliceTimer(nullptr)
, m_topSlicePlayDirection(PlayDirection::Forward)
, m_rightSlicePlayDirection(PlayDirection::Forward)
, m_frontSlicePlayDirection(PlayDirection::Forward)
{

	// Widgets

	auto * group = new QGroupBox(QStringLiteral("Slice Slider"), this);
	auto * vLayout = new QVBoxLayout;
	//top slider
	m_topSliceCheckBox = new QCheckBox;
	m_topSliceCheckBox->setChecked(true);

	connect(m_topSliceCheckBox, &QCheckBox::toggled, m_sliceWidget, &SliceEditorWidget::updateTopSliceActions);

	///TODO:: add connection with slice visiblity function in RenderWidget

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
	connect(m_rightSliceCheckBox, &QCheckBox::toggled, m_sliceWidget, &SliceEditorWidget::updateRightSliceActions);
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
	connect(m_frontSliceCheckBox, &QCheckBox::toggled, m_sliceWidget, &SliceEditorWidget::updateFrontSliceActions);
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
		if (enable == false)
			return;
		m_playSliceType = SliceType::Top;
		m_sliceTimer->start();
	});
	connect(m_rightSlicePlayAction, &QToolButton::toggled, [this](bool enable) {
		m_sliceTimer->stop();
		if (enable == false)
			return;
		m_playSliceType = SliceType::Right;
		m_sliceTimer->start();
	});
	connect(m_frontSlicePlayAction, &QToolButton::toggled, [this](bool enable) {
		m_sliceTimer->stop();
		if (enable == false)
			return;
		m_playSliceType = SliceType::Front;
		m_sliceTimer->start();
	});


	m_playButtonGroup = new QButtonGroup(this);

	m_playButtonGroup->setExclusive(true);
	m_playButtonGroup->addButton(m_topSlicePlayAction);
	m_playButtonGroup->addButton(m_rightSlicePlayAction);
	m_playButtonGroup->addButton(m_frontSlicePlayAction);


	setSliceModel(sliceWidget,volumeWidget);

}


void SliceControlWidget::setSliceModel(SliceEditorWidget* sliceWidget, RenderWidget* volumeWidget) {

	if (m_sliceWidget == sliceWidget && m_volumeWidget == volumeWidget)
		return;

	bool valid=false;
	AbstractSliceDataModel * model = nullptr;

	if (sliceWidget == nullptr && volumeWidget == nullptr) {
		valid = false;
		model = nullptr;
	}else if(sliceWidget != nullptr && volumeWidget != nullptr) {
		valid = m_sliceWidget->sliceModel() == m_volumeWidget->dataModel();
		model = m_sliceWidget->sliceModel();
	}else if(sliceWidget != nullptr && volumeWidget == nullptr) {
		valid = true;
		model = sliceWidget->sliceModel();
	}else if(sliceWidget == nullptr && volumeWidget != nullptr){
		valid = true;
		model = volumeWidget->dataModel();
	}
	m_sliceWidget = sliceWidget;
	m_volumeWidget = volumeWidget;
	setEnabled(valid);
	if (valid == false)
		return;

	
	const auto miz = model->topSliceCount();
	const auto miy = model->rightSliceCount();
	const auto mix = model->frontSliceCount();

	m_topSlider->setMaximum(miz - 1);
	m_rightSlider->setMaximum(miy - 1);
	m_frontSlider->setMaximum(mix - 1);

}

void SliceControlWidget::onSliceTimer() {
	int maxSliceCount = 0;
	int curIndex;
	PlayDirection * direction = nullptr;

	const auto m = m_dataModel;

	switch (m_playSliceType) {
	case SliceType::Top:
		maxSliceCount = m->topSliceCount();
		curIndex = m_topSlider->value();
		direction = &m_topSlicePlayDirection;
		break;
	case SliceType::Right:
		maxSliceCount = m->rightSliceCount();
		curIndex = m_rightSlider->value();
		direction = &m_rightSlicePlayDirection;
		break;
	case SliceType::Front:
		maxSliceCount = m->frontSliceCount();
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
	if (curIndex == maxSliceCount - 1)
		(*direction) = PlayDirection::Backward;
	else if ((curIndex) == 0)
		(*direction) = PlayDirection::Forward;

	if(m_sliceWidget != nullptr)
		m_sliceWidget->setSliceIndex(m_playSliceType, curIndex);

	
	switch (m_playSliceType) {
		case SliceType::Top:
			{
				m_topSlider->setValue(curIndex);		
				if(m_volumeWidget != nullptr) {
					m_volumeWidget->setTopSlice(curIndex);
				}
			}
			break;
		case SliceType::Right: 
			{
				m_rightSlider->setValue(curIndex);
				if (m_volumeWidget != nullptr) {
					m_volumeWidget->setRightSlice(curIndex);
				}
			}
			break;
		case SliceType::Front: 
			{
				m_frontSlider->setValue(curIndex);
				if (m_volumeWidget != nullptr) {
					m_volumeWidget->setFrontSlice(curIndex);
				}
			}
			break;
	}
}
