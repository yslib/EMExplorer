#include "marktreeviewwidget.h"
#include <QAction>
#include <QMenu>
#include <QContextMenuEvent>
#include <QDebug>

#include "model/treeitem.h"
#include "model/markmodel.h"
#include "model/markitem.h"
#include "algorithm/triangulate.h"

#include <QGraphicsItem>
#include "globals.h"


#include <iostream>

MarkManagerWidget::MarkManagerWidget(QWidget * parent) :QTreeView(parent)
{
	setContextMenuPolicy(Qt::DefaultContextMenu);
	setSelectionMode(QAbstractItemView::ExtendedSelection);
	createAction();
	createMenu();
}

void MarkManagerWidget::contextMenuEvent(QContextMenuEvent* event)
{
	updateAction();
	///TODO:: need to check if the click position is propert to open a context menu
	m_menu->exec(event->globalPos());
}

void MarkManagerWidget::createMenu()
{
	m_menu = new QMenu(QStringLiteral("MarkTreeView Context Menu"), this);
	m_menu->addAction(m_markDeleteAction);
	m_menu->addAction(m_markRenameAction);
}

void MarkManagerWidget::createAction()
{
	m_markDeleteAction = new QAction(QStringLiteral("Delete"), this);
	m_markRenameAction = new QAction(QStringLiteral("Rename"), this);

	connect(m_markDeleteAction, &QAction::triggered, this, &MarkManagerWidget::onDeleteAction);
	connect(m_markRenameAction, &QAction::triggered, this, &MarkManagerWidget::onRenameAction);
}




/**
 * \brief This function will be call at every time the context menu is run
 *		  First, It will check the number of the selected items in the model 
 *		  to determine whether the actions should be enabled.
 *		  1) Rename action would be enabled only if there is one selected item.
 *		  2) Delete action would be enabled if at least one item be selected.
 *		  
 */
void MarkManagerWidget::updateAction()
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
		if(d->type() == TreeItemType::Category)
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
		if (d->type() == TreeItemType::Root)
			continue;
		if (d->type() == TreeItemType::Mark)
		{
			if (parents.find(d->parentItem()) == parents.end())
				m_deleteItems.insert(item);
		}
			
	}

	m_markDeleteAction->setEnabled(deletionEnable);
	m_markRenameAction->setEnabled(renameEnable);
}

void MarkManagerWidget::onDeleteAction()
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

void MarkManagerWidget::onRenameAction()
{
	if (m_renameItem.isValid() == false)
		return;
	//TODO::
}




