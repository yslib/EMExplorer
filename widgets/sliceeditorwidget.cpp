#include <QToolBar>
#include <complex>
#include <QMenu>
#include <QComboBox>
#include <QMessageBox>
#include <QTimer>

#include "globals.h"
#include "abstract/abstractslicedatamodel.h"
#include "model/markmodel.h"
#include "model/markitem.h"
#include "model/categoryitem.h"
#include "histogramwidget.h"
#include "pixelwidget.h"
#include "sliceeditorwidget.h"
#include "slicewidget.h"
#include "renderwidget.h"

inline bool SliceEditorWidget::contains(const QWidget* widget, const QPoint& pos)
{
	return (widget->rect()).contains(pos);
}
void SliceEditorWidget::createWidgets()
{

}

void SliceEditorWidget::createToolBar()
{
	m_zoomInAction = new QAction(QIcon(":icons/resources/icons/zoom_in.png"), QStringLiteral("Zoom In"), this);
	m_zoomInAction->setToolTip(QStringLiteral("Zoom In"));
	m_zoomOutAction = new QAction(QIcon(":icons/resources/icons/zoom_out.png"), QStringLiteral("Zoom Out"), this);
	m_zoomOutAction->setToolTip(QStringLiteral("Zoom Out"));
}

void SliceEditorWidget::createConnections()
{
	//forward selected signals
	connect(m_topView, &SliceWidget::sliceSelected, this, &SliceEditorWidget::topSliceSelected);
	connect(m_rightView, &SliceWidget::sliceSelected, this, &SliceEditorWidget::rightSliceSelected);
	connect(m_frontView, &SliceWidget::sliceSelected, this, &SliceEditorWidget::frontSliceSelected);

	connect(m_topView, &SliceWidget::markAdded, [this](QGraphicsItem* mark) {markAddedHelper(SliceType::Top, mark); });
	connect(m_rightView, &SliceWidget::markAdded, [this](QGraphicsItem* mark) {markAddedHelper(SliceType::Right, mark); });
	connect(m_frontView, &SliceWidget::markAdded, [this](QGraphicsItem* mark) {markAddedHelper(SliceType::Front, mark); });

	connect(m_topView, &SliceWidget::viewMoved, [this](const QPointF & delta) {m_rightView->translate(0.0f, delta.y()); m_frontView->translate(delta.x(), 0.0f); });

	connect(m_topView, &SliceWidget::selectionChanged, this, &SliceEditorWidget::updateDeleteAction);
	connect(m_rightView, &SliceWidget::selectionChanged, this, &SliceEditorWidget::updateDeleteAction);
	connect(m_frontView, &SliceWidget::selectionChanged, this, &SliceEditorWidget::updateDeleteAction);

	connect(m_topView, &SliceWidget::selectionChanged, this, &SliceEditorWidget::markSingleSelectionHelper);
	connect(m_rightView, &SliceWidget::selectionChanged, this, &SliceEditorWidget::markSingleSelectionHelper);
	connect(m_frontView, &SliceWidget::selectionChanged, this, &SliceEditorWidget::markSingleSelectionHelper);

	connect(m_zoomInAction, &QAction::triggered, this, &SliceEditorWidget::zoomIn);
	connect(m_zoomOutAction, &QAction::triggered, this, &SliceEditorWidget::zoomOut);
}

void SliceEditorWidget::updateActions()
{
	bool enable = m_sliceModel != nullptr;

	updateDeleteAction();

	//const auto topVis = m_panel ? m_panel->sliceVisible(SliceType::Top) : true;
	//const auto rightVis = m_panel ? m_panel->sliceVisible(SliceType::Right) : true;
	//const auto frontVis = m_panel ? m_panel->sliceVisible(SliceType::Front) : true;

	updateTopSliceActions(true);
	updateRightSliceActions(true);
	updateFrontSliceActions(true);
}

void SliceEditorWidget::updateDeleteAction()
{
	return;
	//if (m_panel == nullptr)return;
	//m_panel->updateDeleteActionPrivate();
}

void SliceEditorWidget::updateTopSliceActions(bool check)
{
	bool enable = check && m_sliceModel != nullptr;
	m_topView->setHidden(!enable);
}

void SliceEditorWidget::updateFrontSliceActions(bool check)
{
	bool enable = check && m_sliceModel != nullptr;
	m_frontView->setHidden(!enable);
}

void SliceEditorWidget::updateRightSliceActions(bool check)
{
	bool enable = check && m_sliceModel != nullptr;
	m_rightView->setHidden(!enable);
}

