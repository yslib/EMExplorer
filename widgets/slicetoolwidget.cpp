#include <QGroupBox>
#include <QVBoxLayout>
#include <QAction>
#include <QCheckBox>
#include <QToolButton>
#include <QLabel>
#include <QComboBox>
#include <QDebug>

#include "slicetoolwidget.h"
#include "titledsliderwithspinbox.h"
#include "sliceeditorwidget.h"
#include "renderwidget.h"
#include "model/mrcdatamodel.h"

#include "slicewidget.h"			//enum SliceType
#include "markcategorydialog.h"
#include "model/categoryitem.h"
#include <QColorDialog>
#include <QTimer>


SliceToolWidget::SliceToolWidget(SliceEditorWidget * canvas, QWidget* parent):
m_canvas(nullptr)
,m_sliceTimer(nullptr)
,m_topSlicePlayDirection(PlayDirection::Forward)
,m_rightSlicePlayDirection(PlayDirection::Forward)
,m_frontSlicePlayDirection(PlayDirection::Forward)
{
	createWidgets();
	setImageCanvas(canvas);
	connections();
	m_sliceTimer = new QTimer(this);
	m_sliceTimer->setInterval(100);
	connect(m_sliceTimer, &QTimer::timeout, this, &SliceToolWidget::onSliceTimer);
}

void SliceToolWidget::setImageCanvas(SliceEditorWidget* canvas)
{
	if (canvas == m_canvas)
		return;
	// disconnect signals
	if(m_canvas != nullptr) 
	{
		disconnect(m_canvas, 0, this, 0);
	}
	m_canvas = canvas;
	connect(m_canvas, &SliceEditorWidget::dataModelChanged, this, &SliceToolWidget::updateDataModel);
	updateDataModel();
}

QString SliceToolWidget::currentCategoryName() const
{
	return m_categoryCBBox->currentText();
}

QColor SliceToolWidget::currentCategoryColor() const
{
	return m_categoryCBBox->currentData().value<QColor>();
}

int SliceToolWidget::categoryCount() const
{
	return m_categoryCBBox->count();
}


void SliceToolWidget::onSliceTimer() {
	qDebug() << "on slice timer";
	int maxSliceCount = 0;
	int curIndex;
	PlayDirection * direction = nullptr;

	const auto m = m_canvas->sliceModel();

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

	m_canvas->setSliceIndex(m_playSliceType, curIndex);
	switch(m_playSliceType) {
	case SliceType::Top:
		m_topSlider->setValue(curIndex);
		break;
	case SliceType::Right:
		m_rightSlider->setValue(curIndex);
		break;
	case SliceType::Front:
		m_frontSlider->setValue(curIndex);
		break;
	}
}

