#include <QToolBar>
#include <QColorDialog>
#include <complex>
#include <QMenu>
#include <QToolButton>
#include <QPushButton>
#include <QCheckBox>
#include <QComboBox>
#include <QMessageBox>
#include <QLabel>

#include "imageviewer.h"
#include "globals.h"
#include "abstractslicedatamodel.h"
#include "titledsliderwithspinbox.h"
#include "histogram.h"
#include "pixelviewer.h"
#include "markcategorydialog.h"
#include "markmodel.h"
#include "markitem.h"
#include "sliceview.h"


inline bool ImageView::contains(const QWidget* widget, const QPoint& pos)
{
	return (widget->rect()).contains(pos);
}

void ImageView::createWidgets()
{

}

void ImageView::createToolBar()
{
	//createToolBar()
	m_topSlicePlayAction = new QAction(tr("Play"), this);
	m_topSlicePlayAction->setCheckable(true);
	m_rightSlicePlayAction = new QAction(tr("Play"), this);
	m_rightSlicePlayAction->setCheckable(true);
	m_frontSlicePlayAction = new QAction(tr("Play"), this);
	m_frontSlicePlayAction->setCheckable(true);

	m_zoomInAction = new QAction(QStringLiteral("ZoomIn"), this);
	m_zoomOutAction = new QAction(QStringLiteral("ZoomOut"), this);
	//tool bar
	m_viewToolBar = new QToolBar(QStringLiteral("View ToolBar"),this);
	m_layout->addWidget(m_viewToolBar, 0, 0, 1, 2);
	m_viewToolBar->addWidget(m_topSliceCheckBox);
	m_viewToolBar->addWidget(m_topSlider);
	m_viewToolBar->addAction(m_topSlicePlayAction);
	m_viewToolBar->addSeparator();
	m_viewToolBar->addWidget(m_rightSliceCheckBox);
	m_viewToolBar->addWidget(m_rightSlider);
	m_viewToolBar->addAction(m_rightSlicePlayAction);
	m_viewToolBar->addSeparator();
	m_viewToolBar->addWidget(m_frontSliceCheckBox);
	m_viewToolBar->addWidget(m_frontSlider);
	m_viewToolBar->addAction(m_frontSlicePlayAction);

	//m_viewToolBar->addAction(m_colorAction);
	//m_viewToolBar->addAction(m_markAction);
	m_viewToolBar->addSeparator();
	m_viewToolBar->addAction(m_zoomInAction);
	m_viewToolBar->addAction(m_zoomOutAction);
	//create menu on toolbar
	m_menu = new QMenu(this);
	m_menuButton = new QToolButton(this);
	m_menuButton->setText(QStringLiteral("Options"));
	m_menuButton->setPopupMode(QToolButton::MenuButtonPopup);
	m_menuButton->setMenu(m_menu);
	m_viewToolBar->addSeparator();
	m_viewToolBar->addWidget(m_menuButton);
	m_histDlg = m_menu->addAction(QStringLiteral("Histogram..."));

	//create edit tool bar
	m_editToolBar = new QToolBar(QStringLiteral("Edit ToolBar"),this);
	m_layout->addWidget(m_editToolBar,1,0,1,2);

	m_markAction = new QAction(QStringLiteral("Mark"), this);
	m_markAction->setCheckable(true);
	m_colorAction = new QAction(QStringLiteral("Color"), this);
	m_markSeletectionAction = new QAction(QStringLiteral("Select"),this);
	m_markMergeAction = new QAction(QStringLiteral("Merge"), this);
	m_markDeletionAction = new QAction(QStringLiteral("Delete"), this);
	m_renameAction = new QAction(QStringLiteral("Rename"), this);

	m_addCategoryAction = new QAction(QStringLiteral("Add..."), this);
	m_categoryLabel = new QLabel(QStringLiteral("Category:"), this);
	m_categoryCBBox = new QComboBox(this);
	m_penSizeLabel = new QLabel(QStringLiteral("PenSize:"), this);
	m_penSizeCCBox = new QComboBox(this);

	for(int i=1;i<=30;i++)
		m_penSizeCCBox->addItem(QString::number(i), QVariant::fromValue(i));
	m_editToolBar->addWidget(m_categoryLabel);
	m_editToolBar->addWidget(m_categoryCBBox);
	m_editToolBar->addAction(m_addCategoryAction);

	m_editToolBar->addSeparator();
	m_editToolBar->addWidget(m_penSizeLabel);
	m_editToolBar->addWidget(m_penSizeCCBox);
	m_editToolBar->addAction(m_colorAction);
	m_editToolBar->addAction(m_markAction);
	m_editToolBar->addSeparator();
	m_editToolBar->addAction(m_markSeletectionAction);
	m_editToolBar->addAction(m_markMergeAction);
	m_editToolBar->addAction(m_markDeletionAction);
	m_editToolBar->addSeparator();
	m_editToolBar->addAction(m_renameAction);

}