void SliceEditorWidget::installMarkModel(MarkModel* model)
{
	Q_ASSERT_X(m_sliceModel,
		"ImageView::updateMarkModel", "null pointer");

	m_markModel = model;
	connect(m_markModel, &MarkModel::modified, this, &SliceEditorWidget::markModified);

	//QVector<QPair<QString, QColor>> cates;
	//if (m_markModel != nullptr) {
	//	m_markModel->m_view = this;
	//	m_markModel->m_dataModel = m_sliceModel;
	//	auto cateItems = m_markModel->categoryItems();
	//	QVector<QPair<QString, QColor>> cates;
	//	foreach(const auto & item, cateItems)
	//		cates << qMakePair(item->name(), item->color());
	//}

	updateMarks(SliceType::Top);
	updateMarks(SliceType::Right);
	updateMarks(SliceType::Front);

	//setCategoryManagerHelper(cates);

}

void SliceEditorWidget::updateSliceModel()
{
	//updateSliceCount(SliceType::Top);
	//updateSliceCount(SliceType::Right);
	//updateSliceCount(SliceType::Front);

	setSliceIndex(SliceType::Front, 0);
	setSliceIndex(SliceType::Right, 0);
	setSliceIndex(SliceType::Top, 0);
}

void SliceEditorWidget::detachMarkModel()
{
	if (m_markModel != nullptr)
		m_markModel->detachFromView();
}


