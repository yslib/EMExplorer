#include <QMenu>
#include <QContextMenuEvent>
#include <QHeaderView>
#include <QVBoxLayout>

#include "globals.h"
#include "marktreeviewwidget.h"
#include "model/treeitem.h"
#include "model/markmodel.h"
#include "widgets/colorlisteditor.h"

#include <iostream>
#include <QItemEditorFactory>
#include <qitemdelegate.h>


MarkTreeView::MarkTreeView(QWidget * parent) :QTreeView(parent)
{
	setContextMenuPolicy(Qt::DefaultContextMenu);
	setSelectionMode(QAbstractItemView::ExtendedSelection);
	createAction();
	//setShowGrid(true);
	setAlternatingRowColors(true);
	createMenu();
}

void MarkTreeView::contextMenuEvent(QContextMenuEvent* event)
{
	updateAction();
	///TODO:: need to check if the click position is propert to open a context menu
	m_menu->exec(event->globalPos());
}

/**
 * \brief Reimplemented from QTreeView::currentChanged(cosnt QModelIndex & current,const QModelIndex& preivous)
 *
 * This override function is used to emit currentIndexChanged() signal
 */

void MarkTreeView::currentChanged(const QModelIndex& current, const QModelIndex& previous) 
{
	QTreeView::currentChanged(current, previous);
	emit currentIndexChanged(current,previous);
}

void MarkTreeView::selectionChanged(const QItemSelection & selected, const QItemSelection & deselected)
{
	QTreeView::selectionChanged(selected, deselected);
	emit selectionIndexChanged(selected, deselected);
}

void MarkTreeView::createMenu()
{
	m_menu = new QMenu(QStringLiteral("MarkTreeView Context Menu"), this);
	m_menu->addAction(m_markDeleteAction);
	m_menu->addAction(m_markRenameAction);
}

void MarkTreeView::createAction()
{
	m_markDeleteAction = new QAction(QStringLiteral("Delete"), this);
	m_markRenameAction = new QAction(QStringLiteral("Rename"), this);

	connect(m_markDeleteAction, &QAction::triggered, this, &MarkTreeView::onDeleteAction);
	connect(m_markRenameAction, &QAction::triggered, this, &MarkTreeView::onRenameAction);
}




/**
 * \brief This function will be call at every time the context menu is run
 *		  First, It will check the number of the selected items in the model 
 *		  to determine whether the actions should be enabled.
 *		  1) Rename action would be enabled only if there is one selected item.
 *		  2) Delete action would be enabled if at least one item be selected.
 *		  
 */
void MarkTreeView::updateAction()
{
	m_renameItem = QModelIndex();
	m_deleteItems.clear();

	auto indexes = selectedIndexes();
	const bool deletionEnable = (indexes.count() >= 1);
	const bool renameEnable = (indexes.count() == 1);
	if(renameEnable)
		m_renameItem = indexes.first();

	QSet<TreeItem*> parents;
	//We only need to delete parent node if some marks' parent is selected.
	foreach(const auto & item,indexes)
	{
		auto d = static_cast<TreeItem*>(item.internalPointer());
		Q_ASSERT_X(d, 
			"MarkTreeView::updateAction", "null pointer");
		if(d->type() == (int)TreeItemType::Category)
		{
			parents.insert(d);
			m_deleteItems.insert(item);



		}
	}
	foreach(const auto & item,indexes)
	{
		auto d = static_cast<TreeItem*>(item.internalPointer());
		Q_ASSERT_X(d,
			"MarkTreeView::updateAction", "null pointer");
		if (d->type() == (int)TreeItemType::Root)
			continue;
		if (d->type() == (int)TreeItemType::Mark)
		{
			if (parents.find(d->parentItem()) == parents.end())
				m_deleteItems.insert(item);
		}
			
	}

	m_markDeleteAction->setEnabled(deletionEnable);
	m_markRenameAction->setEnabled(renameEnable);
}

void MarkTreeView::onDeleteAction()
{
	std::cout << "Delete Action";

	// Pre-Processing: remove items whose parent has already been the list
	foreach(const auto & index,m_deleteItems) {
		auto it = m_deleteItems.find(index.parent());
		if(it != m_deleteItems.end()) {
			m_deleteItems.erase(it);
		}
	}

	foreach(const auto & index,m_deleteItems)
	{
		const auto m = model();
		const QModelIndex parent = index.parent();
		m->removeRows(index.row(),1, parent);
	}
}

void MarkTreeView::onRenameAction()
{
	if (m_renameItem.isValid() == false)
		return;
	//TODO::
}