void ImageView::createConnections()
{
	//forward slider signals
	connect(m_topSlider, &TitledSliderWithSpinBox::valueChanged, this, &ImageView::topSliceChanged);
	connect(m_rightSlider, &TitledSliderWithSpinBox::valueChanged, this, &ImageView::rightSliceChanged);
	connect(m_frontSlider, &TitledSliderWithSpinBox::valueChanged, this, &ImageView::frontSliceChanged);
	//update slice
	connect(m_topSlider, &TitledSliderWithSpinBox::valueChanged, [=](int value) {Q_UNUSED(value); updateSlice(SliceType::Top); });
	connect(m_rightSlider, &TitledSliderWithSpinBox::valueChanged, [=](int value) {Q_UNUSED(value); updateSlice(SliceType::Right); });
	connect(m_frontSlider, &TitledSliderWithSpinBox::valueChanged, [=](int value) {Q_UNUSED(value); updateSlice(SliceType::Front); });
	//forward selected signals
	connect(m_topView, &SliceView::sliceSelected, this, &ImageView::topSliceSelected);
	connect(m_rightView, &SliceView::sliceSelected, this, &ImageView::rightSliceSelected);
	connect(m_frontView, &SliceView::sliceSelected, this, &ImageView::frontSliceSelected);

	connect(m_topView, &SliceView::markAdded, [this](QGraphicsItem* mark){markAddedHelper(SliceType::Top, mark);});
	connect(m_rightView, &SliceView::markAdded, [this](QGraphicsItem* mark){markAddedHelper(SliceType::Right, mark);});
	connect(m_frontView, &SliceView::markAdded, [this](QGraphicsItem* mark){markAddedHelper(SliceType::Front, mark);});

	connect(m_reset, &QPushButton::clicked, [this](bool click)
	{
		Q_UNUSED(click);
		m_topView->resetMatrix();
		m_rightView->resetMatrix();
		m_frontView->resetMatrix();
	});
	connect(m_markAction, &QAction::triggered, [this](bool enable)
	{
		m_topView->paintEnable(enable);
		m_rightView->paintEnable(enable);
		m_frontView->paintEnable(enable);
	});
	connect(m_topSlicePlayAction, &QAction::triggered, [this](bool enable) {onTopSlicePlay(enable); if (!enable)emit topSlicePlayStoped(m_topSlider->value()); });
	connect(m_rightSlicePlayAction, &QAction::triggered, [this](bool enable) {onRightSlicePlay(enable); if (!enable)emit rightSlicePlayStoped(m_rightSlider->value()); });
	connect(m_frontSlicePlayAction, &QAction::triggered, [this](bool enable) {onFrontSlicePlay(enable); if (!enable)emit frontSlicePlayStoped(m_frontSlider->value()); });
	connect(m_addCategoryAction, &QAction::triggered, [this]()
	{
		MarkCategoryDialog dlg(this);
		connect(&dlg, &MarkCategoryDialog::resultReceived, [this](const QString & name, const QColor & color)
		{
			addCategoryManagerHelper(name, color);
			setColor(color);
		});
		dlg.exec();
	});

	connect(m_colorAction, &QAction::triggered, [this]()
	{
		auto d = m_categoryCBBox->itemData(m_categoryCBBox->currentIndex());
		QColor c = d.canConvert<QColor>() ? d.value<QColor>() : Qt::black;
		setColor(QColorDialog::getColor(c, this, QStringLiteral("Color")));
	});
	connect(m_zoomInAction, &QAction::triggered, [this]() {double factor = std::pow(1.125, 1); m_topView->scale(factor, factor); });
	connect(m_zoomOutAction, &QAction::triggered, [this]() {double factor = std::pow(1.125, -1); m_topView->scale(factor, factor); });
	connect(m_histDlg, &QAction::triggered, []()
	{
		///TODO::open histogram dialog
	});
	connect(m_topSliceCheckBox, &QCheckBox::toggled, [this](bool toggle) {Q_UNUSED(toggle); updateTopSliceActions(); });
	connect(m_rightSliceCheckBox, &QCheckBox::toggled, [this](bool toggle) {Q_UNUSED(toggle); updateRightSliceActions(); });
	connect(m_frontSliceCheckBox, &QCheckBox::toggled, [this](bool toggle) {Q_UNUSED(toggle); updateFrontSliceActions(); });
}

