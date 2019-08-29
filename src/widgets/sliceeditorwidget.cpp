#include <QToolBar>
#include <complex>
#include <QMenu>
#include <QComboBox>
#include <QMessageBox>
#include <QTimer>
#include <QCheckBox>
#include <QToolButton>
#include <QUndoCommand>

#include "globals.h"
#include "abstract/abstractslicedatamodel.h"
#include "model/markmodel.h"
#include "model/categorytreeitem.h"
#include "model/marktreeitem.h"
#include "model/commands.h"
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
 *  This is only for class internal use
 *
 * \sa QItemSelectionModel
 */

void SliceEditorWidget::onCurrentMarkChanged(StrokeMarkItem* current)
{
	const auto selectionModel = m_markModel->selectionModel();
	Q_ASSERT(selectionModel);
	const auto currentIndex = current->modelIndex();
	if (currentIndex.isValid())
	{
		const auto b = this->signalsBlocked();
		this->blockSignals(false);
		selectionModel->setCurrentIndex(currentIndex, QItemSelectionModel::SelectCurrent);
		this->blockSignals(b);
	}
}

/**
 * \brief  This slot is called when seleteced marks have been changed in one of the tree slice widget.
 */
void SliceEditorWidget::onSelectedMarksChanged(const QList<StrokeMarkItem*> & selected)
{
	const auto selectionModel = m_markModel->selectionModel();
	Q_ASSERT(selectionModel);
	QItemSelection sel;
	for (auto item : selected)
	{
		const auto index = item->modelIndex();
		if (index.isValid())
		{
			sel.push_back(QItemSelectionRange(index));
		}
	}
	const auto b = this->signalsBlocked();
	// Blocking the slot invoked by the select operation 
	this->blockSignals(false);
	selectionModel->reset();
	selectionModel->select(sel, QItemSelectionModel::Select);
	this->blockSignals(b);
}

/**
 * \brief This slots is called when item is selected in other objects that use the same item selection model
 *
 * \a current represents the newly current item after change, and the \a previous represents the old one.

 * \note current and selected has subtle different semantics. You could learn it from Qt Documentation.
 * This is only for class internal use
 *
 */