//In c++, functions in an anonymous namespace are equivalent to be qualified by static, which have a local scope
//namespace
//{
//	QString propertyToString(const QVariant & var, MarkProperty::Property type)
//	{
//		switch (type)
//		{
//		case MarkProperty::Color:
//		case MarkProperty::CategoryColor:
//		{
//			if (var.canConvert<QColor>() == false)
//				return QString();
//			QColor c = var.value<QColor>();
//			QString str = QStringLiteral("(%1,%2,%3)").arg(c.red()).arg(c.green()).arg(c.blue());
//			return str;
//		}
//		case MarkProperty::Name:
//		case MarkProperty::CategoryName:
//			return var.toString();
//		case MarkProperty::SliceIndex:
//			return QString::number(var.toInt());
//		case MarkProperty::SliceType:
//			switch (static_cast<SliceType>(var.toInt()))
//			{
//			case SliceType::Top:
//				return QStringLiteral("Top");
//			case SliceType::Right:
//				return QStringLiteral("Right");
//			case SliceType::Front:
//				return QStringLiteral("Front");
//			default:
//				return QStringLiteral("");
//			}
//		case MarkProperty::VisibleState:
//			return var.toBool() ? QStringLiteral("True") : QStringLiteral("False");
//		case MarkProperty::Length:
//			return QString::number(var.toDouble());
//		default:
//			return QString();
//			break;
//		}
//
//	}
//
//
//	QTableWidgetItem * tableViewItem(const QVariant & var, MarkProperty::Property type)
//	{
//		QTableWidgetItem * item = nullptr;
//		item = new QTableWidgetItem(propertyToString(var, type));
//		switch (type)
//		{
//		case MarkProperty::CategoryColor:
//		case MarkProperty::Color:
//			item->setData(Qt::BackgroundColorRole, var);
//			return item;
//		case MarkProperty::Name:
//		case MarkProperty::CategoryName:
//		case MarkProperty::SliceIndex:
//		case MarkProperty::SliceType:
//		case MarkProperty::VisibleState:
//		case MarkProperty::Length:
//			break;
//		}
//		return item;
//	}
//}


TreeNodeInfoView::TreeNodeInfoView(QWidget * parent) :QTableView(parent)
{
	horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	//verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	setShowGrid(true);
	setAlternatingRowColors(true);

	auto *factory = new QItemEditorFactory();
	auto *colorListCreator = new QStandardItemEditorCreator<ColorListEditor>();
	factory->registerEditor(QVariant::Color, colorListCreator);

	auto dele = new QItemDelegate;
	dele->setItemEditorFactory(factory);
	setItemDelegate(dele);

}




/**
 * \brief Construct a mark manager widget
 */
MarkManager::MarkManager(QWidget * parent):
QWidget(parent),
m_treeView(nullptr),
m_infoView(nullptr)
{
	m_infoView = new TreeNodeInfoView(this);
	m_treeView = new MarkTreeView(this);
	connect(m_treeView, &MarkTreeView::currentIndexChanged, this, &MarkManager::treeViewCurrentIndexChanged);
//	connect(m_treeView, &MarkTreeView::selectionIndexChanged, this, &MarkManager::treeViewSelectionIndexChanged);
	//connect(m_treeView, &MarkTreeView::clicked, this, &MarkManager::treeViewClicked);
	auto layout = new QVBoxLayout;
	layout->addWidget(m_treeView);
	layout->addWidget(m_infoView);
	layout->setStretchFactor(m_treeView, 7);
	layout->setStretchFactor(m_infoView, 3);
	setWindowTitle(QStringLiteral("Mark Manager"));
	setLayout(layout);
}

/**
 * \brief Set new model \a model to the widget
 * 
 * If the old model is the same as the new one, nothing will be done.
 * \param model 
 * 
 * \note This operation will update the selection of model of the tree view also.
 */
void MarkManager::setMarkModel(MarkModel * model)
{
	Q_ASSERT(m_treeView);
	if (m_treeView->model() == model)
		return;

	m_treeView->setModel(model);
	// update selectionModel
	m_treeView->setSelectionModel(model->selectionModelOfThisModel());
}

void MarkManager::treeViewSelectionIndexChanged(const QItemSelection & selected, const QModelIndex & deselected)
{

}

void MarkManager::treeViewClicked(const QModelIndex& index) {
	const auto item = static_cast<TreeItem *>(index.internalPointer());
	if (item != nullptr) {
		m_infoView->setModel(item->infoModel());
		if (item->type() == TreeItemType::Mark) {
			const auto m = static_cast<StrokeMarkItem*>(item->metaData());
			m->setSelected(true);
		}
	}
}

/**
 * \brief 
 */
void MarkManager::treeViewCurrentIndexChanged(const QModelIndex & current, const QModelIndex& previous)
{
	Q_UNUSED(previous);
	const auto item = static_cast<TreeItem *>(current.internalPointer());
	if (item != nullptr) {
		m_infoView->setModel(item->infoModel());
		if(item->type() == TreeItemType::Mark) {
			const auto m = static_cast<StrokeMarkItem*>(item->metaData());
			m->setSelected(true);
		}
	}
}


