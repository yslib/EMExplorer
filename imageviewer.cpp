#include <QToolBar>
#include <QColorDialog>
#include <complex>
#include <QMenu>
#include <QToolButton>
#include <QPushButton>
#include <QCheckBox>
#include <QComboBox>
#include <QMessageBox>
#include <QDebug>
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
#include "categoryitem.h"
#include "volumewidget.h"



inline bool ImageCanvas::contains(const QWidget* widget, const QPoint& pos)
{
	return (widget->rect()).contains(pos);
}

void ImageCanvas::createWidgets()
{
}

void ImageCanvas::createToolBar()
{
	//createToolBar()
	//qRegisterMetaType("asdfsadf");
	m_topSlicePlayAction = new QAction(QIcon(":icons/resources/icons/play.png"),QStringLiteral("Play"), this);
	m_topSlicePlayAction->setToolTip(QStringLiteral("Play"));
	m_topSlicePlayAction->setCheckable(true);

	m_rightSlicePlayAction = new QAction(QIcon(":icons/resources/icons/play.png"), QStringLiteral("Play"), this);
	m_rightSlicePlayAction->setToolTip(QStringLiteral("Play"));
	m_rightSlicePlayAction->setCheckable(true);

	m_frontSlicePlayAction = new QAction(QIcon(":icons/resources/icons/play.png"), QStringLiteral("Play"), this);
	m_frontSlicePlayAction->setToolTip(QStringLiteral("Play"));
	m_frontSlicePlayAction->setCheckable(true);

	m_zoomInAction = new QAction(QIcon(":icons/resources/icons/zoom_in.png"),QStringLiteral("Zoom In"), this);
	m_zoomInAction->setToolTip(QStringLiteral("Zoom In"));
	m_zoomOutAction = new QAction(QIcon(":icons/resources/icons/zoom_out.png"),QStringLiteral("Zoom Out"), this);
	m_zoomOutAction->setToolTip(QStringLiteral("Zoom Out"));

	m_volumeWidgetAction = new QAction(QStringLiteral("Volume Render"), this);
	m_volumeWidgetAction->setToolTip(QStringLiteral("Volume Rendering"));
	m_volumeWidgetAction->setCheckable(true);
	//connect(m_volumeWidgetAction, &QAction::triggered, [this](bool check) {m_renderView->setVisible(check);});

	//tool bar
	m_viewToolBar = new QToolBar(QStringLiteral("View ToolBar"), this);
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
	m_viewToolBar->addAction(m_volumeWidgetAction);

	//m_viewToolBar->addAction(m_colorAction);
	//m_viewToolBar->addAction(m_markAction);
	m_viewToolBar->addSeparator();
	m_viewToolBar->addAction(m_zoomInAction);
	m_viewToolBar->addAction(m_zoomOutAction);
	m_viewToolBar->addAction(m_resetAction);
	
	//create menu on toolbar
	m_menu = new QMenu(this);
	m_menuButton = new QToolButton(this);
	m_menuButton->setIcon(QIcon(":icons/resources/icons/option.png"));
	m_menuButton->setText(QStringLiteral("Options"));
	m_menuButton->setPopupMode(QToolButton::MenuButtonPopup);
	m_menuButton->setMenu(m_menu);
	m_viewToolBar->addSeparator();
	m_viewToolBar->addWidget(m_menuButton);
	m_histDlg = m_menu->addAction(QStringLiteral("Histogram..."));

	//create edit tool bar
	m_editToolBar = new QToolBar(QStringLiteral("Edit ToolBar"), this);

	m_markAction = new QAction(QIcon(":icons/resources/icons/mark.png"),QStringLiteral("Mark"), this);
	m_markAction->setToolTip(QStringLiteral("Mark"));
	m_markAction->setCheckable(true);
	m_colorAction = new QAction(QIcon(":icons/resources/icons/color.png"),QStringLiteral("Color"), this);
	m_colorAction->setToolTip(QStringLiteral("Select Color"));
	m_markSelectionAction = new QAction(QIcon(":icons/resources/icons/select.png"),QStringLiteral("Select"), this);
	m_markSelectionAction->setToolTip(QStringLiteral("Select Mark"));
	m_markSelectionAction->setCheckable(true);
	//m_moveAction = new QAction(QStringLiteral("Move"), this);
	//m_moveAction->setCheckable(true);
	//m_markMergeAction = new QAction(QStringLiteral("Merge"), this);
	m_markDeletionAction = new QAction(QIcon(":icons/resources/icons/delete.png"),QStringLiteral("Delete"), this);
	m_markDeletionAction->setToolTip(QStringLiteral("Delete Mark"));
	m_addCategoryAction = new QAction(QIcon(":icons/resources/icons/add.png"),QStringLiteral("Add"), this);
	m_addCategoryAction->setToolTip(QStringLiteral("Add Category"));

	m_categoryLabel = new QLabel(QStringLiteral("Category:"), this);
	m_categoryCBBox = new QComboBox(this);
	m_penSizeLabel = new QLabel(QStringLiteral("PenSize:"), this);
	m_penSizeCBBox = new QComboBox(this);
	for (int i = 1; i <= 30; i++)
		m_penSizeCBBox->addItem(QString::number(i), QVariant::fromValue(i));
	m_editToolBar->addWidget(m_categoryLabel);
	m_editToolBar->addWidget(m_categoryCBBox);
	m_editToolBar->addAction(m_addCategoryAction);

	m_editToolBar->addSeparator();
	m_editToolBar->addWidget(m_penSizeLabel);
	m_editToolBar->addWidget(m_penSizeCBBox);
	m_editToolBar->addAction(m_colorAction);
	m_editToolBar->addSeparator();
	//m_editToolBar->addAction(m_moveAction);
	m_editToolBar->addAction(m_markAction);
	m_editToolBar->addAction(m_markSelectionAction);
	m_editToolBar->addSeparator();
	//m_editToolBar->addAction(m_markMergeAction);
	m_editToolBar->addAction(m_markDeletionAction);
}

