#ifndef MARKTREEVIEW_H
#define MARKTREEVIEW_H

#include <QTreeView>
#include <QTableView>
#include "model/markmodel.h"

class MarkModel;
class QAction;
class QMenu;
class TreeItem;
class QGraphicsItem;

/**
 * \brief This class is used to view \a MarkModel
 * 
 */
class MarkTreeView:public QTreeView
{
	Q_OBJECT
public:
    MarkTreeView(QWidget * parent = nullptr);
protected:
	void contextMenuEvent(QContextMenuEvent* event) Q_DECL_OVERRIDE;
	void currentChanged(const QModelIndex& current, const QModelIndex& previous) override;
	void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected) override;
private:
	QMenu * m_menu;
	QAction * m_markDeleteAction;
	QAction * m_markRenameAction;
	QModelIndex m_renameItem;
	QSet<QModelIndex> m_deleteItems;

	void createMenu();
	void createAction();
	void updateAction();
	void onRenameAction();

signals:
	void deleteMarksActionTriggered();
	void currentIndexChanged(const QModelIndex & current,const QModelIndex & previous);
	void selectionIndexChanged(const QItemSelection & selected, const QItemSelection & deselected);
};

/**
 * \brief This is a class used to view mark and mesh information.
 */
class TreeNodeInfoView :public QTableView
{
	Q_OBJECT
public:
	TreeNodeInfoView(QWidget * parent = nullptr);
public slots:

};

/**
 * \brief This is a class used to view marks in a tree view, mark and mesh information.
 * 
 */
class MarkManager:public QWidget 
{
	Q_OBJECT
	MarkTreeView * m_treeView = nullptr;
	TreeNodeInfoView * m_infoView = nullptr;
	MarkModel * m_markModel = nullptr;
public:
	MarkManager(QWidget * parent = nullptr);
	void setMarkModel(MarkModel * model);
private slots:
	void treeViewCurrentIndexChanged(const QModelIndex & current, const QModelIndex& previous);
	void treeViewSelectionIndexChanged(const QItemSelection & selected, const QModelIndex & deselected);
	void treeViewClicked(const QModelIndex & index);
signals:
	void deleteMarksActionTriggered();
};


#endif // MARKTREEVIEW_H