#include <QToolBar>
#include <complex>
#include <QMenu>
#include <QComboBox>
#include <QMessageBox>
#include <QTimer>

#include "globals.h"
#include "abstract/abstractslicedatamodel.h"
#include "model/markmodel.h"
#include "model/categorytreeitem.h"
#include "histogramwidget.h"
#include "sliceeditorwidget.h"
#include "slicewidget.h"
#include "renderwidget.h"


/**
 * \brief This slot is called when a single mark is current in one of the tree slice widget
 * 
 * The slot notifies other objects that use the same item selection model that the mark is set current
 * 
 * \note current and selected has subtle different semantics. You could learn it from Qt Documentation.
 * 
 * \sa QItemSelectionModel
 */
void SliceEditorWidget::_slot_markSelected(StrokeMarkItem* mark) {

	//qDebug() << "SliceEditorWidget::_slot_markSelected " << " SliceEdtiroWidget should be clicked";

	const auto selectionModel = m_markModel->selectionModelOfThisModel();
	Q_ASSERT(selectionModel);
	const auto index = mark->modelIndex();
	if(index.isValid() == true) {
		selectionModel->setCurrentIndex(index, QItemSelectionModel::SelectCurrent);
	}
}

/**
 * \brief This slots is called when item is selected in other objects that use the same item selection model
 * 
 * \a current represents the newly current item after change, and the \a previous represents the old one.
 * 
 *
 * \note current and selected has subtle different semantics. You could learn it from Qt Documentation.
 *   
 */
void SliceEditorWidget::_slot_currentChanged_selectionModel(const QModelIndex & current, const QModelIndex & previous)
{

	const auto item1 = static_cast<TreeItem*>(current.internalPointer());
	if(item1 != nullptr) {
		if (item1->type() == TreeItemType::Mark) {

			//qDebug() << "SliceEditorWidget::_slot_currentChanged_selectionModel " << " Mark should be clicked from QTreeView";
			this->blockSignals(true);
			const auto mark = static_cast<StrokeMarkItem*>(item1->metaData());
			mark->setSelected(true);
			this->blockSignals(false);
		}
		else if (item1->type() == TreeItemType::Instance) {
			// Set all children of the item as selection
			//qDebug() << "SliceEditorWidget::_slot_currentChanged_selectionModel " << " Instance should be clicked from RenderWidget";
			QList<QModelIndex> markIndices;
			const auto nChild = m_markModel->rowCount(current);
			for (int i = 0; i < nChild; i++) {
				markIndices << m_markModel->index(i, 0, current);
			}

			this->blockSignals(true);
			m_topView->scene()->clearSelection();
			for (auto index : markIndices) {
				const auto item = static_cast<TreeItem*>(index.internalPointer());
				if (item->type() == TreeItemType::Mark) {
					//qDebug() << "Mark Selected";
					const auto mark = static_cast<StrokeMarkItem*>(item->metaData());
					mark->setSelected(true);
					/* This will emit selectionChange signal from SliceWidget and will invoke
					 * SliceEditorWidget::_slot_markSelected again so as to call this function recursively,
					 * so at the begining, signal is blocked first
					 */
				}
			}
			this->blockSignals(false);
		}
	}


	const auto item2 = static_cast<TreeItem*>(previous.internalPointer());
	if(item2 != nullptr) {
		if (item2->type() == TreeItemType::Mark) {
			const auto mark = static_cast<StrokeMarkItem*>(item2->metaData());
			this->blockSignals(true);
			mark->setSelected(false);
			this->blockSignals(false);
		}
		else if (item2->type() == TreeItemType::Instance) {
			// Set all children of the item as deselection
			QList<QModelIndex> markIndices;
			const auto nChild = m_markModel->rowCount(previous);
			for (int i = 0; i < nChild; i++) {
				markIndices << m_markModel->index(i, 0, previous);
			}

			this->blockSignals(true);
			for (auto index : markIndices) {
				const auto item = static_cast<TreeItem*>(index.internalPointer());
				if (item->type() == TreeItemType::Mark) {
					const auto mark = static_cast<StrokeMarkItem*>(item->metaData());
					mark->setSelected(false);
					//qDebug() << "Mark Deselected";
					/* This will emit selectionChange signal from SliceWidget and will invoke
					 * SliceEditorWidget::_slot_markSelected again so as to call this function recursively,
					 * so at the begining, signal is blocked first
					 */
				}
			}
			this->blockSignals(false);
		}
	}
	

}

