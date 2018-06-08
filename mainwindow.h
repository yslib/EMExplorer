#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include "mrcdatamodel.h"
QT_BEGIN_NAMESPACE
class QTableView;
class QTreeView;
class QStandardItemModel;
class QAbstractTableModel;
class QAction;
QT_END_NAMESPACE

class ImageView;
class MarkModel;
class ProfileView;
class MRC;


class MainWindow : public QMainWindow
{
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
    void open();
	void save();
    void saveAs();
	void exploererDoubleClicked(const QModelIndex & index);
private:
	Q_OBJECT
	//Widgets
	ImageView * m_imageView;
	ProfileView * m_profileView;
	QTreeView * m_treeView;

	//Data Model
	QHash<QString,std::tuple<QAbstractTableModel*,MRCDataModel* ,QAbstractItemModel*>> m_models;

	//Menu
	QMenu * m_fileMenu;
	QMenu * m_viewMenu;
	
	//Status bar
	QStatusBar * m_statusBar;


	//Actions
	QAction * m_openAction;
	QAction * m_saveAction;
	QAction * m_saveAsAction;
	QAction * m_colorAction;

	void createWidget();
	void createMenu();
    void createActions();
    void createStatusBar();

	void setupModels(const MRC & mrc);
	QAbstractTableModel * setupProfileModel(const MRC & mrc);

};

#endif // MAINWINDOW_H