void ImageView::updateActions()
{
	bool enable = m_sliceModel != nullptr;
	m_addCategoryAction->setEnabled(enable);
	m_categoryCBBox->setEnabled(enable);

	updateTopSliceActions();
	updateRightSliceActions();
	updateFrontSliceActions();
}

void ImageView::updateTopSliceActions()
{
	bool enable = m_topSliceCheckBox->isChecked() && m_sliceModel != nullptr;
	m_topSlicePlayAction->setEnabled(enable);
	m_topSlider->setEnabled(enable);
	m_topView->setHidden(!enable);
	m_reset->setHidden(enable == false && !m_frontSliceCheckBox->isChecked() && !m_rightSliceCheckBox->isChecked());
}

void ImageView::updateFrontSliceActions()
{
	bool enable = m_frontSliceCheckBox->isChecked() && m_sliceModel != nullptr;
	m_frontSlicePlayAction->setEnabled(enable);
	m_frontSlider->setEnabled(enable);
	m_frontView->setHidden(!enable);
	m_reset->setHidden(enable == false && !m_topSliceCheckBox->isChecked() && !m_rightSliceCheckBox->isChecked());
}

void ImageView::updateRightSliceActions()
{
	bool enable = m_rightSliceCheckBox->isChecked() && m_sliceModel != nullptr;
	m_rightSlicePlayAction->setEnabled(enable);
	m_rightSlider->setEnabled(enable);
	m_rightView->setHidden(!enable);
	m_reset->setHidden(enable == false && !m_frontSliceCheckBox->isChecked() && !m_topSliceCheckBox->isChecked());
}