void SliceEditorWidget::createContextMenu()
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
			histViewDlg = new HistogramWidget(SliceType::Top, QStringLiteral("Top Slice Histogram"), m_topView, m_sliceModel, this);
			histViewDlg->setWindowFlag(Qt::Window);
			histViewDlg->show();
			histViewDlg->setAttribute(Qt::WA_DeleteOnClose);
			//histViewDlg->sliceOpenEvent(m_topSlider->value());
			connect(this, &SliceEditorWidget::topSliceOpened, histViewDlg, &AbstractPlugin::sliceOpened);
			connect(this, &SliceEditorWidget::topSlicePlayStoped, histViewDlg, &AbstractPlugin::slicePlayStoped);
			connect(this, &SliceEditorWidget::topSliceChanged, histViewDlg, &AbstractPlugin::sliceChanged);
			emit topSliceOpened(currentSliceIndex(SliceType::Top));
		}
		else if (m_menuWidget == m_rightView)
		{
			histViewDlg = new HistogramWidget(SliceType::Right, QStringLiteral("Right Slice Histogram"), m_rightView, m_sliceModel, this);
			histViewDlg->setWindowFlag(Qt::Window);
			histViewDlg->show();
			histViewDlg->setAttribute(Qt::WA_DeleteOnClose);
			//histViewDlg->sliceOpenEvent(m_rightSlider->value());
			connect(this, &SliceEditorWidget::rightSliceOpened, histViewDlg, &AbstractPlugin::sliceOpened);
			connect(this, &SliceEditorWidget::rightSlicePlayStoped, histViewDlg, &AbstractPlugin::slicePlayStoped);
			connect(this, &SliceEditorWidget::rightSliceChanged, histViewDlg, &AbstractPlugin::sliceChanged);
			emit rightSliceOpened(currentSliceIndex(SliceType::Right));
		}
		else if (m_menuWidget == m_frontView)
		{
			histViewDlg = new HistogramWidget(SliceType::Front, QStringLiteral("Front Slice Histogram"), m_frontView, m_sliceModel, this);
			histViewDlg->setWindowFlag(Qt::Window);
			histViewDlg->show();
			histViewDlg->setAttribute(Qt::WA_DeleteOnClose);
			//histViewDlg->sliceOpenEvent(m_frontSlider->value());
			connect(this, &SliceEditorWidget::frontSliceOpened, histViewDlg, &AbstractPlugin::sliceOpened);
			connect(this, &SliceEditorWidget::frontSlicePlayStoped, histViewDlg, &AbstractPlugin::slicePlayStoped);
			connect(this, &SliceEditorWidget::frontSliceChanged, histViewDlg, &AbstractPlugin::sliceChanged);
			emit frontSliceOpened(currentSliceIndex(SliceType::Front));
		}
		//connect(this, &ImageView::sliceChanged, histViewDlg, &AbstractPlugin::sliceChanged);
		connect(this, &SliceEditorWidget::topSlicePlayStoped, histViewDlg, &AbstractPlugin::slicePlayStoped);
	});
	connect(m_pixelViewDlgAction, &QAction::triggered, [this]()
	{
		PixelWidget * pixelViewDlg;
		if (m_menuWidget == m_topView)
		{
			pixelViewDlg = new PixelWidget(SliceType::Top, QStringLiteral("Top Slice Pixel View"), m_topView, m_sliceModel, this);
			pixelViewDlg->setWindowFlag(Qt::Window);
			pixelViewDlg->show();
			pixelViewDlg->setAttribute(Qt::WA_DeleteOnClose);
			//			pixelViewDlg->setImage(m_sliceModel->topSlice(m_topSlider->value()));
			connect(m_topView, &SliceWidget::sliceSelected, pixelViewDlg, &AbstractPlugin::sliceSelected);
			connect(this, &SliceEditorWidget::topSliceOpened, pixelViewDlg, &AbstractPlugin::sliceOpened);
			emit topSliceOpened(currentSliceIndex(SliceType::Top));

		}
		else if (m_menuWidget == m_rightView)
		{
			pixelViewDlg = new PixelWidget(SliceType::Right, QStringLiteral("Right Slice Pixel View"), m_rightView, m_sliceModel, this);
			pixelViewDlg->setWindowFlag(Qt::Window);
			pixelViewDlg->show();
			pixelViewDlg->setAttribute(Qt::WA_DeleteOnClose);
			//	pixelViewDlg->setImage(m_sliceModel->rightSlice(m_rightSlider->value()));
			connect(m_rightView, &SliceWidget::sliceSelected, pixelViewDlg, &AbstractPlugin::sliceSelected);
			connect(this, &SliceEditorWidget::rightSliceOpened, pixelViewDlg, &AbstractPlugin::sliceOpened);
			emit rightSliceOpened(currentSliceIndex(SliceType::Right));
		}
		else if (m_menuWidget == m_frontView)
		{
			pixelViewDlg = new PixelWidget(SliceType::Front, QStringLiteral("Front Slice Pixel View"), m_frontView, m_sliceModel, this);
			pixelViewDlg->setWindowFlag(Qt::Window);
			pixelViewDlg->show();
			pixelViewDlg->setAttribute(Qt::WA_DeleteOnClose);
			//	pixelViewDlg->setImage(m_sliceModel->frontSlice(m_frontSlider->value()));
			connect(m_frontView, &SliceWidget::sliceSelected, pixelViewDlg, &AbstractPlugin::sliceSelected);
			connect(this, &SliceEditorWidget::frontSliceOpened, pixelViewDlg, &AbstractPlugin::sliceOpened);
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

SliceEditorWidget::SliceEditorWidget(QWidget *parent,
	bool topSliceVisible,
	bool rightSliceVisible,
	bool frontSliceVisible,
	AbstractSliceDataModel * model) :
	QWidget(parent),
	m_markModel(nullptr),
	m_sliceModel(model),
	//m_sliceTimer(nullptr),
	d_ptr(new SliceEditorWidgetPrivate)
	//m_panel(nullptr)
{
	m_layout = new QGridLayout;
	m_layout->setContentsMargins(0, 0, 0, 0);
	m_topView = new SliceWidget(this);
	m_rightView = new SliceWidget(this);
	m_frontView = new SliceWidget(this);

	m_topView->setNavigationViewEnabled(true);
	m_rightView->setNavigationViewEnabled(false);
	m_frontView->setNavigationViewEnabled(false);

	//m_sliceTimer = new QTimer(this);
	//connect(m_sliceTimer, &QTimer::timeout, this, &SliceEditorWidget::onSliceTimer);

	createWidgets();
	createToolBar();
	createContextMenu();
	createConnections();
	updateActions();

	setWindowTitle(QStringLiteral("Slice View"));
	m_layout->addWidget(m_topView, 0, 0, 1, 1, Qt::AlignCenter);
	m_layout->addWidget(m_rightView, 0, 1, 1, 1, Qt::AlignLeft);
	m_layout->addWidget(m_frontView, 1, 0, 1, 1, Qt::AlignTop);
	setLayout(m_layout);
}

void SliceEditorWidget::changeSliceHelper(int value, SliceType type)
{
	Q_ASSERT_X(m_sliceModel != nullptr, "ImageView::sliceChanged", "null pointer");
	SliceWidget * view = nullptr;
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


int SliceEditorWidget::currentIndexHelper(SliceType type)
{
	return currentSliceIndex(type);
}

MarkModel* SliceEditorWidget::createMarkModel(SliceEditorWidget *view, AbstractSliceDataModel * d)
{
	return new MarkModel(d, view,
		new TreeItem(QVector<QVariant>{QStringLiteral("Name"), QStringLiteral("Desc")}, TreeItemType::Root),
		nullptr);
}

void SliceEditorWidget::markAddedHelper(SliceType type, QGraphicsItem * mark)
{
	//Q_ASSERT_X(m_panel, "ImageCanvas::markAddedHelper", "null pointer");

	//d_ptr->state->category;
	Q_ASSERT_X(m_markModel, "SliceEditorWidget::markAddedHelper", "m_markModel != nullptr");

	const auto cate = d_ptr->state->currentCategory;
	const auto cateItem = m_markModel->categoryItem(cate);

	Q_ASSERT_X(cateItem != nullptr, "SliceEditorWidget::markAddedHelper", "cateItem != nullptr");

	const QVariant categoryColor = QVariant::fromValue<QColor>(cateItem->categoryInfo().color);


	//if (cate.isEmpty())
	//{
	//	//Add a default catetory
	//	cate = QStringLiteral("Category#%1").arg(m_panel->categoryCount());
	//	categoryColor = QVariant::fromValue(Qt::black);
	//	QPen pen = m_topView->pen();
	//	pen.setColor(Qt::black);
	//	setPen(pen);
	//	addCategoryManagerHelper(cate, categoryColor.value<QColor>());
	//}
	//auto m = QueryMarkItemInterface<AbstractMarkItem*,PolyMarkItem*>(mark);
	mark->setData(MarkProperty::SliceType, QVariant::fromValue(static_cast<int>(type)));
	mark->setData(MarkProperty::CategoryName, QVariant::fromValue(cate));
	mark->setData(MarkProperty::CategoryColor, categoryColor);
	mark->setData(MarkProperty::VisibleState, QVariant::fromValue(true));
	//slicetype, sliceindex, categoryname, name, color, categorycolor, visible state
	int index;
	QColor color;
	index = currentSliceIndex(type);
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

void SliceEditorWidget::markDeleteHelper()
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

void SliceEditorWidget::markSingleSelectionHelper()
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



SliceWidget* SliceEditorWidget::focusOn()
{
	if (m_topView->hasFocus())
		return m_topView;
	if (m_rightView->hasFocus())
		return m_rightView;
	if (m_frontView->hasFocus())
		return m_frontView;
	return nullptr;
}

bool SliceEditorWidget::topSliceVisible() const
{
	return m_topView->isHidden();
}

bool SliceEditorWidget::rightSliceVisible() const
{
	return m_rightView->isHidden();
}

bool SliceEditorWidget::frontSliceVisible() const
{
	return m_frontView->isHidden();
}

QPen SliceEditorWidget::pen() const
{
	return m_topView->pen();
}



void SliceEditorWidget::setSliceVisible(SliceType type, bool visible) {
	switch (type) {
	case SliceType::Top:
		m_topView->setVisible(visible);
		break;
	case SliceType::Right:
		m_rightView->setVisible(visible);
		break;
	case SliceType::Front:
		m_frontView->setVisible(visible);
		break;
	default:
		return;
	}
	updateActions();
	return;
}

void SliceEditorWidget::setTopSliceVisible(bool enable)
{
	setSliceVisible(SliceType::Top, enable);
}

void SliceEditorWidget::setRightSliceVisible(bool enable)
{
	setSliceVisible(SliceType::Right, enable);
}

void SliceEditorWidget::setFrontSliceVisible(bool enable)
{
	setSliceVisible(SliceType::Front, enable);
}

void SliceEditorWidget::setPen(const QPen &pen)
{
	m_topView->setPen(pen);
	m_rightView->setPen(pen);
	m_frontView->setPen(pen);
}

AbstractSliceDataModel* SliceEditorWidget::takeSliceModel(AbstractSliceDataModel* model)
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
MarkModel* SliceEditorWidget::takeMarkModel(MarkModel* model, bool * success)noexcept
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
MarkModel * SliceEditorWidget::markModel()
{
	return m_markModel;
}

SliceEditorWidget::~SliceEditorWidget()
{
	delete d_ptr;
}


int SliceEditorWidget::currentSliceIndex(SliceType type) const
{
	switch (type)
	{
	case SliceType::Top:
		return d_ptr->state->topSliceIndex;
	case SliceType::Right:
		return d_ptr->state->rightSliceIndex;
	case SliceType::Front:
		return d_ptr->state->frontSliceIndex;
	}
	return -1;

}

void SliceEditorWidget::resetZoom(bool check)
{
	m_topView->resetMatrix();
	m_rightView->resetMatrix();
	m_frontView->resetMatrix();
}

void SliceEditorWidget::zoomIn()
{
	double factor = std::pow(1.125, 1);
	m_topView->scale(factor, factor);
	m_rightView->scale(factor, factor);
	m_frontView->scale(factor, factor);
}

void SliceEditorWidget::zoomOut()
{
	double factor = std::pow(1.125, -1);
	m_topView->scale(factor, factor);
	m_rightView->scale(factor, factor);
	m_frontView->scale(factor, factor);
}

void SliceEditorWidget::setOperation(SliceType type, int opt)
{
	switch (type)
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




void SliceEditorWidget::contextMenuEvent(QContextMenuEvent* event)
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

void SliceEditorWidget::setSliceIndex(SliceType type, int index)
{
	//
	Q_ASSERT_X(m_sliceModel, "SliceEditorWidget::setSliceIndex", "null pointer");
	SliceWidget * view = nullptr;
	std::function<QImage(int)> sliceGetter;
	const MarkModel::MarkSliceList * list = nullptr;
	Q_D(SliceEditorWidget);
	switch (type)
	{
	case SliceType::Top:
	{
		if (index > m_sliceModel->topSliceCount() - 1)
			return;
		view = m_topView;
		sliceGetter = std::bind(&AbstractSliceDataModel::topSlice, m_sliceModel, std::placeholders::_1);
		if (m_markModel != nullptr)
			list = &m_markModel->topSliceVisibleMarks();
		d->state->topSliceIndex = index;
		break;
	}

	case SliceType::Right:
	{
		if (index > m_sliceModel->rightSliceCount() - 1)
			return;
		view = m_rightView;
		sliceGetter = std::bind(&AbstractSliceDataModel::rightSlice, m_sliceModel, std::placeholders::_1);
		if (m_markModel != nullptr)
			list = &m_markModel->rightSliceVisibleMarks();
		d->state->rightSliceIndex = index;
		break;
	}
	case SliceType::Front:
	{
		if (index > m_sliceModel->frontSliceCount() - 1)
			return;
		view = m_frontView;
		sliceGetter = std::bind(&AbstractSliceDataModel::frontSlice, m_sliceModel, std::placeholders::_1);
		if (m_markModel != nullptr)
			list = &m_markModel->frontSliceVisibleMarks();
		d->state->frontSliceIndex = index;
		break;
	}

	default:
		Q_ASSERT_X(false,
			"ImageView::updateSlice", "SliceType error.");
		return;
	}
	Q_ASSERT_X(static_cast<bool>(sliceGetter) == true,
		"ImageView::updateSlice", "null function");

	view->setImage(sliceGetter(index));
	view->clearSliceMarks();

	// Set Marks
	if (list != nullptr)
		view->setMarks((*list)[index]);
}

QString SliceEditorWidget::currentCategory() const
{
	return d_ptr->state->currentCategory;
}

void SliceEditorWidget::setCurrentCategory(const QString& name) {
	Q_D(SliceEditorWidget);
	d->state->currentCategory = name;
}

bool SliceEditorWidget::addCategory(const CategoryInfo& info) {
	if (m_markModel == nullptr)
		return false;
	return m_markModel->addCategory(info);
}

//bool SliceEditorWidget::addCategory(const CategoryInfo & info)const
//{
//	if (m_markModel == nullptr)
//		return false;
//	return m_markModel->addCategory(info);
//}

QStringList SliceEditorWidget::categories() const
{
	if (m_markModel == nullptr)
		return  QStringList();
	return m_markModel->categoryText();
}

void SliceEditorWidget::updateMarks(SliceType type)
{
	if (m_markModel == nullptr)
		return;

	const auto index = currentSliceIndex(type);
	switch (type)
	{
	case SliceType::Top:
	{
		const auto &lists = m_markModel->topSliceVisibleMarks();
		Q_ASSERT_X(index < lists.size(), "SliceEditorWidget::updateMarks", "top slice index out of range");
		const auto &m = m_markModel->topSliceVisibleMarks()[index];
		m_topView->setMarks(m);
	}
	break;
	case SliceType::Right:
	{
		const auto &lists = m_markModel->rightSliceVisibleMarks();
		Q_ASSERT_X(index < lists.size(), "SliceEditorWidget::updateMarks", "right slice index out of range");
		const auto &m = lists[index];
		m_rightView->setMarks(m);
	}
	break;
	case SliceType::Front:
	{
		const auto &lists = m_markModel->frontSliceVisibleMarks();
		Q_ASSERT_X(index < lists.size(), "SliceEditorWidget::updateMarks", "front slice index out of range");
		const auto &m = lists[index];
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
	QGraphicsScene::mousePressEvent(event);
}

void SliceScene::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
	QGraphicsScene::mouseMoveEvent(event);
}

void SliceScene::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
	QGraphicsScene::mouseReleaseEvent(event);
}

void SliceScene::wheelEvent(QGraphicsSceneWheelEvent * event)
{
	QGraphicsScene::wheelEvent(event);
}
