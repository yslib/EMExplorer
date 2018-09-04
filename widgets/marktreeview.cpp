#include "marktreeview.h"
#include <QAction>
#include <QMenu>
#include <QContextMenuEvent>
#include <QDebug>
#include "model/treeitem.h"

MarkTreeView::MarkTreeView(QWidget * parent) :QTreeView(parent)
{
	setContextMenuPolicy(Qt::DefaultContextMenu);
	setSelectionMode(QAbstractItemView::ExtendedSelection);
	createAction();
	createMenu();
}

void MarkTreeView::contextMenuEvent(QContextMenuEvent* event)
{
	updateAction();
	///TODO:: need to check if the click position is propert to open a context menu
	m_menu->exec(event->globalPos());
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
		if(d->type() == TreeItemType::Category)
		{
			parents.insert(d);
			m_deleteItems.append(item);
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
				m_deleteItems.append(item);
		}
			
	}
	m_markDeleteAction->setEnabled(deletionEnable);
	m_markRenameAction->setEnabled(renameEnable);
}

void MarkTreeView::onDeleteAction()
{
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

}