void ImageCanvas::createConnections()
{
	//forward slider signals
	connect(m_topSlider, &TitledSliderWithSpinBox::valueChanged, this, &ImageCanvas::topSliceChanged);
	connect(m_rightSlider, &TitledSliderWithSpinBox::valueChanged, this, &ImageCanvas::rightSliceChanged);
	connect(m_frontSlider, &TitledSliderWithSpinBox::valueChanged, this, &ImageCanvas::frontSliceChanged);
	//update slice
	connect(m_topSlider, &TitledSliderWithSpinBox::valueChanged, [=](int value) { updateSlice(SliceType::Top,value); });
	connect(m_rightSlider, &TitledSliderWithSpinBox::valueChanged, [=](int value) {updateSlice(SliceType::Right,value); });
	connect(m_frontSlider, &TitledSliderWithSpinBox::valueChanged, [=](int value) {updateSlice(SliceType::Front,value); });
	//forward selected signals
	connect(m_topView, &SliceView::sliceSelected, this, &ImageCanvas::topSliceSelected);
	connect(m_rightView, &SliceView::sliceSelected, this, &ImageCanvas::rightSliceSelected);
	connect(m_frontView, &SliceView::sliceSelected, this, &ImageCanvas::frontSliceSelected);

	connect(m_topView, &SliceView::markAdded, [this](QGraphicsItem* mark) {markAddedHelper(SliceType::Top, mark);});
	connect(m_rightView, &SliceView::markAdded, [this](QGraphicsItem* mark) {markAddedHelper(SliceType::Right, mark);});
	connect(m_frontView, &SliceView::markAdded, [this](QGraphicsItem* mark) {markAddedHelper(SliceType::Front, mark);});

	connect(m_topSliceCheckBox, &QCheckBox::toggled, [this](bool toggle) {Q_UNUSED(toggle); updateTopSliceActions(); });
	connect(m_rightSliceCheckBox, &QCheckBox::toggled, [this](bool toggle) {Q_UNUSED(toggle); updateRightSliceActions(); });
	connect(m_frontSliceCheckBox, &QCheckBox::toggled, [this](bool toggle) {Q_UNUSED(toggle); updateFrontSliceActions(); });



	connect(m_resetAction, &QAction::triggered, this, &ImageCanvas::resetZoom);
	//view toolbar actions
	connect(m_topSlicePlayAction, &QAction::triggered, [this](bool enable) {onTopSlicePlay(enable); if (!enable)emit topSlicePlayStoped(m_topSlider->value()); });
	connect(m_rightSlicePlayAction, &QAction::triggered, [this](bool enable) {onRightSlicePlay(enable); if (!enable)emit rightSlicePlayStoped(m_rightSlider->value()); });
	connect(m_frontSlicePlayAction, &QAction::triggered, [this](bool enable) {onFrontSlicePlay(enable); if (!enable)emit frontSlicePlayStoped(m_frontSlider->value()); });
	connect(m_addCategoryAction, &QAction::triggered, this,&ImageCanvas::categoryAdded);

	connect(m_zoomInAction, &QAction::triggered, this, &ImageCanvas::zoomIn);
	connect(m_zoomOutAction, &QAction::triggered, this, &ImageCanvas::zoomOut);
	connect(m_topView, &SliceView::viewMoved, [this](const QPointF & delta) {m_rightView->translate(0.0f, delta.y()); m_frontView->translate(delta.x(), 0.0f); });
	connect(m_topView, &SliceView::selectionChanged, this, &ImageCanvas::updateDeleteAction);
	connect(m_rightView, &SliceView::selectionChanged, this, &ImageCanvas::updateDeleteAction);
	connect(m_frontView, &SliceView::selectionChanged, this, &ImageCanvas::updateDeleteAction);

	connect(m_topView, &SliceView::selectionChanged, this, &ImageCanvas::markSingleSelectionHelper);
	connect(m_rightView, &SliceView::selectionChanged, this, &ImageCanvas::markSingleSelectionHelper);
	connect(m_frontView, &SliceView::selectionChanged, this, &ImageCanvas::markSingleSelectionHelper);

	connect(m_penSizeCBBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int) {QPen pen = m_topView->pen(); pen.setWidth(m_penSizeCBBox->currentData().toInt()); updatePen(pen); });
	connect(m_colorAction, &QAction::triggered, this,&ImageCanvas::colorChanged);


	connect(m_markAction, &QAction::triggered, [this](bool enable)
	{
		if (enable == true)
		{
			m_topView->setOperation(SliceView::Paint);
			m_rightView->setOperation(SliceView::Paint);
			m_frontView->setOperation(SliceView::Paint);
			//m_moveAction->setChecked(false);
			m_markSelectionAction->setChecked(false);
		}
		else
		{
			m_topView->setOperation(SliceView::Move);
			m_rightView->setOperation(SliceView::Move);
			m_frontView->setOperation(SliceView::Move);
		}
	});
	connect(m_markSelectionAction, &QAction::triggered, [this](bool enable)
	{
		if (enable == true)
		{
			m_topView->setOperation(SliceView::Selection);
			m_rightView->setOperation(SliceView::Selection);
			m_frontView->setOperation(SliceView::Selection);
			m_markAction->setChecked(false);
			//m_moveAction->setChecked(false);
		}
		else
		{
			m_topView->setOperation(SliceView::Move);
			m_rightView->setOperation(SliceView::Move);
			m_frontView->setOperation(SliceView::Move);
		}

	});
	connect(m_markDeletionAction, &QAction::triggered, [this](bool enable) {Q_UNUSED(enable); markDeleteHelper(); updateDeleteAction(); });
	//connect(m_markMergeAction, &QAction::triggered, [this](bool enable) {});

}