/**
 * \brief This slot is not be used so far.
 */
void SliceEditorWidget::_slot_selectionChanged_selectionModel(const QItemSelection & selected, const QItemSelection & deselected)
{

}


/**
 * \brief Update states for all widgets that they are should be.
 */
void SliceEditorWidget::updateActions()
{
	const auto enable = m_sliceModel != nullptr;

	setTopSliceVisibility(enable);
	setRightSliceVisibility(enable);
	setFrontSliceVisibility(enable);
}

/**
 * \brief 
 * \param check 
 */
void SliceEditorWidget::setTopSliceVisibility(bool check)
{
	const auto enable = check && m_sliceModel != nullptr;
	m_topView->setHidden(!enable);
}

/**
 * \brief 
 * \param check 
 */
void SliceEditorWidget::setFrontSliceVisibility(bool check)
{
	const auto enable = check && m_sliceModel != nullptr;
	m_frontView->setHidden(!enable);
}

/**
 * \brief 
 * \param check 
 */
void SliceEditorWidget::setRightSliceVisibility(bool check)
{
	const auto enable = check && m_sliceModel != nullptr;
	m_rightView->setHidden(!enable);
}

/**
 * \brief 
 * \param model 
 */
void SliceEditorWidget::installMarkModel(MarkModel* model)
{
	Q_ASSERT_X(m_sliceModel,
		"ImageView::updateMarkModel", "null pointer");

	if(m_markModel != nullptr) {
		// disconnect old signals
		disconnect(m_markModel->selectionModelOfThisModel(), &QItemSelectionModel::currentChanged, this, &SliceEditorWidget::_slot_currentChanged_selectionModel);
		disconnect(m_markModel->selectionModelOfThisModel(), &QItemSelectionModel::selectionChanged, this, &SliceEditorWidget::_slot_selectionChanged_selectionModel);
	}
	m_markModel = model;

	if(m_markModel != nullptr) {
		// connect new signals 
		connect(m_markModel->selectionModelOfThisModel(), &QItemSelectionModel::currentChanged,this, &SliceEditorWidget::_slot_currentChanged_selectionModel);
		connect(m_markModel->selectionModelOfThisModel(), &QItemSelectionModel::selectionChanged,this,&SliceEditorWidget::_slot_selectionChanged_selectionModel);
	}

	updateMarks(SliceType::Top);
	updateMarks(SliceType::Right);
	updateMarks(SliceType::Front);
	updateActions();
}




