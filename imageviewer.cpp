#include <QToolBar>
#include <complex>
#include <QMenu>
#include <QComboBox>
#include <QMessageBox>
#include <QDebug>

#include "imageviewer.h"
#include "globals.h"
#include "abstractslicedatamodel.h"
#include "histogram.h"
#include "pixelviewer.h"
#include "markmodel.h"
#include "markitem.h"
#include "sliceview.h"
#include "categoryitem.h"
#include "volumewidget.h"
#include "imageviewcontrolpanel.h"

inline bool ImageCanvas::contains(const QWidget* widget, const QPoint& pos)
{
	return (widget->rect()).contains(pos);
}
void ImageCanvas::createWidgets()
{
}

void ImageCanvas::createToolBar()
{
	m_zoomInAction = new QAction(QIcon(":icons/resources/icons/zoom_in.png"),QStringLiteral("Zoom In"), this);
	m_zoomInAction->setToolTip(QStringLiteral("Zoom In"));
	m_zoomOutAction = new QAction(QIcon(":icons/resources/icons/zoom_out.png"),QStringLiteral("Zoom Out"), this);
	m_zoomOutAction->setToolTip(QStringLiteral("Zoom Out"));
}

void ImageCanvas::createConnections()
{
	//forward selected signals
	connect(m_topView, &SliceView::sliceSelected, this, &ImageCanvas::topSliceSelected);
	connect(m_rightView, &SliceView::sliceSelected, this, &ImageCanvas::rightSliceSelected);
	connect(m_frontView, &SliceView::sliceSelected, this, &ImageCanvas::frontSliceSelected);

	connect(m_topView, &SliceView::markAdded, [this](QGraphicsItem* mark) {markAddedHelper(SliceType::Top, mark); });
	connect(m_rightView, &SliceView::markAdded, [this](QGraphicsItem* mark) {markAddedHelper(SliceType::Right, mark); });
	connect(m_frontView, &SliceView::markAdded, [this](QGraphicsItem* mark) {markAddedHelper(SliceType::Front, mark); });

	connect(m_topView, &SliceView::viewMoved, [this](const QPointF & delta) {m_rightView->translate(0.0f, delta.y()); m_frontView->translate(delta.x(), 0.0f); });

	connect(m_topView, &SliceView::selectionChanged, this, &ImageCanvas::updateDeleteAction);
	connect(m_rightView, &SliceView::selectionChanged, this, &ImageCanvas::updateDeleteAction);
	connect(m_frontView, &SliceView::selectionChanged, this, &ImageCanvas::updateDeleteAction);

	connect(m_topView, &SliceView::selectionChanged, this, &ImageCanvas::markSingleSelectionHelper);
	connect(m_rightView, &SliceView::selectionChanged, this, &ImageCanvas::markSingleSelectionHelper);
	connect(m_frontView, &SliceView::selectionChanged, this, &ImageCanvas::markSingleSelectionHelper);

	connect(m_zoomInAction, &QAction::triggered, this, &ImageCanvas::zoomIn);
	connect(m_zoomOutAction, &QAction::triggered, this, &ImageCanvas::zoomOut);
}

void ImageCanvas::updateActions()
{
	bool enable = m_sliceModel != nullptr;
	updateDeleteAction();
	const auto topVis = m_panel ? m_panel->sliceVisible(SliceType::Top) : true;
	const auto rightVis = m_panel ? m_panel->sliceVisible(SliceType::Right) : true;
	const auto frontVis = m_panel ? m_panel->sliceVisible(SliceType::Front) : true;
	updateTopSliceActions(topVis);
	updateRightSliceActions(rightVis);
	updateFrontSliceActions(frontVis);
}

void ImageCanvas::updateDeleteAction()
{
	if (m_panel == nullptr)return;
	m_panel->updateDeleteActionPrivate();
}

void ImageCanvas::updateTopSliceActions(bool check)
{
	bool enable = check && m_sliceModel != nullptr;
	m_topView->setHidden(!enable);
}

void ImageCanvas::updateFrontSliceActions(bool check)
{
	bool enable = check && m_sliceModel != nullptr;
	m_frontView->setHidden(!enable);
}