void ImageView::createContextMenu()
{
	m_contextMenu = new QMenu(this);
	///TODO:: add icons here
	m_zoomIn = new QAction(QStringLiteral("Zoom In"), this);
	m_zoomOut = new QAction(QStringLiteral("Zoom Out"), this);

	m_histDlgAction = new QAction(QStringLiteral("Histgoram..."), this);
	m_pixelViewDlgAction = new QAction(QStringLiteral("Pixel View..."), this);
	m_marksManagerDlgAction = new QAction(QStringLiteral("Marks..."), this);

	connect(m_histDlgAction, &QAction::triggered, [this]()
	{
		AbstractPlugin * histViewDlg;
		if (m_menuWidget == m_topView)
		{
			histViewDlg = new HistogramViewer(SliceType::Top, QStringLiteral("Top Slice Histogram"), m_topView, m_sliceModel, this);
			histViewDlg->setWindowFlag(Qt::Window);
			histViewDlg->show();
			histViewDlg->setAttribute(Qt::WA_DeleteOnClose);
			//histViewDlg->sliceOpenEvent(m_topSlider->value());
			connect(this, &ImageView::topSliceOpened, histViewDlg, &AbstractPlugin::sliceOpened);
			connect(this, &ImageView::topSlicePlayStoped, histViewDlg, &AbstractPlugin::slicePlayStoped);
			connect(this, &ImageView::topSliceChanged, histViewDlg, &AbstractPlugin::sliceChanged);

			emit topSliceOpened(m_topSlider->value());
		}
		else if (m_menuWidget == m_rightView)
		{
			histViewDlg = new HistogramViewer(SliceType::Right, QStringLiteral("Right Slice Histogram"), m_rightView, m_sliceModel, this);
			histViewDlg->setWindowFlag(Qt::Window);
			histViewDlg->show();
			histViewDlg->setAttribute(Qt::WA_DeleteOnClose);
			//histViewDlg->sliceOpenEvent(m_rightSlider->value());
			connect(this, &ImageView::rightSliceOpened, histViewDlg, &AbstractPlugin::sliceOpened);
			connect(this, &ImageView::rightSlicePlayStoped, histViewDlg, &AbstractPlugin::slicePlayStoped);
			connect(this, &ImageView::rightSliceChanged, histViewDlg, &AbstractPlugin::sliceChanged);
			emit rightSliceOpened(m_rightSlider->value());
		}
		else if (m_menuWidget == m_frontView)
		{
			histViewDlg = new HistogramViewer(SliceType::Front, QStringLiteral("Front Slice Histogram"), m_frontView, m_sliceModel, this);
			histViewDlg->setWindowFlag(Qt::Window);
			histViewDlg->show();
			histViewDlg->setAttribute(Qt::WA_DeleteOnClose);
			//histViewDlg->sliceOpenEvent(m_frontSlider->value());
			connect(this, &ImageView::frontSliceOpened, histViewDlg, &AbstractPlugin::sliceOpened);
			connect(this, &ImageView::frontSlicePlayStoped, histViewDlg, &AbstractPlugin::slicePlayStoped);
			connect(this, &ImageView::frontSliceChanged, histViewDlg, &AbstractPlugin::sliceChanged);
			emit frontSliceOpened(m_frontSlider->value());
		}
		//connect(this, &ImageView::sliceChanged, histViewDlg, &AbstractPlugin::sliceChanged);
		connect(this, &ImageView::topSlicePlayStoped, histViewDlg, &AbstractPlugin::slicePlayStoped);
	});
	connect(m_pixelViewDlgAction, &QAction::triggered, [this]()
	{
		PixelViewer * pixelViewDlg;
		if (m_menuWidget == m_topView)
		{
			pixelViewDlg = new PixelViewer(SliceType::Top, QStringLiteral("Top Slice Pixel View"), m_topView, m_sliceModel, this);
			pixelViewDlg->setWindowFlag(Qt::Window);
			pixelViewDlg->show();
			pixelViewDlg->setAttribute(Qt::WA_DeleteOnClose);
			//			pixelViewDlg->setImage(m_sliceModel->topSlice(m_topSlider->value()));
			connect(m_topView, &SliceView::sliceSelected, pixelViewDlg, &AbstractPlugin::sliceSelected);
			connect(this, &ImageView::topSliceOpened, pixelViewDlg, &AbstractPlugin::sliceOpened);
			emit topSliceOpened(m_topSlider->value());

		}
		else if (m_menuWidget == m_rightView)
		{
			pixelViewDlg = new PixelViewer(SliceType::Right, QStringLiteral("Right Slice Pixel View"), m_rightView, m_sliceModel, this);
			pixelViewDlg->setWindowFlag(Qt::Window);
			pixelViewDlg->show();
			pixelViewDlg->setAttribute(Qt::WA_DeleteOnClose);
			//	pixelViewDlg->setImage(m_sliceModel->rightSlice(m_rightSlider->value()));
			connect(m_rightView, &SliceView::sliceSelected, pixelViewDlg, &AbstractPlugin::sliceSelected);
			connect(this, &ImageView::rightSliceOpened, pixelViewDlg, &AbstractPlugin::sliceOpened);
			emit rightSliceOpened(m_rightSlider->value());
		}
		else if (m_menuWidget == m_frontView)
		{
			pixelViewDlg = new PixelViewer(SliceType::Front, QStringLiteral("Front Slice Pixel View"), m_frontView, m_sliceModel, this);
			pixelViewDlg->setWindowFlag(Qt::Window);
			pixelViewDlg->show();
			pixelViewDlg->setAttribute(Qt::WA_DeleteOnClose);
			//	pixelViewDlg->setImage(m_sliceModel->frontSlice(m_frontSlider->value()));
			connect(m_frontView, &SliceView::sliceSelected, pixelViewDlg, &AbstractPlugin::sliceSelected);
			connect(this, &ImageView::frontSliceOpened, pixelViewDlg, &AbstractPlugin::sliceOpened);
			emit frontSliceOpened(m_frontSlider->value());
		}
	});
	connect(m_marksManagerDlgAction, &QAction::triggered, [this]()
	{

	});

	m_contextMenu->addAction(m_zoomIn);
	m_contextMenu->addAction(m_zoomOut);
	m_contextMenu->addSeparator();
	m_contextMenu->addAction(m_histDlgAction);
	m_contextMenu->addAction(m_pixelViewDlgAction);
	m_contextMenu->addAction(m_marksManagerDlgAction);
}