/**
 * \brief 
 * \param parent 
 * \param topSliceVisible 
 * \param rightSliceVisible 
 * \param frontSliceVisible 
 * \param model 
 */
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
	m_topView->installEventFilter(this);
	m_topView->setNavigationViewEnabled(true);

	m_rightView = new SliceWidget(this);
	m_rightView->installEventFilter(this);
	m_rightView->setNavigationViewEnabled(false);

	m_frontView = new SliceWidget(this);
	m_frontView->installEventFilter(this);
	m_frontView->setNavigationViewEnabled(false);


	// Connections 
	connect(m_topView, QOverload<const QPoint &>::of(&SliceWidget::sliceSelected), this, &SliceEditorWidget::topSliceSelected);
	connect(m_rightView, QOverload<const QPoint &>::of(&SliceWidget::sliceSelected), this, &SliceEditorWidget::rightSliceSelected);
	connect(m_frontView, QOverload<const QPoint &>::of(&SliceWidget::sliceSelected), this, &SliceEditorWidget::frontSliceSelected);

	connect(m_topView, &SliceWidget::markAdded, [this](StrokeMarkItem* mark) {markAddedHelper(SliceType::Top, mark); });
	connect(m_rightView, &SliceWidget::markAdded, [this](StrokeMarkItem* mark) {markAddedHelper(SliceType::Right, mark); });
	connect(m_frontView, &SliceWidget::markAdded, [this](StrokeMarkItem* mark) {markAddedHelper(SliceType::Front, mark); });

	connect(m_topView, &SliceWidget::viewMoved, [this](const QPointF & delta) {m_rightView->translate(0.0f, delta.y()); m_frontView->translate(delta.x(), 0.0f); });

	connect(m_topView, &SliceWidget::selectionChanged, this, &SliceEditorWidget::markSingleSelectionHelper);
	connect(m_rightView, &SliceWidget::selectionChanged, this, &SliceEditorWidget::markSingleSelectionHelper);
	connect(m_frontView, &SliceWidget::selectionChanged, this, &SliceEditorWidget::markSingleSelectionHelper);

	connect(m_topView, QOverload<>::of(&SliceWidget::sliceSelected), [this]() { emit viewFocus(SliceType::Top); });
	connect(m_rightView, QOverload<>::of(&SliceWidget::sliceSelected), [this]() { emit viewFocus(SliceType::Right); });
	connect(m_frontView, QOverload<>::of(&SliceWidget::sliceSelected), [this]() { emit viewFocus(SliceType::Front); });

	connect(this, &SliceEditorWidget::markSelected, this, &SliceEditorWidget::_slot_markSelected);

	updateActions();

	setWindowTitle(QStringLiteral("Slice Editor"));
	m_layout->addWidget(m_topView, 0, 0, 1, 1, Qt::AlignCenter);
	m_layout->addWidget(m_rightView, 0, 1, 1, 1, Qt::AlignLeft);
	m_layout->addWidget(m_frontView, 1, 0, 1, 1, Qt::AlignTop);

	setLayout(m_layout);
}

/**
 * \brief Reimplemented from QWidget::eventFilter
 * 
 * Filters the mouse wheel event for the zooming in/out of slice widget
 * \note If you want to filter the event out, i.e. stop it being handled further, return \a true; otherwise return \a false.
 * 
 * \warning If you delete the receiver object in this function, be sure to return true. 
 * Otherwise, Qt will forward the event to the deleted object and the program might crash.
 * 
 */
bool SliceEditorWidget::eventFilter(QObject* watched, QEvent* event) {
	if (watched == m_topView) {
		if (event->type() == QEvent::Wheel) {
			const auto e = static_cast<QWheelEvent*>(event);
			if (e->delta() > 0)
				zoomIn();
			else
				zoomOut();
			event->accept();
			return true;
		}
	}
	else if (watched == m_rightView) {
		if (event->type() == QEvent::Wheel) {
			const auto e = static_cast<QWheelEvent*>(event);
			if (e->delta() > 0)
				zoomIn();
			else
				zoomOut();
			event->accept();
			return true;
		}

	}
	else if (watched == m_frontView) {
		if (event->type() == QEvent::Wheel) {
			const auto e = static_cast<QWheelEvent*>(event);
			if (e->delta() > 0)
				zoomIn();
			else
				zoomOut();
			event->accept();
			return true;
		}
	}
	return false;
}



/**
 * \brief 
 * \param value 
 * \param type 
 */
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


/**
 * \brief 
 * \param type 
 * \return 
 */
int SliceEditorWidget::currentIndexHelper(SliceType type) const {
	return currentSliceIndex(type);
}

/**
 * \brief Creates an instance of \a MarkModel. 
 * 
 * \param view 
 * \param d 
 * \return 
 * 
 * \sa MarkModel
 */
MarkModel* SliceEditorWidget::createMarkModel(SliceEditorWidget *view, AbstractSliceDataModel * d)
{
	return new MarkModel(d, view,
		//new TreeItem(QVector<QVariant>{QStringLiteral("Name"), QStringLiteral("Desc")}, TreeItemType::Root),
		nullptr);
}