void ImageCanvas::updateActions()
{
	bool enable = m_sliceModel != nullptr;
	m_addCategoryAction->setEnabled(enable);
	m_categoryLabel->setEnabled(enable);
	m_categoryCBBox->setEnabled(enable);
	m_penSizeLabel->setEnabled(enable);
	m_penSizeCBBox->setEnabled(enable);
	m_zoomInAction->setEnabled(enable);
	m_zoomOutAction->setEnabled(enable);
	m_colorAction->setEnabled(enable);
	m_markAction->setEnabled(enable);
	m_markSelectionAction->setEnabled(enable);
	//m_markMergeAction->setEnabled(enable);
	m_resetAction->setEnabled(enable);
	updateDeleteAction();
	updateTopSliceActions();
	updateRightSliceActions();
	updateFrontSliceActions();
}

void ImageCanvas::updateDeleteAction()
{
	Q_ASSERT_X(m_topView || m_rightView || m_frontView,
		"ImageView::updateDeleteAction", "null pointer");
	const bool enable = m_markModel
		&& (m_topView->selectedItemCount()
			|| m_rightView->selectedItemCount()
			|| m_frontView->selectedItemCount());
	m_markDeletionAction->setEnabled(enable);

}

void ImageCanvas::updateTopSliceActions()
{
	bool enable = m_topSliceCheckBox->isChecked() && m_sliceModel != nullptr;
	m_topSlicePlayAction->setEnabled(enable);
	m_topSlider->setEnabled(enable);
	m_topView->setHidden(!enable);
}