ImageView::ImageView(QWidget *parent, bool topSliceVisible, bool rightSliceVisible, bool frontSliceVisible, AbstractSliceDataModel * model) :
	QWidget(parent),
	m_markModel(nullptr),
	m_sliceModel(model)
{
	//layout
	m_layout = new QGridLayout;
	//QGraphicsView
	m_topView = new SliceView;
	m_rightView = new SliceView;
	m_frontView = new SliceView;
	m_topView->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
	m_rightView->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
	m_frontView->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

	m_reset = new QPushButton(QStringLiteral("Reset"));
	m_reset->setSizePolicy(QSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored));
	//m_view->resize(500, 500);
	//m_rightView->resize(50, 500);
	//m_frontView->resize(500, 50);
	m_layout->setSizeConstraint(QLayout::SetFixedSize);
	//m_layout->setColumnStretch(0, 5);
	//m_layout->setColumnStretch(1,1);
	m_layout->addWidget(m_topView, 2, 0, 1, 1, Qt::AlignCenter);
	m_layout->addWidget(m_rightView, 2, 1, 1, 1, Qt::AlignCenter);
	m_layout->addWidget(m_frontView, 3, 0, 1, 1, Qt::AlignCenter);
	m_layout->addWidget(m_reset, 3, 1, 1, 1, Qt::AlignCenter);
	//m_view->setScene(m_scene);

	//sliders

	m_topSliceCheckBox = new QCheckBox;
	m_topSliceCheckBox->setChecked(topSliceVisible);
	m_topSlider = new TitledSliderWithSpinBox(this, tr("Z:"));
	m_rightSliceCheckBox = new QCheckBox;
	m_rightSliceCheckBox->setChecked(rightSliceVisible);
	m_rightSlider = new TitledSliderWithSpinBox(this, tr("Y:"));
	m_frontSliceCheckBox = new QCheckBox;
	m_frontSliceCheckBox->setChecked(frontSliceVisible);
	m_frontSlider = new TitledSliderWithSpinBox(this, tr("X:"));

	createToolBar();


	createContextMenu();
	createConnections();
	updateActions();
	setLayout(m_layout);
}

void ImageView::changeSlice(int value, SliceType type)
{
	Q_ASSERT_X(m_sliceModel != nullptr, "ImageView::sliceChanged", "null model pointer");
	SliceView * view = nullptr;
	std::function<QImage(int)> sliceGetter;
	//TODO:: 
	switch (type)
	{
	case SliceType::Top:
		view = m_topView;
		sliceGetter = std::bind(&AbstractSliceDataModel::frontSlice, m_sliceModel, std::placeholders::_1);
		break;
	case SliceType::Right:
		view = m_rightView;
		sliceGetter = std::bind(&AbstractSliceDataModel::rightSlice, m_sliceModel, std::placeholders::_1);
		break;
	case SliceType::Front:
		view = m_frontView;
		sliceGetter = std::bind(&AbstractSliceDataModel::frontSlice, m_sliceModel, std::placeholders::_1);
		break;
	default:
		Q_ASSERT_X(false, "ImageView::updateSlice", "SliceType error.");
	}
	Q_ASSERT_X(static_cast<bool>(sliceGetter) == true, "ImageView::updateSlice", "invalid bind");
	view->setImage(sliceGetter(value));
	view->clearSliceMarks();
}

inline void ImageView::setTopSliceCount(int value) { m_topSlider->setMaximum(value - 1); }
inline void ImageView::setRightSliceCount(int value) { m_rightSlider->setMaximum(value - 1); }
inline void ImageView::setFrontSliceCount(int value) { m_frontSlider->setMaximum(value - 1); }
int ImageView::currentIndex(SliceType type)
{
	switch (type)
	{
	case SliceType::Top:
		return topSliceIndex();
	case SliceType::Right:
		return rightSliceIndex();
	case SliceType::Front:
		return frontSliceIndex();
	default:
		return -1;
	}
}

MarkModel* ImageView::createMarkModel(ImageView *view, AbstractSliceDataModel * d)
{
	return new MarkModel( d, view,
		new TreeItem(QVector<QVariant>{QStringLiteral("Name"), QStringLiteral("Desc")}, TreeItemType::Root),
		nullptr);
}

void ImageView::markAddedHelper(SliceType type, QGraphicsItem * mark)
{
	QString cate = m_categoryCBBox->currentText();
	QVariant categoryColor = m_categoryCBBox->currentData();
	if(cate.isEmpty())
	{
		cate = QStringLiteral("Category#%1").arg(m_categoryCBBox->count());
		categoryColor = QVariant::fromValue(Qt::black);
		addCategoryManagerHelper(cate, categoryColor.value<QColor>());
	}
	//auto m = QueryMarkItemInterface<AbstractMarkItem*,PolyMarkItem*>(mark);
	mark->setData(MarkProperty::SliceType, QVariant::fromValue(static_cast<int>(type)));
	mark->setData(MarkProperty::CategoryName, QVariant::fromValue(cate));
	mark->setData(MarkProperty::CategoryColor, categoryColor);
	mark->setData(MarkProperty::VisibleState, QVariant::fromValue(true));
	//slicetype, sliceindex, categoryname, name, color, categorycolor, visible state
	int index;
	QColor color;
	switch(type)
	{
	case SliceType::Top:
		index = m_topSlider->value();
		color = m_topView->color();
		break;
	case SliceType::Right:
		index = m_rightSlider->value();
		color = m_rightView->color();
		break;
	case SliceType::Front:
		index = m_frontSlider->value();
		color = m_frontView->color();
		break;
	}
	mark->setData(MarkProperty::SliceIndex, QVariant::fromValue(index));
	mark->setData(MarkProperty::Color, color);

	Q_ASSERT_X(m_markModel != nullptr,
		"mark_create_helper_", "null pointer");
	m_markModel->addMark(cate,mark);
}