void SliceEditorWidget::markAddedHelper(SliceType type, StrokeMarkItem* mark)
{
	//Q_ASSERT_X(m_panel, "ImageCanvas::markAddedHelper", "null pointer");

	//d_ptr->state->category;
	Q_ASSERT_X(m_markModel, "SliceEditorWidget::markAddedHelper", "m_markModel != nullptr");

	const auto cate = d_ptr->state->currentCategory;

	//const auto cateItem = m_markModel->categoryItem(cate);

	//Q_ASSERT_X(cateItem != nullptr, "SliceEditorWidget::markAddedHelper", "cateItem != nullptr");

	const QVariant categoryColor = QVariant::fromValue<QColor>(Qt::blue);


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

void SliceEditorWidget::markDeleteHelper(SliceType type, StrokeMarkItem * mark)
{

}

void SliceEditorWidget::deleteSelectedMarks()
{
	Q_ASSERT_X(m_markModel, "ImageView::markDeleteHelper", "null pointer");
	QList<StrokeMarkItem*> items;

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
	const auto count = m_topView->selectedItemCount() + m_rightView->selectedItemCount() + m_frontView->selectedItemCount();
	if (count != 1)
		return;
	StrokeMarkItem * item = nullptr;
	if (m_topView->selectedItemCount() == 1)
		item = m_topView->selectedItems()[0];
	else if (m_rightView->selectedItemCount() == 1)
		item = m_rightView->selectedItems()[0];
	else if (m_frontView->selectedItemCount() == 1)
		item = m_frontView->selectedItems()[0];
	emit markSelected(item);
}

/**
 * \brief  Returns the slice widget which currently has a focus, otherwise return \a nullptr
 */
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

/**
 * \brief Returns the visibility of top slice.
 * 
 * Returns \a true if it's visible, otherwise returns \a false
 */
bool SliceEditorWidget::topSliceVisible() const
{
	return m_topView->isHidden();
}

/**
 * \brief Returns the visibility of right slice.
 *
 * Returns \a true if it's visible, otherwise returns \a false
 */
bool SliceEditorWidget::rightSliceVisible() const
{
	return m_rightView->isHidden();
}

/**
 * \brief Returns the visibility of front slice.
 *
 * Returns \a true if it's visible, otherwise returns \a false
 */
bool SliceEditorWidget::frontSliceVisible() const
{
	return m_frontView->isHidden();
}

/**
 * \brief Returns a \a QPen object used for painting marks
 */
QPen SliceEditorWidget::pen() const
{
	return m_topView->pen();
}

/**
 * \brief This property holds visibility of the three types of slice widget.
 * \param type Indicates the slice type.
 * \param visible Indicates visibility of the slice widget.
 */
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

/** 
 * \brief This a convienience function to change the visibility of the top slice widget

 * \param enable Indicates visibility of the slice widget.
 * 
 * \sa SliceEditorWidget::setSliceVisible(SliceType type, bool visible)
 */
void SliceEditorWidget::setTopSliceVisible(bool enable)
{
	setSliceVisible(SliceType::Top, enable);
}

/**
 * \brief This a convienience function to change the visibility of the front slice widget
 * 
 * \param enable Indicates visibility of the slice widget.
 * \sa SliceEditorWidget::setSliceVisible(SliceType type, bool visible)
 */
void SliceEditorWidget::setRightSliceVisible(bool enable)
{
	setSliceVisible(SliceType::Right, enable);
}

/**
 * \brief This a convienience function to change the visibility of the right slice widget
 * 
 * \param enable Indicates visibility of the slice widget.
 * \sa SliceEditorWidget::setSliceVisible(SliceType type, bool visible)
 */
void SliceEditorWidget::setFrontSliceVisible(bool enable)
{
	setSliceVisible(SliceType::Front, enable);
}

/**
 * \brief Sets the slice widgets' pen to be the given \a pen 
 * 
 * The \a pen used for painting marks
 */

void SliceEditorWidget::setPen(const QPen &pen)
{
	m_topView->setPen(pen);
	m_rightView->setPen(pen);
	m_frontView->setPen(pen);
}

/**
 * \brief 
 * \param model 
 * \return 
 */
AbstractSliceDataModel* SliceEditorWidget::takeSliceModel(AbstractSliceDataModel* model)
{
	const auto t = m_sliceModel;

	m_sliceModel = model;
	delete m_markModel;

	installMarkModel(createMarkModel(this, m_sliceModel));

	setSliceIndex(SliceType::Front, 0);
	setSliceIndex(SliceType::Right, 0);
	setSliceIndex(SliceType::Top, 0);

	updateActions();
	emit dataModelChanged();
	return t;
}
/**
 * \brief 
 * \param model 
 * \param success 
 * \return 
 */
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
		auto t = m_markModel;
		installMarkModel(nullptr);
		if (success != nullptr)
			*success = true;
		emit markModelChanged();
		return t;
	}

	if (model->checkMatchHelper(m_sliceModel) == false)		// Check whether the new mark model match the current data model
	{
		if (success != nullptr)
			*success = false;
		return nullptr;
	}

	const auto t = m_markModel;		//old 
	installMarkModel(model);
	if (success != nullptr)
		*success = true;
	emit markModelChanged();
	return t;
}