void ImageCanvas::updateFrontSliceActions()
{
	bool enable = m_frontSliceCheckBox->isChecked() && m_sliceModel != nullptr;
	m_frontSlicePlayAction->setEnabled(enable);
	m_frontSlider->setEnabled(enable);
	m_frontView->setHidden(!enable);
}

void ImageCanvas::updateRightSliceActions()
{
	bool enable = m_rightSliceCheckBox->isChecked() && m_sliceModel != nullptr;
	m_rightSlicePlayAction->setEnabled(enable);
	m_rightSlider->setEnabled(enable);
	m_rightView->setHidden(!enable);
}

void ImageCanvas::installMarkModel(MarkModel* model)
{
	Q_ASSERT_X(m_sliceModel,
		"ImageView::updateMarkModel", "null pointer");

	m_markModel = model;
	connect(m_markModel,&MarkModel::modified,this,&ImageCanvas::markModified);
	QVector<QPair<QString, QColor>> cates;
	if (m_markModel != nullptr) {
		m_markModel->m_view = this;
		m_markModel->m_dataModel = m_sliceModel;
		auto cateItems = m_markModel->categoryItems();
		QVector<QPair<QString, QColor>> cates;
		foreach(const auto & item, cateItems)
			cates << qMakePair(item->name(), item->color());
	}
	updateMarks(SliceType::Top);
	updateMarks(SliceType::Right);
	updateMarks(SliceType::Front);

	setCategoryManagerHelper(cates);

}

void ImageCanvas::updateSliceModel()
{
	updateSliceCount(SliceType::Top);
	updateSliceCount(SliceType::Right);
	updateSliceCount(SliceType::Front);

	updateSlice(SliceType::Front,0);
	updateSlice(SliceType::Right,0);
	updateSlice(SliceType::Top,0);
}

void ImageCanvas::detachMarkModel()
{
	if (m_markModel != nullptr)
		m_markModel->detachFromView();
}