void ImageView::setCategoryManagerHelper(const QVector<QPair<QString, QColor>>& cates)
{
	Q_ASSERT_X(m_categoryCBBox, "ImageVIew::initCCBoxHelper", "null pointer");
	m_categoryCBBox->clear();
	foreach(const auto & p,cates)
		m_categoryCBBox->addItem(p.first, QVariant::fromValue(p.second));
}

void ImageView::addCategoryManagerHelper(const QString & name, const QColor & color)
{
	m_categoryCBBox->addItem(name,color);
	m_categoryCBBox->setCurrentText(name);
}

void ImageView::markModelUpdatedHelper(MarkModel * model)
{

}

int ImageView::topSliceIndex() const { return m_topSlider->value(); }

int ImageView::rightSliceIndex() const { return m_rightSlider->value(); }

int ImageView::frontSliceIndex() const { return m_frontSlider->value(); }

inline void ImageView::topSliceEnable(bool enable)
{
	m_topSliceCheckBox->setChecked(enable);
	updateActions();
}

inline void ImageView::rightSliceEnable(bool enable)
{
	m_rightSliceCheckBox->setChecked(enable);
	updateActions();
}

inline void ImageView::frontSliceEnable(bool enable)
{
	m_frontSliceCheckBox->setChecked(enable);
	updateActions();
}

void ImageView::setColor(const QColor & color)
{
	m_topView->setColor(color);
	m_rightView->setColor(color);
	m_frontView->setColor(color);
}

void ImageView::setSliceModel(AbstractSliceDataModel * model)
{
	m_sliceModel = model;
	updateSliceCount(SliceType::Top);
	updateSliceCount(SliceType::Right);
	updateSliceCount(SliceType::Front);

	updateSlice(SliceType::Front);
	updateSlice(SliceType::Right);
	updateSlice(SliceType::Top);

	//TODO::update marks
	if (m_markModel != nullptr)
	{
		if (m_markModel->checkMatchHelper(model) == false)		//If the mark model doesnt match the slice model
		{
			m_markModel->deleteLater();				//Error
			m_markModel = nullptr;
		}
	}
	else
	{
		m_markModel = createMarkModel(this, m_sliceModel);
	}//create a new mark model
	updateMarks(SliceType::Top);
	updateMarks(SliceType::Right);
	updateMarks(SliceType::Front);
	updateActions();
}



MarkModel* ImageView::replaceMarkModel(MarkModel* model, bool * success)noexcept
{
	//check the model
	if (m_sliceModel == nullptr)
	{
		QMessageBox::critical(this, QStringLiteral("Error"),
			QStringLiteral("Mark model can't be applied without slice data."),
			QMessageBox::StandardButton::Ok, QMessageBox::StandardButton::Ok);
		if (success != nullptr)
			*success = false;
		return nullptr;
	}
	if (model == nullptr)			//remove mark
	{
		auto t= m_markModel;
		t->m_view = nullptr;
		t->m_dataModel = nullptr;

		m_markModel = nullptr;
		updateMarks(SliceType::Top);
		updateMarks(SliceType::Right);
		updateMarks(SliceType::Front);
		setCategoryManagerHelper(QVector<QPair<QString, QColor>>());

		if (success != nullptr)
			*success = true;
		return t;
	}
	if (model->checkMatchHelper(m_sliceModel) == false)
	{
		if (success != nullptr)
			*success = false;
		return nullptr;
	}
	auto t = m_markModel;
	t->m_view = nullptr;
	t->m_dataModel = nullptr;
	m_markModel = model;
	m_markModel->m_view = this;
	m_markModel->m_dataModel = m_sliceModel;
	updateMarks(SliceType::Top);
	updateMarks(SliceType::Right);
	updateMarks(SliceType::Front);
	auto cateItems = m_markModel->categoryItems();
	QVector<QPair<QString, QColor>> cates;

	foreach(const auto & item,cateItems)
		cates << qMakePair(item->name(),item->color());
	setCategoryManagerHelper(cates);

	if (success != nullptr)
		*success = true;
	return t;
}
MarkModel * ImageView::markModel()
{
	return m_markModel;
}
void ImageView::setEnabled(bool enable)
{
	m_topSliceCheckBox->setEnabled(enable);
	m_rightSliceCheckBox->setEnabled(enable);
	m_frontSliceCheckBox->setEnabled(enable);

	m_colorAction->setEnabled(enable);
	m_markAction->setEnabled(enable);
	m_zoomInAction->setEnabled(enable);
	m_zoomOutAction->setEnabled(enable);
	m_menuButton->setEnabled(enable);

	m_topSlicePlayAction->setEnabled(enable);
	m_topSlider->setEnabled(enable);
	m_topView->setEnabled(enable);
	m_topSliceCheckBox->setEnabled(enable);

	m_rightView->setEnabled(enable);
	m_rightSlicePlayAction->setEnabled(enable);
	m_rightSlider->setEnabled(enable);
	m_topSliceCheckBox->setEnabled(enable);

	m_frontSlider->setEnabled(enable);
	m_frontView->setEnabled(enable);
	m_frontSlicePlayAction->setEnabled(enable);
	m_frontSliceCheckBox->setEnabled(enable);
}