/**
 * \brief Returns the mark model held in the widget.
 */
MarkModel * SliceEditorWidget::markModel()
{
	return m_markModel;
}

/**
 * \brief Destroys the widget
 */
SliceEditorWidget::~SliceEditorWidget()
{
	delete d_ptr;
	if (m_markModel != nullptr)
		delete m_markModel;
}


/**
 * \brief Returns the slice index of \a type type slice
 */
int SliceEditorWidget::currentSliceIndex(SliceType type) const
{
	switch (type)
	{
	case SliceType::Top:
		return d_ptr->state->currentTopSliceIndex;
	case SliceType::Right:
		return d_ptr->state->currentRightSliceIndex;
	case SliceType::Front:
		return d_ptr->state->currentFrontSliceIndex;
	}
	return -1;

}

/**
 * \brief Resets the default zoom for three slice widgets 
 */
void SliceEditorWidget::resetZoom(bool check)
{
	m_topView->resetMatrix();
	m_rightView->resetMatrix();
	m_frontView->resetMatrix();
}

/**
 * \brief Zooms in for three slice widgets
 */
void SliceEditorWidget::zoomIn()
{
	const auto factor = std::pow(1.125, 1);
	m_topView->scale(factor, factor);
	m_rightView->scale(factor, factor);
	m_frontView->scale(factor, factor);
}

/**
 * \brief Zooms out for three slice widgets
 */
void SliceEditorWidget::zoomOut()
{
	const auto factor = std::pow(1.125, -1);
	m_topView->scale(factor, factor);
	m_rightView->scale(factor, factor);
	m_frontView->scale(factor, factor);
}

/**
 * \brief Sets the given \a opt operation for the \a type type slice.
 * 
 * The operation includes mark drawing, mark selection and slice movement.
 * 
 * \sa SliceType Operation
 * 
 */
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

/**
 * \brief Sets the current slice index given in \a index for the slice type \a type
 * 
 * \sa SliceType
 */
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
		d->state->currentTopSliceIndex = index;
		emit topSliceChanged(index);
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
		d->state->currentRightSliceIndex = index;
		emit rightSliceChanged(index);
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
		d->state->currentFrontSliceIndex = index;
		emit frontSliceChanged(index);
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

	view->clearSliceMarks();		// clear previous marks

	// Set Marks
	if (list != nullptr)
		view->setMarks((*list)[index]);
}

/**
 * \brief Returns the current category
 */
QString SliceEditorWidget::currentCategory() const
{
	return d_ptr->state->currentCategory;
}

/**
 * \brief This property holds the current category that the mark will be painted on.
 *  
 */
void SliceEditorWidget::setCurrentCategory(const QString& name) {
	Q_D(SliceEditorWidget);
	d->state->currentCategory = name;
}

/**
 * \brief Adds a new category into the mark model in the \a SliceEditorWidget
 * 
 * Returns \true if add is successfully, otherwise return \a false
 * 
 * \sa CategoryInfo
 */
bool SliceEditorWidget::addCategory(const CategoryInfo& info) const {
	if (m_markModel == nullptr)
		return false;
	return m_markModel->addCategory(info);
}

/**
 * \brief Returns the categories that the current mark model holds
 */
QStringList SliceEditorWidget::categories() const
{
	if (m_markModel == nullptr)
		return  QStringList();
	return m_markModel->categoryText();
}

/**
 * \brief Update marks on \a type type of slice.
 * 
 * This is often used when new slice data model or new mark model is set.
 * 
 * \sa SliceType
 */
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