void ImageCanvas::createContextMenu()
{
	m_contextMenu = new QMenu(this);
	///TODO:: add icons here

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
			connect(this, &ImageCanvas::topSliceOpened, histViewDlg, &AbstractPlugin::sliceOpened);
			connect(this, &ImageCanvas::topSlicePlayStoped, histViewDlg, &AbstractPlugin::slicePlayStoped);
			connect(this, &ImageCanvas::topSliceChanged, histViewDlg, &AbstractPlugin::sliceChanged);
			emit topSliceOpened(m_topSlider->value());
		}
		else if (m_menuWidget == m_rightView)
		{
			histViewDlg = new HistogramViewer(SliceType::Right, QStringLiteral("Right Slice Histogram"), m_rightView, m_sliceModel, this);
			histViewDlg->setWindowFlag(Qt::Window);
			histViewDlg->show();
			histViewDlg->setAttribute(Qt::WA_DeleteOnClose);
			//histViewDlg->sliceOpenEvent(m_rightSlider->value());
			connect(this, &ImageCanvas::rightSliceOpened, histViewDlg, &AbstractPlugin::sliceOpened);
			connect(this, &ImageCanvas::rightSlicePlayStoped, histViewDlg, &AbstractPlugin::slicePlayStoped);
			connect(this, &ImageCanvas::rightSliceChanged, histViewDlg, &AbstractPlugin::sliceChanged);
			emit rightSliceOpened(m_rightSlider->value());
		}
		else if (m_menuWidget == m_frontView)
		{
			histViewDlg = new HistogramViewer(SliceType::Front, QStringLiteral("Front Slice Histogram"), m_frontView, m_sliceModel, this);
			histViewDlg->setWindowFlag(Qt::Window);
			histViewDlg->show();
			histViewDlg->setAttribute(Qt::WA_DeleteOnClose);
			//histViewDlg->sliceOpenEvent(m_frontSlider->value());
			connect(this, &ImageCanvas::frontSliceOpened, histViewDlg, &AbstractPlugin::sliceOpened);
			connect(this, &ImageCanvas::frontSlicePlayStoped, histViewDlg, &AbstractPlugin::slicePlayStoped);
			connect(this, &ImageCanvas::frontSliceChanged, histViewDlg, &AbstractPlugin::sliceChanged);
			emit frontSliceOpened(m_frontSlider->value());
		}
		//connect(this, &ImageView::sliceChanged, histViewDlg, &AbstractPlugin::sliceChanged);
		connect(this, &ImageCanvas::topSlicePlayStoped, histViewDlg, &AbstractPlugin::slicePlayStoped);
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
			connect(this, &ImageCanvas::topSliceOpened, pixelViewDlg, &AbstractPlugin::sliceOpened);
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
			connect(this, &ImageCanvas::rightSliceOpened, pixelViewDlg, &AbstractPlugin::sliceOpened);
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
			connect(this, &ImageCanvas::frontSliceOpened, pixelViewDlg, &AbstractPlugin::sliceOpened);
			emit frontSliceOpened(m_frontSlider->value());
		}
	});

	m_contextMenu->addAction(m_zoomInAction);
	m_contextMenu->addAction(m_zoomOutAction);
	m_contextMenu->addSeparator();
	m_contextMenu->addAction(m_histDlgAction);
	m_contextMenu->addAction(m_pixelViewDlgAction);
	m_contextMenu->addAction(m_marksManagerDlgAction);
}

QIcon ImageCanvas::createColorIcon(const QColor & color)
{
	constexpr int iconSize = 64;
	constexpr int barHeight = 20;

	QPixmap pixmap(iconSize, iconSize + barHeight);
	pixmap.fill(Qt::transparent);
	QPainter painter(&pixmap);
	QPixmap image(":icons/resources/icons/color.png");
	// Draw icon centred horizontally on button.
	QRect target(0,0,iconSize, iconSize);
	painter.fillRect(QRect(0,iconSize,iconSize,barHeight), color);
	painter.drawPixmap(target, image);
	return QIcon(pixmap);
}