void SliceToolWidget::createWidgets()
{
	QVBoxLayout *mainLayout = new QVBoxLayout;

	//slice slider group
	QGroupBox * group = new QGroupBox(QStringLiteral("Slice Slider"), this);
	QVBoxLayout * vLayout = new QVBoxLayout;
	//top slider
	m_topSliceCheckBox = new QCheckBox;
	m_topSliceCheckBox->setChecked(true);
	m_topSlider = new TitledSliderWithSpinBox(this, tr("Z:"));
	m_topSlicePlayAction = new QToolButton(this);
	m_topSlicePlayAction->setStyleSheet("QToolButton::menu-indicator{image: none;}");
	m_topSlicePlayAction->setIcon(QIcon(":icons/resources/icons/play.png"));
	m_topSlicePlayAction->setToolTip(QStringLiteral("PlayZ"));
	m_topSlicePlayAction->setCheckable(true);

	QHBoxLayout * hLayout = new QHBoxLayout;

	hLayout->addWidget(m_topSliceCheckBox);
	hLayout->addWidget(m_topSlider);
	hLayout->addWidget(m_topSlicePlayAction);
	vLayout->addLayout(hLayout);

	//right slider 
	m_rightSliceCheckBox = new QCheckBox;
	m_rightSliceCheckBox->setChecked(true);
	m_rightSlider = new TitledSliderWithSpinBox(this, tr("Y:"));
	m_rightSlicePlayAction = new QToolButton(this);
	m_rightSlicePlayAction->setToolTip(QStringLiteral("PlayY"));
	m_rightSlicePlayAction->setCheckable(true);
	m_rightSlicePlayAction->setIcon(QIcon(":icons/resources/icons/play.png"));
	m_rightSlicePlayAction->setStyleSheet("QToolButton::menu-indicator{image: none;}");
	hLayout = new QHBoxLayout;
	hLayout->addWidget(m_rightSliceCheckBox);
	hLayout->addWidget(m_rightSlider);
	hLayout->addWidget(m_rightSlicePlayAction);
	vLayout->addLayout(hLayout);

	//front slider
	m_frontSliceCheckBox = new QCheckBox;
	m_frontSliceCheckBox->setChecked(true);
	m_frontSlider = new TitledSliderWithSpinBox(this, tr("X:"));
	m_frontSlicePlayAction = new QToolButton(this);
	m_frontSlicePlayAction->setToolTip(QStringLiteral("PlayZ"));
	m_frontSlicePlayAction->setCheckable(true);
	m_frontSlicePlayAction->setIcon(QIcon(":icons/resources/icons/play.png"));
	m_frontSlicePlayAction->setStyleSheet("QToolButton::menu-indicator{image: none;}");
	hLayout = new QHBoxLayout;
	hLayout->addWidget(m_frontSliceCheckBox);
	hLayout->addWidget(m_frontSlider);
	hLayout->addWidget(m_frontSlicePlayAction);
	vLayout->addLayout(hLayout);

	group->setLayout(vLayout);
	mainLayout->addWidget(group);

	//View Group
	group = new QGroupBox(QStringLiteral("View"), this);
	hLayout = new QHBoxLayout;

	m_zoomInAction = new QToolButton(this);
	m_zoomInAction->setToolTip(QStringLiteral("Zoom In"));
	m_zoomInAction->setStyleSheet("QToolButton::menu-indicator{image: none;}");
	m_zoomInAction->setIcon(QIcon(":icons/resources/icons/zoom_in.png"));
	hLayout->addWidget(m_zoomInAction);

	m_zoomOutAction = new QToolButton(this);
	m_zoomOutAction->setToolTip(QStringLiteral("Zoom Out"));
	m_zoomOutAction->setStyleSheet("QToolButton::menu-indicator{image: none;}");
	m_zoomOutAction->setIcon(QIcon(":icons/resources/icons/zoom_out.png"));
	hLayout->addWidget(m_zoomOutAction);

	m_resetAction = new QToolButton(this);
	m_resetAction->setToolTip(QStringLiteral("Reset"));
	m_resetAction->setStyleSheet("QToolButton::menu-indicator{image: none;}");
	m_resetAction->setIcon(QIcon(":icons/resources/icons/reset.png"));

	hLayout->addWidget(m_resetAction);
	hLayout->setAlignment(Qt::AlignLeft);
	group->setLayout(hLayout);
	mainLayout->addWidget(group);

	//Mark Group
	group = new QGroupBox(QStringLiteral("Mark"), this);
	vLayout = new QVBoxLayout;

	//Categroy
	m_categoryLabel = new QLabel(QStringLiteral("Category:"), this);
	m_categoryCBBox = new QComboBox(this);
	m_addCategoryAction = new QToolButton(this);
	m_addCategoryAction->setToolTip(QStringLiteral("Add Category"));
	m_addCategoryAction->setStyleSheet("QToolButton::menu-indicator{image: none;}");
	m_addCategoryAction->setIcon(QIcon(":icons/resources/icons/add.png"));

	hLayout = new QHBoxLayout;
	hLayout->addWidget(m_categoryLabel);
	hLayout->addWidget(m_categoryCBBox);
	hLayout->addWidget(m_addCategoryAction);
	vLayout->addLayout(hLayout);
	//pen size and color
	m_penSizeLabel = new QLabel(QStringLiteral("PenSize:"), this);
	m_penSizeCBBox = new QComboBox(this);
	for (int i = 1; i <= 30; i++)
		m_penSizeCBBox->addItem(QString::number(i), QVariant::fromValue(i));
	m_colorAction = new QToolButton(this);
	m_colorAction->setToolTip(QStringLiteral("Select Color"));
	m_colorAction->setStyleSheet("QToolButton::menu-indicator{image: none;}");
	m_colorAction->setIcon(QIcon(":icons/resources/icons/color.png"));
	hLayout = new QHBoxLayout;
	hLayout->addWidget(m_penSizeLabel);
	hLayout->addWidget(m_penSizeCBBox);
	hLayout->addWidget(m_colorAction);
	vLayout->addLayout(hLayout);

	//Mark pen
	hLayout = new QHBoxLayout;

	m_markAction = new QToolButton(this);
	m_markAction->setToolTip(QStringLiteral("Mark"));
	m_markAction->setCheckable(true);
	m_markAction->setStyleSheet("QToolButton::menu-indicator{image: none;}");
	m_markAction->setIcon(QIcon(":icons/resources/icons/mark.png"));
	m_markAction->setCheckable(true);
	hLayout->addWidget(m_markAction);
	//selection
	m_markSelectionAction = new QToolButton(this);
	m_markSelectionAction->setToolTip(QStringLiteral("Select Mark"));
	m_markSelectionAction->setCheckable(true);
	m_markSelectionAction->setStyleSheet("QToolButton::menu-indicator{image: none;}");
	m_markSelectionAction->setIcon(QIcon(":icons/resources/icons/select.png"));
	hLayout->addWidget(m_markSelectionAction);
	//deletion
	m_markDeletionAction = new QToolButton(this);
	m_markDeletionAction->setToolTip(QStringLiteral("Delete Mark"));
	m_markDeletionAction->setStyleSheet("QToolButton::menu-indicator{image: none;}");
	m_markDeletionAction->setIcon(QIcon(":icons/resources/icons/delete.png"));
	hLayout->addWidget(m_markDeletionAction);
	hLayout->setAlignment(Qt::AlignLeft);

	vLayout->addLayout(hLayout);
	group->setLayout(vLayout);
	mainLayout->addWidget(group);
	mainLayout->addStretch();

	setLayout(mainLayout);
}



