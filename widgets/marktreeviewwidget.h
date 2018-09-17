#ifndef MARKTREEVIEW_H
#define MARKTREEVIEW_H

#include <QTreeView>

class QAction;
class QMenu;
class TreeItem;

class MarkManagerWidget:public QTreeView
{
public:
    MarkManagerWidget(QWidget * parent = nullptr);
	QSize minimumSizeHint() const Q_DECL_OVERRIDE { return QSize(250, 250); }
	QSize sizeHint() const Q_DECL_OVERRIDE { return QSize(250, 250); }
protected:
	void contextMenuEvent(QContextMenuEvent* event) Q_DECL_OVERRIDE;
private:
	QMenu * m_menu;
	QAction * m_markDeleteAction;
	QAction * m_markRenameAction;
	//QAction * m_markUnionAction;
	QModelIndex m_renameItem;
	QSet<QModelIndex> m_deleteItems;

	void createMenu();
	void createAction();
	void updateAction();

	void onDeleteAction();
	void onRenameAction();
};

#endif // MARKTREEVIEW_H