ImageCanvas::ImageCanvas(QWidget *parent, bool topSliceVisible, bool rightSliceVisible, bool frontSliceVisible, AbstractSliceDataModel * model) :
	QWidget(parent),
	m_markModel(nullptr),
	m_sliceModel(model)
{
	m_layout = new QGridLayout;
	m_topView = new SliceView(this);
	m_rightView = new SliceView(this);
	m_frontView = new SliceView(this);
	m_topView->setNavigationViewEnabled(true);
	m_rightView->setNavigationViewEnabled(false);
	m_frontView->setNavigationViewEnabled(false);

	m_resetAction = new QAction(QIcon(":icons/resources/icons/reset.png"),QStringLiteral("Reset"), this);
	m_resetAction->setToolTip(QStringLiteral("Reset"));

	//m_renderView = new VolumeWidget(nullptr, nullptr, this);



	//m_layout->addWidget(m_reset, 3, 1, 1, 1, Qt::AlignCenter);
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
	createWidgets();
	createToolBar();
	createContextMenu();
	createConnections();

	updateActions();

	m_penSizeCBBox->setCurrentIndex(4);
	setWindowTitle(QStringLiteral("ImageCanvas"));
	//m_layout->setSizeConstraint(QLayout::SetFixedSize);
	m_layout->addWidget(m_topView, 2, 0, 1, 1, Qt::AlignCenter);
	m_layout->addWidget(m_rightView, 2, 1, 1, 1, Qt::AlignCenter);
	m_layout->addWidget(m_frontView, 3, 0, 1, 1, Qt::AlignCenter);
	//m_layout->addWidget(m_renderView, 0, 3, 4, 1, Qt::AlignCenter);
	m_layout->addWidget(m_viewToolBar, 0, 0, 1, 2);
	m_layout->addWidget(m_editToolBar, 1, 0, 1, 2);
	setLayout(m_layout);
}