void SliceToolWidget::updateDataModel()
{
	if (m_canvas == nullptr)
		return;
	const auto m = m_canvas->sliceModel();
	if (m == nullptr) {
		setEnabled(false);
		return;
	}

	const auto miz = m->topSliceCount();
	const auto miy = m->rightSliceCount();
	const auto mix = m->frontSliceCount();

	m_topSlider->setMaximum(miz - 1);
	m_rightSlider->setMaximum(miy - 1);
	m_frontSlider->setMaximum(mix - 1);
	setEnabled(true);

	//	Add category
	const auto cates = m_canvas->categories();
	m_categoryCBBox->clear();
	foreach(const auto & c,cates) {
		m_categoryCBBox->addItem(c);
	}
	if(m_categoryCBBox->count() == 0) {
		m_categoryCBBox->addItem(QStringLiteral("Default"));
		m_categoryCBBox->setCurrentText(QStringLiteral("Default"));
	}
}


void SliceToolWidget::connections()
{
	connect(m_topSlider,   &TitledSliderWithSpinBox::valueChanged, [this](int value) {m_canvas->setSliceIndex(SliceType::Top, value); });
	connect(m_rightSlider, &TitledSliderWithSpinBox::valueChanged, [this](int value) {m_canvas->setSliceIndex(SliceType::Right, value); });
	connect(m_frontSlider, &TitledSliderWithSpinBox::valueChanged, [this](int value) {m_canvas->setSliceIndex(SliceType::Front, value); });

	connect(m_topSlider,   &TitledSliderWithSpinBox::valueChanged, this,&SliceToolWidget::topSliceIndexChanged);
	connect(m_rightSlider, &TitledSliderWithSpinBox::valueChanged, this,&SliceToolWidget::rightSliceIndexChanged);
	connect(m_frontSlider, &TitledSliderWithSpinBox::valueChanged, this,&SliceToolWidget::frontSliceIndexChanged);

	connect(m_topSliceCheckBox, &QCheckBox::toggled, m_canvas, &SliceEditorWidget::updateTopSliceActions);
	connect(m_rightSliceCheckBox, &QCheckBox::toggled, m_canvas, &SliceEditorWidget::updateRightSliceActions);
	connect(m_frontSliceCheckBox, &QCheckBox::toggled, m_canvas, &SliceEditorWidget::updateFrontSliceActions);

	connect(m_resetAction, &QToolButton::clicked, m_canvas, &SliceEditorWidget::resetZoom);
	//view toolbar actions
	connect(m_topSlicePlayAction, &QToolButton::toggled, [this](bool enable) {
		m_sliceTimer->stop();
		m_rightSlicePlayAction->blockSignals(true);
		m_rightSlicePlayAction->toggled(false);
		m_rightSlicePlayAction->blockSignals(false);
		m_frontSlicePlayAction->blockSignals(true);
		m_frontSlicePlayAction->toggled(false);
		m_frontSlicePlayAction->blockSignals(false);
		if (enable == false)
			return;
		m_playSliceType = SliceType::Top;
		m_sliceTimer->start();
	});
	connect(m_rightSlicePlayAction, &QToolButton::toggled, [this](bool enable) {
		m_sliceTimer->stop();
		m_topSlicePlayAction->blockSignals(true);
		m_topSlicePlayAction->toggled(false);
		m_topSlicePlayAction->blockSignals(false);
		m_frontSlicePlayAction->blockSignals(true);
		m_frontSlicePlayAction->toggled(false);
		m_frontSlicePlayAction->blockSignals(false);
		if (enable == false)
			return;
		m_playSliceType = SliceType::Right;
		m_sliceTimer->start();
	});
	connect(m_frontSlicePlayAction, &QToolButton::toggled, [this](bool enable) {
		m_sliceTimer->stop();
		m_topSlicePlayAction->blockSignals(true);
		m_topSlicePlayAction->toggled(false);
		m_topSlicePlayAction->blockSignals(false);
		m_rightSlicePlayAction->blockSignals(true);
		m_rightSlicePlayAction->toggled(false);
		m_rightSlicePlayAction->blockSignals(false);
		if (enable == false)
			return;
		m_playSliceType = SliceType::Front;
		m_sliceTimer->start();
	});
	connect(m_addCategoryAction, &QToolButton::clicked, this, &SliceToolWidget::onCategoryAdded);
	connect(m_zoomInAction, &QToolButton::clicked, m_canvas, &SliceEditorWidget::zoomIn);
	connect(m_zoomOutAction, &QToolButton::clicked, m_canvas, &SliceEditorWidget::zoomOut);
	connect(m_penSizeCBBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int) {QPen pen = m_canvas->m_topView->pen(); pen.setWidth(m_penSizeCBBox->currentData().toInt()); m_canvas->setPen(pen); });
	connect(m_colorAction, &QToolButton::clicked, this, &SliceToolWidget::colorChanged);

	connect(m_markAction, &QToolButton::toggled, [this](bool enable)
	{
		if (enable == true)
		{
			m_canvas->m_topView->setOperation(SliceWidget::Paint);
			m_canvas->m_rightView->setOperation(SliceWidget::Paint);
			m_canvas->m_frontView->setOperation(SliceWidget::Paint);
			//m_moveAction->setChecked(false);
			m_markSelectionAction->setChecked(false);
		}
		else
		{
			m_canvas->m_topView->setOperation(SliceWidget::Move);
			m_canvas->m_rightView->setOperation(SliceWidget::Move);
			m_canvas->m_frontView->setOperation(SliceWidget::Move);
		}
	});
	connect(m_markSelectionAction, &QToolButton::toggled, [this](bool enable)
	{
		if (enable == true)
		{
			m_canvas->m_topView->setOperation(SliceWidget::Selection);
			m_canvas->m_rightView->setOperation(SliceWidget::Selection);
			m_canvas->m_frontView->setOperation(SliceWidget::Selection);
			m_markAction->setChecked(false);
			//m_moveAction->setChecked(false);
		}
		else
		{
			m_canvas->m_topView->setOperation(SliceWidget::Move);
			m_canvas->m_rightView->setOperation(SliceWidget::Move);
			m_canvas->m_frontView->setOperation(SliceWidget::Move);
		}

	});
	connect(m_markDeletionAction, &QToolButton::clicked, [this](bool enable) {Q_UNUSED(enable); m_canvas->markDeleteHelper(); m_canvas->updateDeleteAction(); });

}

