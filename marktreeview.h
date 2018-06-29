#ifndef MARKTREEVIEW_H
#define MARKTREEVIEW_H

#include <QTreeView>

class QAction;
class QMenu;

class MarkTreeView:public QTreeView
{
public:
    MarkTreeView(QWidget * parent = nullptr);
protected:
	void contextMenuEvent(QContextMenuEvent* event) Q_DECL_OVERRIDE;
private:
	QMenu * m_menu;
	QAction * m_markDeleteAction;
	QAction * m_markRenameAction;
	//QAction * m_markUnionAction;


	QModelIndex m_renameItem;
	QList<QModelIndex> m_deleteItems;

	void createMenu();
	void createAction();
	void updateAction();

	void onDeleteAction();
	void onRenameAction();
};

#endif // MARKTREEVIEW_H