void ImageCanvas::changeSliceHelper(int value, SliceType type)
{
	Q_ASSERT_X(m_sliceModel != nullptr, "ImageView::sliceChanged", "null pointer");
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

inline void ImageCanvas::setTopSliceCountHelper(int value) { m_topSlider->setMaximum(value - 1); }
inline void ImageCanvas::setRightSliceCountHelper(int value) { m_rightSlider->setMaximum(value - 1); }
inline void ImageCanvas::setFrontSliceCountHelper(int value) { m_frontSlider->setMaximum(value - 1); }
int ImageCanvas::currentIndexHelper(SliceType type)
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

MarkModel* ImageCanvas::createMarkModel(ImageCanvas *view, AbstractSliceDataModel * d)
{
	return new MarkModel(d, view,
		new TreeItem(QVector<QVariant>{QStringLiteral("Name"), QStringLiteral("Desc")}, TreeItemType::Root),
		nullptr);
}

void ImageCanvas::markAddedHelper(SliceType type, QGraphicsItem * mark)
{
	QString cate = m_categoryCBBox->currentText();
	QVariant categoryColor = m_categoryCBBox->currentData();

	if (cate.isEmpty())
	{
		//Add a default catetory
		cate = QStringLiteral("Category#%1").arg(m_categoryCBBox->count());
		categoryColor = QVariant::fromValue(Qt::black);
		QPen pen = m_topView->pen();
		pen.setColor(Qt::black);
		updatePen(pen);
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
	switch (type)
	{
	case SliceType::Top:
		index = m_topSlider->value();
		color = m_topView->pen().color();
		break;
	case SliceType::Right:
		index = m_rightSlider->value();
		color = m_rightView->pen().color();
		break;
	case SliceType::Front:
		index = m_frontSlider->value();
		color = m_frontView->pen().color();
		break;
	}
	mark->setData(MarkProperty::SliceIndex, QVariant::fromValue(index));
	mark->setData(MarkProperty::Color, color);
	Q_ASSERT_X(m_markModel != nullptr,
		"mark_create_helper_", "null pointer");

	m_markModel->addMark(cate, mark);
}

void ImageCanvas::markDeleteHelper()
{
	Q_ASSERT_X(m_markModel, "ImageView::markDeleteHelper", "null pointer");
	QList<QGraphicsItem*> items;
	if (m_topView != nullptr)
		foreach(auto item, m_topView->selectedItems())
		items << item;
	if (m_rightView != nullptr)
		foreach(auto item, m_rightView->selectedItems())
		items << item;
	if (m_frontView != nullptr)
		foreach(auto item, m_frontView->selectedItems())
		items << item;
	m_markModel->removeMarks(items);
}

void ImageCanvas::markSingleSelectionHelper()
{
	int count = m_topView->selectedItemCount() + m_rightView->selectedItemCount() + m_frontView->selectedItemCount();
	if (count != 1)
		return;
	QGraphicsItem * item = nullptr;
	if (m_topView->selectedItemCount() == 1)
		item = m_topView->selectedItems()[0];
	else if (m_rightView->selectedItemCount() == 1)
		item = m_rightView->selectedItems()[0];
	else if (m_frontView->selectedItemCount() == 1)
		item = m_frontView->selectedItems()[0];
	emit markSeleteced(item);
}

void ImageCanvas::setCategoryManagerHelper(const QVector<QPair<QString, QColor>>& cates)
{
	Q_ASSERT_X(m_categoryCBBox, "ImageVIew::initCCBoxHelper", "null pointer");
	m_categoryCBBox->clear();
	foreach(const auto & p, cates)
		m_categoryCBBox->addItem(p.first, QVariant::fromValue(p.second));
}

void ImageCanvas::addCategoryManagerHelper(const QString & name, const QColor & color)
{
	m_categoryCBBox->addItem(name, color);
	m_categoryCBBox->setCurrentText(name);
}



SliceView* ImageCanvas::focusOn()
{
	if (m_topView->hasFocus())
		return m_topView;
	if (m_rightView->hasFocus())
		return m_rightView;
	if (m_frontView->hasFocus())
		return m_frontView;
	return nullptr;
}

int ImageCanvas::topSliceIndex() const { return m_topSlider->value(); }

int ImageCanvas::rightSliceIndex() const { return m_rightSlider->value(); }

int ImageCanvas::frontSliceIndex() const { return m_frontSlider->value(); }

bool ImageCanvas::isTopSliceEnabled() const
{
	return m_topSliceCheckBox->isChecked();
}

bool ImageCanvas::isRightSliceEnabled() const
{
	return m_rightSliceCheckBox->isChecked();
}

bool ImageCanvas::isFrontSliceEnabled() const
{
	return m_topSliceCheckBox->isChecked();
}

QPen ImageCanvas::pen() const
{
	return m_topView->pen();
}

void ImageCanvas::setPen(const QPen & pen)
{
	updatePen(pen);
}
 void ImageCanvas::topSliceEnable(bool enable)
{
	m_topSliceCheckBox->setChecked(enable);
	updateActions();
}

 void ImageCanvas::rightSliceEnable(bool enable)
{
	m_rightSliceCheckBox->setChecked(enable);
	updateActions();
}

 void ImageCanvas::frontSliceEnable(bool enable)
{
	m_frontSliceCheckBox->setChecked(enable);
	updateActions();
}

void ImageCanvas::updatePen(const QPen &pen)
{
	m_topView->setPen(pen);
	m_rightView->setPen(pen);
	m_frontView->setPen(pen);
	const auto & c = pen.color();
	m_penSizeCBBox->setCurrentIndex(pen.width() - 1);
	m_colorAction->setIcon(createColorIcon(c));
}

AbstractSliceDataModel* ImageCanvas::takeSliceModel(AbstractSliceDataModel* model)
{
	auto t = m_sliceModel;
	m_sliceModel = model;
	detachMarkModel();
	installMarkModel(createMarkModel(this, m_sliceModel));
	updateSliceModel();
	updateActions();
	emit dataModelChanged();
	return t;
}
MarkModel* ImageCanvas::takeMarkModel(MarkModel* model, bool * success)noexcept
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
		detachMarkModel();
		auto t = m_markModel;
		installMarkModel(nullptr);
		updateActions();
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
	detachMarkModel();
	auto t = m_markModel;
	installMarkModel(model);
	updateActions();
	if (success != nullptr)
		*success = true;
	emit markModelChanged();
	return t;
}
MarkModel * ImageCanvas::markModel()
{
	return m_markModel;
}

void ImageCanvas::resetZoom()
{
	m_topView->resetMatrix();
	m_rightView->resetMatrix();
	m_frontView->resetMatrix();
}

void ImageCanvas::zoomIn()
{
	double factor = std::pow(1.125, 1);
	m_topView->scale(factor, factor);
	m_rightView->scale(factor, factor);
	m_frontView->scale(factor, factor);
}

void ImageCanvas::zoomOut()
{
	double factor = std::pow(1.125, -1);
	m_topView->scale(factor, factor);
	m_rightView->scale(factor, factor);
	m_frontView->scale(factor, factor);
}

void ImageCanvas::categoryAdded()
{
	MarkCategoryDialog dlg(this);
	connect(&dlg, &MarkCategoryDialog::resultReceived, [this](const QString & name, const QColor & color)
	{
		addCategoryManagerHelper(name, color);
		QPen pen = m_topView->pen();
		pen.setColor(color);
		updatePen(pen);
	});
	dlg.exec();
}

void ImageCanvas::colorChanged()
{
	auto d = m_categoryCBBox->itemData(m_categoryCBBox->currentIndex());
	QColor defaultColor = d.canConvert<QColor>() ? d.value<QColor>() : Qt::black;
	QPen pen = m_topView->pen();
	pen.setColor(QColorDialog::getColor(defaultColor, this, QStringLiteral("Color")));
	updatePen(pen);
}

void ImageCanvas::setEnabled(bool enable)
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


void ImageCanvas::onTopSlicePlay(bool enable)
{
	if (enable)
	{
		m_topTimerId = startTimer(50);
		//qDebug() << "onTopSliceTimer" << m_topTimerId;
		m_topSlicePlayDirection = PlayDirection::Forward;
	}
	else
	{
		killTimer(m_topTimerId);
		m_topTimerId = 0;
	}
}

void ImageCanvas::onRightSlicePlay(bool enable)
{
	if (enable)
	{
		m_rightTimerId = startTimer(50);
		//qDebug() << "onRightSliceTimer" << m_rightTimerId;
		m_rightSlicePlayDirection = PlayDirection::Forward;
	}
	else
	{
		killTimer(m_rightTimerId);
		m_rightTimerId = 0;
	}
}

void ImageCanvas::onFrontSlicePlay(bool enable)
{
	if (enable)
	{
		m_frontTimerId = startTimer(50);
		//qDebug() << "onFrontSliceTimer" << m_frontTimerId;
		m_frontSlicePlayDirection = PlayDirection::Forward;
	}
	else
	{
		killTimer(m_frontTimerId);
		m_frontTimerId = 0;
	}
}

void ImageCanvas::timerEvent(QTimerEvent* event)
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

void ImageCanvas::contextMenuEvent(QContextMenuEvent* event)
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

void ImageCanvas::updateSliceCount(SliceType type)
{
	switch (type)
	{
	case SliceType::Top:
		setTopSliceCountHelper(m_sliceModel->topSliceCount() - 1);
		break;
	case SliceType::Right:
		setRightSliceCountHelper(m_sliceModel->rightSliceCount() - 1);
		break;
	case SliceType::Front:
		setFrontSliceCountHelper(m_sliceModel->frontSliceCount() - 1);
		break;
	default:
		break;
	}
}

void ImageCanvas::updateSlice(SliceType type, int index)
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
	view->setImage(sliceGetter(index));
	view->clearSliceMarks();
	if (m_markModel == nullptr)
		return;
	//Q_ASSERT_X(m_markModel, "ImageView::updateSlice", "null pointer");
	view->setMarks((*list)[index]);
}

void ImageCanvas::updateMarks(SliceType type)
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