void SliceToolWidget::setCategoryInfoPrivate(const QVector<QPair<QString, QColor>>& cates)
{
	Q_ASSERT_X(m_categoryCBBox, "ImageVIew::initCCBoxHelper", "null pointer");
	m_categoryCBBox->clear();
	foreach(const auto & p, cates)
		m_categoryCBBox->addItem(p.first, QVariant::fromValue(p.second));

}

void SliceToolWidget::addCategoryInfoPrivate(const QString & name, const QColor & color)
{
	m_categoryCBBox->addItem(name, color);
	m_categoryCBBox->setCurrentText(name);
	//auto ci = CategoryInfo(name, color);
	m_canvas->addCategory(CategoryInfo(name,color));
	m_canvas->setCurrentCategory(name);
}

void SliceToolWidget::updateDeleteActionPrivate()
{
	const bool enable = m_canvas && m_canvas->markModel()
		&& (m_canvas->topView()->selectedItemCount()
			|| m_canvas->rightView()->selectedItemCount()
			|| m_canvas->frontView()->selectedItemCount());
	m_markDeletionAction->setEnabled(enable);
}

void SliceToolWidget::updateSliceActions(SliceType type, bool checked)
{
	bool enable = checked && m_canvas&& m_canvas->sliceModel();
	const auto vp1 = m_canvas->topView();
	const auto vp2 = m_canvas->rightView();
	const auto vp3 = m_canvas->frontView();
	Q_ASSERT_X(vp1 && vp2 && vp3, "ImageViewControlPanel::updateSliceActions", "null pointer");
	switch (type)
	{
	case SliceType::Top:
		m_topSlicePlayAction->setEnabled(enable);
		m_topSlider->setEnabled(enable);
		vp1->setHidden(!enable);
		break;
	case SliceType::Right:
		m_frontSlicePlayAction->setEnabled(enable);
		m_frontSlider->setEnabled(enable);
		vp2->setHidden(!enable);
		break;
	case SliceType::Front:
		m_frontSlicePlayAction->setEnabled(enable);
		m_frontSlider->setEnabled(enable);
		vp3->setHidden(!enable);
		break;
	}
}

