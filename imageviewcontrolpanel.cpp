#include <QGroupBox>
#include <QVBoxLayout>
#include <QAction>
#include <QCheckBox>
#include <QToolButton>
#include <QLabel>
#include <QComboBox>

#include "imageviewcontrolpanel.h"
#include "titledsliderwithspinbox.h"



ImageViewControlPanel::ImageViewControlPanel(ImageCanvas * canvas,QWidget* parent):m_canvas(canvas)
{
	createWidgets();
}

void ImageViewControlPanel::createWidgets()
{
	QVBoxLayout *mainLayout = new QVBoxLayout;

	//slice slider group
	QGroupBox * group = new QGroupBox(QStringLiteral("Slice Slider"), this);
	QVBoxLayout * vLayout = new QVBoxLayout;
	m_topSliceCheckBox = new QCheckBox;
	m_topSliceCheckBox->setChecked(true);
	m_topSlider = new TitledSliderWithSpinBox(this, tr("Z:"));
	m_topSlicePlayAction = new QAction(QStringLiteral("PlayX"), this);
	m_topSlicePlayAction->setToolTip(QStringLiteral("PlayX"));
	QToolButton *toolButton = new QToolButton(this);
	toolButton->setStyleSheet("QToolButton::menu-indicator{image: none;}");
	toolButton->setIcon(QIcon(":icons/resources/icons/play.png"));
	toolButton->setCheckable(true);
	QHBoxLayout * hLayout = new QHBoxLayout;
	toolButton->addAction(m_topSlicePlayAction);

	hLayout->addWidget(m_topSliceCheckBox);
	hLayout->addWidget(m_topSlider);
	hLayout->addWidget(toolButton);
	vLayout->addLayout(hLayout);

	m_rightSliceCheckBox = new QCheckBox;
	m_rightSliceCheckBox->setChecked(true);
	m_rightSlider = new TitledSliderWithSpinBox(this, tr("Y:"));
	m_rightSlicePlayAction = new QAction( QStringLiteral("PlayY"), this);
	m_rightSlicePlayAction->setToolTip(QStringLiteral("PlayY"));
	m_rightSlicePlayAction->setCheckable(true);
	toolButton = new QToolButton(this);
	toolButton->setStyleSheet("QToolButton::menu-indicator{image: none;}");
	toolButton->setIcon(QIcon(":icons/resources/icons/play.png"));
	toolButton->setCheckable(true);
	hLayout = new QHBoxLayout;
	toolButton->addAction(m_rightSlicePlayAction);
	hLayout->addWidget(m_rightSliceCheckBox);
	hLayout->addWidget(m_rightSlider);
	hLayout->addWidget(toolButton);
	vLayout->addLayout(hLayout);


	m_frontSliceCheckBox = new QCheckBox;
	m_frontSliceCheckBox->setChecked(true);
	m_frontSlider = new TitledSliderWithSpinBox(this, tr("X:"));
	m_frontSlicePlayAction = new QAction(QStringLiteral("PlayZ"), this);
	m_frontSlicePlayAction->setToolTip(QStringLiteral("PlayZ"));
	m_frontSlicePlayAction->setCheckable(true);
	toolButton = new QToolButton(this);
	toolButton->setStyleSheet("QToolButton::menu-indicator{image: none;}");
	toolButton->setIcon(QIcon(":icons/resources/icons/play.png"));
	toolButton->setCheckable(true);
	hLayout = new QHBoxLayout;
	toolButton->addAction(m_frontSlicePlayAction);
	hLayout->addWidget(m_frontSliceCheckBox);
	hLayout->addWidget(m_frontSlider);
	hLayout->addWidget(toolButton);
	vLayout->addLayout(hLayout);

	group->setLayout(vLayout);
	mainLayout->addWidget(group);

	//View Group
	group = new QGroupBox(QStringLiteral("View"), this);
	hLayout = new QHBoxLayout;
	
	m_zoomInAction = new QAction(QIcon(":icons/resources/icons/zoom_in.png"), QStringLiteral("Zoom In"), this);
	m_zoomInAction->setToolTip(QStringLiteral("Zoom In"));
	toolButton = new QToolButton(this);
	toolButton->setStyleSheet("QToolButton::menu-indicator{image: none;}");
	toolButton->setIcon(QIcon(":icons/resources/icons/zoom_in.png"));
	toolButton->addAction(m_zoomInAction);
	hLayout->addWidget(toolButton);

	m_zoomOutAction = new QAction(QIcon(":icons/resources/icons/zoom_out.png"), QStringLiteral("Zoom Out"), this);
	m_zoomOutAction->setToolTip(QStringLiteral("Zoom Out"));
	toolButton = new QToolButton(this);
	toolButton->setStyleSheet("QToolButton::menu-indicator{image: none;}");
	toolButton->setIcon(QIcon(":icons/resources/icons/zoom_out.png"));
	toolButton->addAction(m_zoomOutAction);
	hLayout->addWidget(toolButton);

	m_resetAction = new QAction(QIcon(":icons/resources/icons/reset.png"), QStringLiteral("Reset"), this);
	m_resetAction->setToolTip(QStringLiteral("Reset"));
	toolButton = new QToolButton(this);
	toolButton->setStyleSheet("QToolButton::menu-indicator{image: none;}");
	toolButton->setIcon(QIcon(":icons/resources/icons/reset.png"));
	toolButton->addAction(m_resetAction);
	hLayout->addWidget(toolButton);
	hLayout->setAlignment(Qt::AlignLeft);
	group->setLayout(hLayout);
	mainLayout->addWidget(group);

	//Mark Group
	group = new QGroupBox(QStringLiteral("Mark"), this);
	vLayout = new QVBoxLayout;
	
		//Categroy
	m_categoryLabel = new QLabel(QStringLiteral("Category:"), this);
	m_categoryCBBox = new QComboBox(this);
	m_addCategoryAction = new QAction(QStringLiteral("Add"), this);
	m_addCategoryAction->setToolTip(QStringLiteral("Add Category"));
	toolButton = new QToolButton(this);
	toolButton->setStyleSheet("QToolButton::menu-indicator{image: none;}");
	toolButton->setIcon(QIcon(":icons/resources/icons/add.png"));
	toolButton->addAction(m_addCategoryAction);

	hLayout = new QHBoxLayout;
	hLayout->addWidget(m_categoryLabel);
	hLayout->addWidget(m_categoryCBBox);
	hLayout->addWidget(toolButton);
	vLayout->addLayout(hLayout);
		//pen size and color
	m_penSizeLabel = new QLabel(QStringLiteral("PenSize:"), this);
	m_penSizeCBBox = new QComboBox(this);
	m_colorAction = new QAction( QStringLiteral("Color"), this);
	m_colorAction->setToolTip(QStringLiteral("Select Color"));
	toolButton = new QToolButton(this);
	toolButton->setStyleSheet("QToolButton::menu-indicator{image: none;}");
	toolButton->setIcon(QIcon(":icons/resources/icons/color.png"));
	toolButton->addAction(m_colorAction);
	hLayout = new QHBoxLayout;
	hLayout->addWidget(m_penSizeLabel);
	hLayout->addWidget(m_penSizeCBBox);
	hLayout->addWidget(toolButton);
	vLayout->addLayout(hLayout);

		//Mark pen
	hLayout = new QHBoxLayout;

	m_markAction = new QAction( QStringLiteral("Mark"), this);
	m_markAction->setToolTip(QStringLiteral("Mark"));
	m_markAction->setCheckable(true);
	toolButton = new QToolButton(this);
	toolButton->setStyleSheet("QToolButton::menu-indicator{image: none;}");
	toolButton->setIcon(QIcon(":icons/resources/icons/mark.png"));
	toolButton->addAction(m_markAction);
	toolButton->setCheckable(true);
	hLayout->addWidget(toolButton);
		//selection
	m_markSelectionAction = new QAction( QStringLiteral("Select"), this);
	m_markSelectionAction->setToolTip(QStringLiteral("Select Mark"));
	m_markSelectionAction->setCheckable(true);
	toolButton = new QToolButton(this);
	toolButton->setStyleSheet("QToolButton::menu-indicator{image: none;}");
	toolButton->setIcon(QIcon(":icons/resources/icons/select.png"));
	toolButton->addAction(m_markSelectionAction);
	toolButton->setCheckable(true);
	hLayout->addWidget(toolButton);
		//deletion
	m_markDeletionAction = new QAction( QStringLiteral("Delete"), this);
	m_markDeletionAction->setToolTip(QStringLiteral("Delete Mark"));
	toolButton = new QToolButton(this);
	toolButton->setStyleSheet("QToolButton::menu-indicator{image: none;}");
	toolButton->setIcon(QIcon(":icons/resources/icons/delete.png"));
	toolButton->addAction(m_markDeletionAction);
	toolButton->setCheckable(true);
	hLayout->addWidget(toolButton);
	hLayout->setAlignment(Qt::AlignLeft);

	vLayout->addLayout(hLayout);
	group->setLayout(vLayout);
	mainLayout->addWidget(group);
	mainLayout->addStretch();
	

	setLayout(mainLayout);
}