void ImageView::onTopSlicePlay(bool enable)
{
	if (enable)
	{
		m_topTimerId = startTimer(10);
		//qDebug() << "onTopSliceTimer" << m_topTimerId;
		m_topSlicePlayDirection = PlayDirection::Forward;
	}
	else
	{
		killTimer(m_topTimerId);
		m_topTimerId = 0;
	}
}

void ImageView::onRightSlicePlay(bool enable)
{
	if (enable)
	{
		m_rightTimerId = startTimer(10);
		//qDebug() << "onRightSliceTimer" << m_rightTimerId;
		m_rightSlicePlayDirection = PlayDirection::Forward;
	}
	else
	{
		killTimer(m_rightTimerId);
		m_rightTimerId = 0;
	}
}

void ImageView::onFrontSlicePlay(bool enable)
{
	if (enable)
	{
		m_frontTimerId = startTimer(10);
		//qDebug() << "onFrontSliceTimer" << m_frontTimerId;
		m_frontSlicePlayDirection = PlayDirection::Forward;
	}
	else
	{
		killTimer(m_frontTimerId);
		m_frontTimerId = 0;
	}
}

void ImageView::timerEvent(QTimerEvent* event)
{
	int timeId = event->timerId();
	if (timeId == m_topTimerId)
	{
		int maxSlice = m_topSlider->maximum();
		int cur = m_topSlider->value();
		if (m_topSlicePlayDirection == PlayDirection::Forward)
		{
			cur++;
		}
		else
		{
			cur--;
		}
		if (cur >= maxSlice)
		{
			m_topSlicePlayDirection = PlayDirection::Backward;
		}
		else if (cur <= 0)
		{
			m_topSlicePlayDirection = PlayDirection::Forward;
		}
		m_topSlider->setValue(cur);
	}
	else if (timeId == m_rightTimerId)
	{
		int maxSlice = m_rightSlider->maximum();
		int cur = m_rightSlider->value();
		if (m_rightSlicePlayDirection == PlayDirection::Forward)
		{
			cur++;
		}
		else
		{
			cur--;
		}
		if (cur >= maxSlice)
		{
			m_rightSlicePlayDirection = PlayDirection::Backward;
		}
		else if (cur <= 0)
		{
			m_rightSlicePlayDirection = PlayDirection::Forward;
		}
		m_rightSlider->setValue(cur);

	}
	else if (timeId == m_frontTimerId)
	{
		int maxSlice = m_frontSlider->maximum();
		int cur = m_frontSlider->value();
		if (m_frontSlicePlayDirection == PlayDirection::Forward)
		{
			cur++;
		}
		else
		{
			cur--;
		}
		if (cur >= maxSlice)
		{
			m_frontSlicePlayDirection = PlayDirection::Backward;
		}
		else if (cur <= 0)
		{
			m_frontSlicePlayDirection = PlayDirection::Forward;
		}
		m_frontSlider->setValue(cur);
	}
}