QIcon SliceToolWidget::createColorIcon(const QColor& color)
{
	constexpr int iconSize = 64;
	constexpr int barHeight = 20;

	QPixmap pixmap(iconSize, iconSize + barHeight);
	pixmap.fill(Qt::transparent);
	QPainter painter(&pixmap);
	QPixmap image(":icons/resources/icons/color.png");
	// Draw icon centred horizontally on button.
	QRect target(0, 0, iconSize, iconSize);
	painter.fillRect(QRect(0, iconSize, iconSize, barHeight), color);
	painter.drawPixmap(target, image);
	return QIcon(pixmap);
}

void SliceToolWidget::onCategoryAdded()
{
	MarkCategoryDialog dlg(this);
	connect(&dlg, &MarkCategoryDialog::resultReceived, [this](const QString & name, const QColor & color)
	{
		addCategoryInfoPrivate(name, color);
		QPen pen = m_canvas->m_topView->pen();
		pen.setColor(color);
		m_canvas->setPen(pen);
		///TODO:: This color need to be add categoryItem

	});
	dlg.exec();
}

void SliceToolWidget::colorChanged()
{
	Q_ASSERT_X(m_canvas, "ImageViewControlPanel::colorChanged", "null pointer");

	auto d = m_categoryCBBox->itemData(m_categoryCBBox->currentIndex());
	QColor defaultColor = d.canConvert<QColor>() ? d.value<QColor>() : Qt::black;
	QPen pen = m_canvas->pen();
	pen.setColor(QColorDialog::getColor(defaultColor, this, QStringLiteral("Color")));
	m_canvas->setPen(pen);
}