void SliceEditorWidget::onExternalCurrentMarkChanged(const QModelIndex & current, const QModelIndex & previous)
{

	const auto item1 = static_cast<TreeItem*>(current.internalPointer());
	if (item1 != nullptr) {
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
	if (item2 != nullptr) {
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
					/* This will emit selectionChange signal from SliceWidget and will invoke
					 * SliceEditorWidget::_slot_markSelected again so as to call this function recursively,
					 * so at the beginning, signal is blocked first
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
void SliceEditorWidget::onExternalSelectedMarksChanged(const QItemSelection & selected, const QItemSelection & deselected)
{
	// We don't do any reactions if multi marks are selected in other widgets
}

void SliceEditorWidget::_slot_topViewSliceSelection(const QPoint& pos)
{
	if (m_sliceModel == nullptr)
		return;
	const auto rightSliceCount = m_sliceModel->rightSliceCount();
	const auto frontSliceCount = m_sliceModel->frontSliceCount();
	if (pos.x() >= 0 && pos.x() <= rightSliceCount)
	{
		setSliceIndex(SliceType::Right, pos.x());
	}
	if (pos.y() >= 0 && pos.y() <= frontSliceCount)
	{
		setSliceIndex(SliceType::Front, pos.y());

	}
	//display selected coordinates
}

void SliceEditorWidget::_slot_rightViewSliceSelection(const QPoint & pos)
{
	if (m_sliceModel == nullptr)
		return;
	const auto topSliceCount = m_sliceModel->topSliceCount();
	const auto frontSliceCount = m_sliceModel->frontSliceCount();
	if (pos.x() >= 0 && pos.x() <= topSliceCount)
	{
		setSliceIndex(SliceType::Top, pos.x());
	}
	if (pos.y() >= 0 && pos.y() <= frontSliceCount)
	{
		setSliceIndex(SliceType::Front, pos.y());

	}
}

void SliceEditorWidget::_slot_frontViewSliceSelection(const QPoint& pos)
{
	if (m_sliceModel == nullptr)
		return;
	const auto rightSliceCount = m_sliceModel->rightSliceCount();
	const auto topSliceCount = m_sliceModel->topSliceCount();
	if (pos.x() >= 0 && pos.x() <= rightSliceCount)
	{
		setSliceIndex(SliceType::Right, pos.x());
	}
	if (pos.y() >= 0 && pos.y() <= topSliceCount)
	{
		setSliceIndex(SliceType::Top, pos.y());

    }
}

void SliceEditorWidget::addMark(SliceType type, QUndoCommand * addMarkCommand)
{
    static_cast<AddMarkCommand *>(addMarkCommand)->setType(type);
    static_cast<AddMarkCommand *>(addMarkCommand)->setWidget(this);
    m_undoStack->push(addMarkCommand);
}

/**
 * \brief Update states for all widgets that they are should be.
 *
 * \note This is only for class internal use
 */
void SliceEditorWidget::updateActions()
{
	const auto enable = m_sliceModel != nullptr;

	setTopSliceVisibility(enable);
	setRightSliceVisibility(enable);
	setFrontSliceVisibility(enable);

	m_toolButton->setVisible(enable);
}

/**
 * \brief This is a convenience function to set top slice visibility
 */
void SliceEditorWidget::setTopSliceVisibility(bool check)
{
	const auto enable = check && m_sliceModel != nullptr;
	m_topView->setHidden(!enable);
}

/**
 * \brief This is a convenience function to set front slice visibility
 */
void SliceEditorWidget::setFrontSliceVisibility(bool check)
{
	const auto enable = check && m_sliceModel != nullptr;
	m_frontView->setHidden(!enable);
}

/**
 * \brief This is a convenience function to set right slice visibility
 */
void SliceEditorWidget::setRightSliceVisibility(bool check)
{
	const auto enable = check && m_sliceModel != nullptr;
	m_rightView->setHidden(!enable);
}

/**
 * \brief This function is used to set the given \a model as the mark model
 *
 * \sa MarkModel
 * \note This is only for class internal use
 */
void SliceEditorWidget::installMarkModel(MarkModel* model)
{
	Q_ASSERT_X(m_sliceModel,
		"ImageView::updateMarkModel", "null pointer");
	if (m_markModel == model)
		return;

	if (m_markModel != nullptr)
	{
		// disconnect old signals
		disconnect(m_markModel->selectionModel(), &QItemSelectionModel::currentChanged, this, &SliceEditorWidget::onExternalCurrentMarkChanged);
		disconnect(m_markModel->selectionModel(), &QItemSelectionModel::selectionChanged, this, &SliceEditorWidget::onExternalSelectedMarksChanged);
	}
	m_markModel->deleteLater();
	m_markModel = model;


	if (m_markModel != nullptr) {
		// connect new signals 
		connect(m_markModel->selectionModel(), &QItemSelectionModel::currentChanged, this, &SliceEditorWidget::onExternalCurrentMarkChanged);
		connect(m_markModel->selectionModel(), &QItemSelectionModel::selectionChanged, this, &SliceEditorWidget::onExternalSelectedMarksChanged);
	}

	updateMarks(SliceType::Top);
	updateMarks(SliceType::Right);
	updateMarks(SliceType::Front);
	updateActions();

	emit markModelChanged();
}

/**
 * \brief Constructs an instance of \a SliceEditorWidget
 *
 * \param parent The parent widget pointer
 * \param topSliceVisible The visibility of the top slice widget
 * \param rightSliceVisible The visibility of the right slice widget
 * \param frontSliceVisible The visibility of the front slice widget
 * \param model Slice data model that is used to display
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
    m_undoStack = new QUndoStack(this);
	m_layout = new QGridLayout;
	//m_layout->setContentsMargins(0, 0, 0, 0);

	m_topView = new SliceWidget(this);
	m_topView->installEventFilter(this);
	m_topView->setNavigationViewEnabled(true);

	m_rightView = new SliceWidget(this);
	m_rightView->installEventFilter(this);
	m_rightView->setNavigationViewEnabled(false);

	m_frontView = new SliceWidget(this);
	m_frontView->installEventFilter(this);
	m_frontView->setNavigationViewEnabled(false);


	m_toolButton = new QToolButton(this);
	m_toolButton->setText(QStringLiteral("Advanced"));
	m_toolButton->setPopupMode(QToolButton::InstantPopup);
	auto menu = new QMenu(QStringLiteral("Slice Editor"), this);
	m_toolButton->setMenu(menu);
	QAction * hideAction = new QAction(QStringLiteral("Hide Navigation View"));
	hideAction->setCheckable(true);
	hideAction->setChecked(m_topView->navigationViewEnabled());
	connect(hideAction, &QAction::triggered, [this](bool checked) {m_topView->setNavigationViewEnabled(checked); });
	menu->addAction(hideAction);

	// Connections

	// slice selection signals
	connect(m_topView, QOverload<const QPoint &>::of(&SliceWidget::sliceSelected), this, &SliceEditorWidget::topSliceSelected);
	connect(m_rightView, QOverload<const QPoint &>::of(&SliceWidget::sliceSelected), this, &SliceEditorWidget::rightSliceSelected);
	connect(m_frontView, QOverload<const QPoint &>::of(&SliceWidget::sliceSelected), this, &SliceEditorWidget::frontSliceSelected);

	// Mark added signals
    connect(m_topView, &SliceWidget::markAdded, [this](QUndoCommand* mark) {addMark(SliceType::Top, mark); });
    connect(m_rightView, &SliceWidget::markAdded, [this](QUndoCommand* mark) {addMark(SliceType::Right, mark); });
    connect(m_frontView, &SliceWidget::markAdded, [this](QUndoCommand* mark) {addMark(SliceType::Front, mark); });

	connect(m_topView, &SliceWidget::viewMoved, [this](const QPointF & delta) {m_rightView->translate(0.0f, delta.y()); m_frontView->translate(delta.x(), 0.0f); });

	connect(m_topView, &SliceWidget::selectionChanged, this, &SliceEditorWidget::markSelectionChangedHandler);
	connect(m_rightView, &SliceWidget::selectionChanged, this, &SliceEditorWidget::markSelectionChangedHandler);
	connect(m_frontView, &SliceWidget::selectionChanged, this, &SliceEditorWidget::markSelectionChangedHandler);



	connect(m_topView, QOverload<>::of(&SliceWidget::sliceSelected), [this]() { emit viewFocus(SliceType::Top); });
	connect(m_rightView, QOverload<>::of(&SliceWidget::sliceSelected), [this]() { emit viewFocus(SliceType::Right); });
	connect(m_frontView, QOverload<>::of(&SliceWidget::sliceSelected), [this]() { emit viewFocus(SliceType::Front); });

	connect(m_topView, QOverload<const QPoint &>::of(&SliceWidget::sliceSelected), this, &SliceEditorWidget::_slot_topViewSliceSelection);
	connect(m_rightView, QOverload<const QPoint &>::of(&SliceWidget::sliceSelected), this, &SliceEditorWidget::_slot_rightViewSliceSelection);
	connect(m_frontView, QOverload<const QPoint &>::of(&SliceWidget::sliceSelected), this, &SliceEditorWidget::_slot_frontViewSliceSelection);

	//connect(this, &SliceEditorWidget::currentMarkChanged, this, &SliceEditorWidget::onCurrentMarkChanged);


	updateActions();
	setWindowTitle(QStringLiteral("Slice Editor"));
	m_layout->addWidget(m_topView, 0, 0, 1, 1);
	m_layout->addWidget(m_rightView, 0, 1, 1, 1, Qt::AlignLeft);
	m_layout->addWidget(m_frontView, 1, 0, 1, 1, Qt::AlignTop);
	m_layout->addWidget(m_toolButton, 1, 1, 1, 1, Qt::AlignCenter);
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
 * \brief This is helper function that change the index of a given \a type type slice with the new given \a index
 *
 * A lot of works need to be done for a slice change. First, the image must be retrieved from slice data model.
 * And then, marks in the previous slice must be cleared
 *
 * \sa SliceType
 */
 //void SliceEditorWidget::changeSliceHelper(int value, SliceType type)
 //{
 //	Q_ASSERT_X(m_sliceModel != nullptr, "ImageView::sliceChanged", "null pointer");
 //	SliceWidget * view = nullptr;
 //	std::function<QImage(int)> sliceGetter;
 //	//TODO:: 
 //	switch (type)
 //	{
 //	case SliceType::Top:
 //		view = m_topView;
 //		sliceGetter = std::bind(&AbstractSliceDataModel::frontSlice, m_sliceModel, std::placeholders::_1);
 //		break;
 //	case SliceType::Right:
 //		view = m_rightView;
 //		sliceGetter = std::bind(&AbstractSliceDataModel::rightSlice, m_sliceModel, std::placeholders::_1);
 //		break;
 //	case SliceType::Front:
 //		view = m_frontView;
 //		sliceGetter = std::bind(&AbstractSliceDataModel::frontSlice, m_sliceModel, std::placeholders::_1);
 //		break;
 //	default:
 //		Q_ASSERT_X(false, "ImageView::updateSlice", "SliceType error.");
 //	}
 //	Q_ASSERT_X(static_cast<bool>(sliceGetter) == true, "ImageView::updateSlice", "invalid bind");
 //	view->setImage(sliceGetter(value));
 //	view->clearSliceMarks();
 //}



 /**
  * \brief Creates an instance of \a MarkModel by a given \a SliceEditorWidget \a view and a \a AbstractSliceDataModel \a data
  *
  * \sa MarkModel SliceEditorWidget AbstractSliceDataModel
  *
  * \note This is only for class internal use
  */
MarkModel* SliceEditorWidget::createMarkModel(SliceEditorWidget *view, AbstractSliceDataModel * data)
{
    return new MarkModel(data, view, nullptr);
}

/**
*	\brief
*	\param category
*	\return return a \a QModelIndex represents the \a category
*
*	\internal
*	\note This can be implement by a hash table, which is more efficient.
*/
QModelIndex SliceEditorWidget::_hlp_categoryIndex(const QString & category) const
{
    int c = m_markModel->rowCount();	//children number of root. It's category
	for (int i = 0; i < c; i++)
	{
		auto id = m_markModel->index(i, 0);
		const auto item = static_cast<TreeItem*>(id.internalPointer());

		if (item != nullptr && item->type() != TreeItemType::Category)
			continue;

		//auto d = item->data(0).value<__Internal_Categroy_Type_>();
		const auto var = item->data(0, Qt::DisplayRole);
		Q_ASSERT_X(var.canConvert<QString>(), "MarkModel::categoryIndexHelper", "convert failed");
		if (var == category)
		{
			return id;
		}
	}
	return QModelIndex();
}

void SliceEditorWidget::_hlp_categoryAdd(const CategoryInfo & info) const {

    int row = m_markModel->rowCount();

    if (!m_markModel->insertRows(row, 1, QModelIndex()))
        return;

    const auto newIndex = m_markModel->index(row, 0, QModelIndex());
	const auto p = new CategoryTreeItem(CategoryItem(info.name, info.color), newIndex, m_markModel->rootItem());
    m_markModel->setData(newIndex, QVariant::fromValue(static_cast<void*>(p)), MarkModel::TreeItemRole);
}

QModelIndex SliceEditorWidget::_hlp_instanceFind(const QString & category, const StrokeMarkItem * item)
{
	auto cIndex = _hlp_categoryIndex(category);
    Q_ASSERT_X(cIndex.isValid(), "MarkModel::instanceAddHelper", "index is invalid");

	const auto nChild = m_markModel->rowCount(cIndex);
	const auto itemRect = item->boundingRect();

	QModelIndex best;
	double maxArea = 0;
	for (auto i = 0; i < nChild; i++) {
		const auto iIndex = m_markModel->index(i, 0, cIndex);
		const auto item = m_markModel->treeItem(iIndex);
		if (item->type() == TreeItemType::Instance) {
			const auto instanceItem = static_cast<InstanceTreeItem*>(item);
			const auto rect = instanceItem->boundingBox();

			const auto intersected = rect.intersected(itemRect);
			const auto area = intersected.width()*intersected.height();
			if (maxArea < area) {
				best = iIndex;
				maxArea = area;
			}
		}
	}
	if (best.isValid() == false)
	{
		return QModelIndex();
	}
	return best;
}

QModelIndex SliceEditorWidget::_hlp_instanceAdd(const QString & category, const StrokeMarkItem * mark)
{
	auto cIndex = _hlp_categoryIndex(category);
	Q_ASSERT_X(cIndex.isValid(), "MarkModel::instanceFindHelper", "invalid index");

	const auto c = m_markModel->rowCount(cIndex);

	// Insert a new row
	const auto success = m_markModel->insertRows(c, 1, cIndex);

    if (!success)
		return QModelIndex();

	// Fetch the new inserted index
	const auto newIndex = m_markModel->MarkModel::index(c, 0, cIndex);
	// Create a tree item pointer
	InstanceMetaData * metaData = new InstanceMetaData;			// Bull Shit designed

	metaData->setName(QStringLiteral("Instance"));

	metaData->setColor(mark->pen().color());

	const auto p = new InstanceTreeItem(metaData, newIndex, nullptr);
	p->setBoundingBox(mark->boundingRect().toRect());

	m_markModel->MarkModel::setData(newIndex, QVariant::fromValue(static_cast<void*>(p)), MarkModel::TreeItemRole);
	return m_markModel->MarkModel::index(c, 0, cIndex);
}

QStringList SliceEditorWidget::categoryText() const
{
	if (m_markModel == nullptr)
		return  QStringList{};

	QVector<QVariant> data;
	QStringList list;
	m_markModel->retrieveData(m_markModel->rootItem(), TreeItemType::Category, 0, data, Qt::DisplayRole);
	foreach(const auto & var, data)
	{
		Q_ASSERT_X(var.canConvert<QString>(), "MarkModel::categoryText", "convert falied");
		list << var.toString();
	}
	return list;
}

bool SliceEditorWidget::removeMark(StrokeMarkItem * mark)
{
	const auto parent = m_markModel->parent(mark->modelIndex());
	m_markModel->removeRows(mark->modelIndex().row(), 1, parent);

	/// TODO:: 
	//delete mark;

	return true;
}

int SliceEditorWidget::removeMarks(const QList<StrokeMarkItem*>& marks)
{
	auto success = 0;
	for (auto item : marks)
		if (removeMark(item))
			success++;
	return success;
}

/**
 * \brief This is a helper function used to add mark \a mark to a \a type type slice
 *
 * Some information about the mark should be added into it including \a type of SliceType and \a index
 *
 * \note This is only for class internal use
 */
void SliceEditorWidget::markAddedHelper(SliceType type, StrokeMarkItem* mark)
{
	Q_ASSERT_X(m_markModel, "SliceEditorWidget::markAddedHelper", "m_markModel != nullptr");
	const auto cate = d_ptr->state->currentCategory;
	const auto index = currentSliceIndex(type);
	mark->setSliceType(type);
	mark->setSliceIndex(index);

    auto instance = _hlp_instanceFind(cate, mark);
    if (!instance.isValid())
	{
        instance = _hlp_instanceAdd(cate, mark);
	}
    int row = m_markModel->rowCount(instance);
	// Insert rows
    m_markModel->insertRows(row, 1, instance);

    // Get index of new inserted rows then set data
    const auto newIndex = m_markModel->MarkModel::index(row, 0, instance);
	const auto p = new StrokeMarkTreeItem(mark, newIndex, nullptr);
    m_markModel->setData(newIndex, QVariant::fromValue(static_cast<void*>(p)), MarkModel::TreeItemRole);
}

void SliceEditorWidget::removeItem(SliceType type, StrokeMarkItem *mark)
{
    Q_ASSERT_X(m_markModel, "SliceEditorWidget::markAddedHelper", "m_markModel != nullptr");
    m_markModel->selectionModel()->reset();
    m_markModel->selectionModel()->select(mark->modelIndex(), QItemSelectionModel::Select);
    m_markModel->removeSelectedItems();
    switch (type)
    {
        case SliceType::Top:
            m_topView->scene()->removeItem(mark);
            break;
        case SliceType::Right:
            m_topView->scene()->removeItem(mark);
            break;
        case SliceType::Front:
            m_topView->scene()->removeItem(mark);
            break;
    }
}

void SliceEditorWidget::addItem(SliceType type, StrokeMarkItem *mark)
{
    switch (type)
    {
        case SliceType::Top:
            m_topView->scene()->addItem(mark);
            break;
        case SliceType::Right:
            m_topView->scene()->addItem(mark);
            break;
        case SliceType::Front:
            m_topView->scene()->addItem(mark);
            break;
    }
}


/**
 * \brief This is a slot
 *
 *  This slot is called when selectionChanged signal of one of three slice widget emits.
 *  It will emits the \sa markSelected(StrokeMarkItem * mark) signal to forward only one mark
 *
 *  \sa markSelected(StrokeMarkItem * mark)
 */
void SliceEditorWidget::markSelectionChangedHandler()
{
	const auto count = m_topView->selectedItemCount() + m_rightView->selectedItemCount() + m_frontView->selectedItemCount();
	if (count == 1)
	{
		StrokeMarkItem * item = nullptr;
		if (m_topView->selectedItemCount() == 1)
			item = m_topView->selectedItems()[0];
		else if (m_rightView->selectedItemCount() == 1)
			item = m_rightView->selectedItems()[0];
		else if (m_frontView->selectedItemCount() == 1)
			item = m_frontView->selectedItems()[0];
		emit currentMarkChanged(item);
		onCurrentMarkChanged(item);
	}

	QList<StrokeMarkItem*> sel;
	sel.append(m_topView->selectedItems());
	sel.append(m_frontView->selectedItems());
	sel.append(m_rightView->selectedItems());
	emit selectedMarksChanged(sel);
	onSelectedMarksChanged(sel);
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
 * \brief Replaces the old slice model with the given \a model and returns the old one.
 *
 * The mark model corresponding to the slice model will be deleted once the old slice model is replaced, and a new mark model will be created.
 * So it's caller's responsibility to save the corresponding mark model before the replacement takes place, and
 * the old slice model and its ownership will be returned to the caller. Sets \a model as \a nullptr is equivalent to empty the slice data.
 *
 * \note This function also emits dataModelChanged() signal
 *
 */

AbstractSliceDataModel* SliceEditorWidget::takeSliceModel(AbstractSliceDataModel* model)
{
	const auto t = m_sliceModel;
	m_sliceModel = model;

	setSliceIndex(SliceType::Front, 0);
	setSliceIndex(SliceType::Right, 0);
	setSliceIndex(SliceType::Top, 0);

	updateActions();
	emit dataModelChanged();
	installMarkModel(createMarkModel(this, m_sliceModel));
	resetZoom(true);
	return t;
}

/**
 * \brief Replaces the old mark model with the given \a model and returns the old one.
 *	A flag \a success refers to is set as \a true when replacement is
 *	successful if it's not \a nullptr, otherwise it's set as \a false
 *
 * A check would be applied for the given \a model in case that the mark model \a model
 * matches data model. If it don't match or the data model is empty, a flag \a success
 * refers to will be set as \a false and \a nullptr will be returned. Above two situations
 * won't emit markModelChanged() signals. Others will emits markModelChanged() signal.
 * \a model set as \a nullptr is equivalent to delete mark model and will emit markModelChanged() signal
 *
 * \note This function also emits markModelChanged() signal
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
	m_topView->setDefaultZoom();
	m_rightView->setDefaultZoom();
	m_frontView->setDefaultZoom();
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
            list = &m_markModel->m_topSliceVisibleMarks;
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
            list = &m_markModel->m_rightSliceVisibleMarks;
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
            list = &m_markModel->m_frontSliceVisibleMarks;
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
void SliceEditorWidget::setCurrentCategory(const QString& name)
{
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
bool SliceEditorWidget::addCategory(const CategoryInfo & info) const {
	if (m_markModel == nullptr)
		return false;

	auto i = _hlp_categoryIndex(info.name);
	if (i.isValid() == false)
	{
		_hlp_categoryAdd(info);
		return true;
	}
	return false;
}

/**
 * \brief Returns the categories that the current mark model holds
 */
QStringList SliceEditorWidget::categories() const
{
	return categoryText();
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
        const auto &lists = m_markModel->m_topSliceVisibleMarks;
		Q_ASSERT_X(index < lists.size(), "SliceEditorWidget::updateMarks", "top slice index out of range");
        const auto &m = m_markModel->m_topSliceVisibleMarks[index];
		m_topView->setMarks(m);
	}
	break;
	case SliceType::Right:
	{
        const auto &lists = m_markModel->m_rightSliceVisibleMarks;
		Q_ASSERT_X(index < lists.size(), "SliceEditorWidget::updateMarks", "right slice index out of range");
		const auto &m = lists[index];
		m_rightView->setMarks(m);
	}
	break;
	case SliceType::Front:
	{
        const auto &lists = m_markModel->m_frontSliceVisibleMarks;
		Q_ASSERT_X(index < lists.size(), "SliceEditorWidget::updateMarks", "front slice index out of range");
		const auto &m = lists[index];
		m_frontView->setMarks(m);
	}
	break;
	}
}