void ImageView::contextMenuEvent(QContextMenuEvent* event)
{
	const QPoint pos = event->pos();
	if (true == contains(m_topView, m_topView->mapFrom(this, pos)))
	{
		m_menuWidget = m_topView;
		m_contextMenu->exec(this->mapToGlobal(pos));
	}
	else if (true == contains(m_rightView, m_rightView->mapFrom(this, pos)))
	{
		m_menuWidget = m_rightView;
		m_contextMenu->exec(this->mapToGlobal(pos));
	}
	else if (true == contains(m_frontView, m_frontView->mapFrom(this, pos)))
	{
		m_menuWidget = m_frontView;
		m_contextMenu->exec(this->mapToGlobal(pos));
	}
	event->accept();
}
void ImageView::updateSliceCount(SliceType type)
{
	switch (type)
	{
	case SliceType::Top:
		setTopSliceCount(m_sliceModel->topSliceCount() - 1);
		break;
	case SliceType::Right:
		setRightSliceCount(m_sliceModel->rightSliceCount() - 1);
		break;
	case SliceType::Front:
		setFrontSliceCount(m_sliceModel->frontSliceCount() - 1);
		break;
	default:
		break;
	}
}

void ImageView::updateSlice(SliceType type)
{
	//
	SliceView * view = nullptr;
	std::function<QImage(int)> sliceGetter;
	const MarkModel::MarkSliceList * list = nullptr;
	switch (type)
	{
	case SliceType::Top:
		view = m_topView;
		sliceGetter = std::bind(&AbstractSliceDataModel::topSlice, 
			m_sliceModel, std::placeholders::_1);
		list = &m_markModel->topSliceVisibleMarks();
		break;
	case SliceType::Right:
		view = m_rightView;
		sliceGetter = std::bind(&AbstractSliceDataModel::rightSlice,
			m_sliceModel, std::placeholders::_1);
		list = &m_markModel->rightSliceVisibleMarks();
		break;
	case SliceType::Front:
		view = m_frontView;
		sliceGetter = std::bind(&AbstractSliceDataModel::frontSlice,
			m_sliceModel, std::placeholders::_1);
		list = &m_markModel->frontSliceVisibleMarks();
		break;
	default:
		Q_ASSERT_X(false,
			"ImageView::updateSlice", "SliceType error.");
	}
	Q_ASSERT_X(static_cast<bool>(sliceGetter) == true,
		"ImageView::updateSlice", "null function");
	int index = currentIndex(type);
	view->setImage(sliceGetter(index));
	view->clearSliceMarks();
	if (m_markModel == nullptr)
		return;
	//Q_ASSERT_X(m_markModel, "ImageView::updateSlice", "null pointer");
	view->setMarks((*list)[index]);
}

void ImageView::updateMarks(SliceType type)
{
	Q_ASSERT_X(m_markModel,
		"ImageView::updateMarks", "null pointer");
	switch (type)
	{
	case SliceType::Top:
	{
		//QList<QGraphicsItem*> res;
		auto m = m_markModel->topSliceVisibleMarks()[topSliceIndex()];
		//for (int i = 0; i < m.size(); i++)
			//res.append(QueryMarkItemInterface<QGraphicsItem*,PolyMarkItem*>(m[i]));
		m_topView->setMarks(m);
	}
	break;
	case SliceType::Right:
	{
		//QList<QGraphicsItem*> res;
		auto m = m_markModel->rightSliceVisibleMarks()[rightSliceIndex()];
		//for (int i = 0; i < m.size(); i++)
			//res.append(QueryMarkItemInterface<QGraphicsItem*, PolyMarkItem*>(m[i]));
		m_rightView->setMarks(m);
	}
	break;
	case SliceType::Front:
	{
		//QList<QGraphicsItem*> res;
		auto m = m_markModel->frontSliceVisibleMarks()[frontSliceIndex()];
		//for (int i = 0; i < m.size(); i++)
		//	res.append(QueryMarkItemInterface<QGraphicsItem*, PolyMarkItem*>(m[i]));
		m_frontView->setMarks(m);
	}
	break;
	}
}



SliceScene::SliceScene(QObject *parent) :QGraphicsScene(parent)
{

}

void SliceScene::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
	//qDebug() << "mousePressEvent in scene";
	QGraphicsScene::mousePressEvent(event);
}

void SliceScene::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
	//qDebug() << "mouseMoveEvent in scene";
	QGraphicsScene::mouseMoveEvent(event);
}

void SliceScene::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
	//qDebug() << "mouseReleaseEvent in scene";
	QGraphicsScene::mouseReleaseEvent(event);
}

void SliceScene::wheelEvent(QGraphicsSceneWheelEvent * event)
{
	//qDebug() << "wheelEvent in scene";
	QGraphicsScene::wheelEvent(event);
}