void ImageCanvas::updateRightSliceActions(bool check)
{
	bool enable = check && m_sliceModel != nullptr;
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

	//m_panel

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
			emit topSliceOpened(currentSliceIndex(SliceType::Top));
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
			emit rightSliceOpened(currentSliceIndex(SliceType::Right));
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
			emit frontSliceOpened(currentSliceIndex(SliceType::Front));
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
			emit topSliceOpened(currentSliceIndex(SliceType::Top));

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
			emit rightSliceOpened(currentSliceIndex(SliceType::Right));
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
			emit frontSliceOpened(currentSliceIndex(SliceType::Front));
		}
	});

	m_contextMenu->addAction(m_zoomInAction);
	m_contextMenu->addAction(m_zoomOutAction);
	m_contextMenu->addSeparator();
	m_contextMenu->addAction(m_histDlgAction);
	m_contextMenu->addAction(m_pixelViewDlgAction);
	m_contextMenu->addAction(m_marksManagerDlgAction);
}

ImageCanvas::ImageCanvas(QWidget *parent, 
	bool topSliceVisible,
	bool rightSliceVisible,
	bool frontSliceVisible, 
	AbstractSliceDataModel * model) :
	QWidget(parent),
	m_markModel(nullptr),
	m_sliceModel(model),
	m_panel(nullptr)
{
	m_layout = new QGridLayout;
	m_topView = new SliceView(this);
	m_rightView = new SliceView(this);
	m_frontView = new SliceView(this);

	m_topView->setNavigationViewEnabled(true);
	m_rightView->setNavigationViewEnabled(false);
	m_frontView->setNavigationViewEnabled(false);

	createWidgets();
	createToolBar();
	createContextMenu();
	createConnections();
	updateActions();
	setWindowTitle(QStringLiteral("ImageCanvas"));
	m_layout->addWidget(m_topView, 0, 0, 1, 1, Qt::AlignCenter);
	m_layout->addWidget(m_rightView, 0, 1, 1, 1, Qt::AlignCenter);
	m_layout->addWidget(m_frontView, 1, 0, 1, 1, Qt::AlignCenter);
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


int ImageCanvas::currentIndexHelper(SliceType type)
{
	return currentSliceIndex(type);
}

MarkModel* ImageCanvas::createMarkModel(ImageCanvas *view, AbstractSliceDataModel * d)
{
	return new MarkModel(d, view,
		new TreeItem(QVector<QVariant>{QStringLiteral("Name"), QStringLiteral("Desc")}, TreeItemType::Root),
		nullptr);
}

void ImageCanvas::markAddedHelper(SliceType type, QGraphicsItem * mark)
{
	Q_ASSERT_X(m_panel, "ImageCanvas::markAddedHelper", "null pointer");
	QString cate = m_panel->currentCategoryName();
	QVariant categoryColor = m_panel->currentCategoryColor();

	if (cate.isEmpty())
	{
		//Add a default catetory
		cate = QStringLiteral("Category#%1").arg(m_panel->categoryCount());
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
	index = m_panel->sliceIndex(type);
	switch (type)
	{
	case SliceType::Top:
		color = m_topView->pen().color();
		break;
	case SliceType::Right:
		color = m_rightView->pen().color();
		break;
	case SliceType::Front:
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
	if (m_panel == nullptr)return;
	m_panel->setCategoryInfoPrivate(cates);
}

void ImageCanvas::addCategoryManagerHelper(const QString & name, const QColor & color)
{
	if (m_panel == nullptr)return;
	m_panel->addCategoryInfoPrivate(name, color);
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
bool ImageCanvas::topSliceVisible() const
{
	return m_topView->isHidden();
}
bool ImageCanvas::rightSliceVisible() const
{
	return m_rightView->isHidden();
}
bool ImageCanvas::frontSliceVisible() const
{
	return m_frontView->isHidden();
}
QPen ImageCanvas::pen() const
{
	return m_topView->pen();
}
void ImageCanvas::setPen(const QPen & pen)
{
	updatePen(pen);
}
 void ImageCanvas::setTopSliceVisible(bool enable)
{
	 m_topView->setVisible(enable);
	updateActions();
}
 void ImageCanvas::setRightSliceVisible(bool enable)
{
	m_rightView->setVisible(enable);
	updateActions();
}
 void ImageCanvas::setFrontSliceVisible(bool enable)
{
	m_frontView->setVisible(enable);
	updateActions();
}
void ImageCanvas::updatePen(const QPen &pen)
{
	m_topView->setPen(pen);
	m_rightView->setPen(pen);
	m_frontView->setPen(pen);
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

int ImageCanvas::currentSliceIndex(SliceType type) const
{
	//static int topSliceIndex;
	//static int rightSliceIndex;
	//static int frontSliceIndex;
	if(m_panel == nullptr)
	{
		switch(type)
		{
		case SliceType::Top:
			return 0;
		case SliceType::Right:
			return 0;
		case SliceType::Front:
			return 0;
		}
	}else
	{
		return m_panel->sliceIndex(type);
	}
}

void ImageCanvas::resetZoom(bool check)
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

void ImageCanvas::setOperation(SliceType type, int opt)
{
	switch(type)
	{
	case SliceType::Top:
		m_topView->setOperation(opt);
		break;
	case SliceType::Right:
		m_rightView->setOperation(opt);
		break;
	case SliceType::Front:
		m_frontView->setOperation(opt);
		break;
	}
}
void ImageCanvas::setEnabled(bool enable)
{
	m_topView->setEnabled(enable);
	m_rightView->setEnabled(enable);
	m_frontView->setEnabled(enable);

}


void ImageCanvas::onTopSlicePlay(bool enable)
{
	if (enable)
	{
		m_topTimerId = startTimer(50);
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
	Q_ASSERT_X(m_panel, "ImageCanvas::timerEvent", "null pointer");
	Q_ASSERT_X(m_sliceModel, "ImageCanvas::timerEvent", "null pointer");
	int timeId = event->timerId();
	if (timeId == m_topTimerId)
	{
		int maxSlice = m_sliceModel->topSliceCount();
		int cur = m_panel->sliceIndex(SliceType::Top);
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
		//m_topSlider->setValue(cur);
		m_panel->setSliceIndex(SliceType::Top, cur);
	}
	else if (timeId == m_rightTimerId)
	{
		int maxSlice = m_sliceModel->rightSliceCount();
		int cur = m_panel->sliceIndex(SliceType::Right);
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
		//m_rightSlider->setValue(cur);
		m_panel->setSliceIndex(SliceType::Right, cur);

	}
	else if (timeId == m_frontTimerId)
	{
		int maxSlice = m_sliceModel->frontSliceCount();
		int cur = m_panel->sliceIndex(SliceType::Front);
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
		//m_frontSlider->setValue(cur);
		m_panel->setSliceIndex(SliceType::Front, cur);
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
	Q_ASSERT_X(m_panel, "ImageCanvas::updateSliceCount", "null pointer");
	int count = -1;
	switch (type)
	{
	case SliceType::Top:
		count = m_sliceModel->topSliceCount();
		break;
	case SliceType::Right:
		count = m_sliceModel->rightSliceCount();
		break;
	case SliceType::Front:
		count = m_sliceModel->frontSliceCount();
		break;
	default:
		break;
	}
	m_panel->setSliceCount(type, count-1);
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
	Q_ASSERT_X(m_markModel,"ImageCanvas::updateMarks", "mark model null pointer");
	Q_ASSERT_X(m_panel, "ImageCanvas::updateMarks", "panel null pointer");
	switch (type)
	{
	case SliceType::Top:
	{
		//QList<QGraphicsItem*> res;
		auto m = m_markModel->topSliceVisibleMarks()[m_panel->sliceIndex(type)];
		//for (int i = 0; i < m.size(); i++)
			//res.append(QueryMarkItemInterface<QGraphicsItem*,PolyMarkItem*>(m[i]));
		m_topView->setMarks(m);
	}
	break;
	case SliceType::Right:
	{
		//QList<QGraphicsItem*> res;
		auto m = m_markModel->rightSliceVisibleMarks()[m_panel->sliceIndex(type)];
		//for (int i = 0; i < m.size(); i++)
			//res.append(QueryMarkItemInterface<QGraphicsItem*, PolyMarkItem*>(m[i]));
		m_rightView->setMarks(m);
	}
	break;
	case SliceType::Front:
	{
		//QList<QGraphicsItem*> res;
		auto m = m_markModel->frontSliceVisibleMarks()[m_panel->sliceIndex(type)];
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
