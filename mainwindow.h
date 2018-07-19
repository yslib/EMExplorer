#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include "mrcdatamodel.h"
QT_BEGIN_NAMESPACE
class QTableView;
class QStandardItemModel;
class QAbstractTableModel;
class QAction;
class QSettings;
QT_END_NAMESPACE
class ImageCanvas;
class MarkModel;
class ProfileView;
class MRC;
class MarkTreeView;
class MarkInfoWidget;
class MainWindow : public QMainWindow
{
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
	
protected:
	void closeEvent(QCloseEvent* event)Q_DECL_OVERRIDE;
private slots:
    void open();
	bool saveMark();
	void openMark();
    void saveAs();

	void writeSettingsForDockWidget(QDockWidget *dock, QSettings* settings);
	void readSettingsForDockWidget(QDockWidget * dock, QSettings* settings);
	void writeSettingsForImageView(ImageCanvas * view, QSettings * settings);
	void readSettingsForImageView(ImageCanvas * view, QSettings * settings);
	void readSettings();
	void writeSettings();
private:
	Q_OBJECT
	//Widgets
	ImageCanvas * m_imageView;
	QDockWidget * m_profileViewDockWidget;
	ProfileView * m_profileView;
	QDockWidget * m_treeViewDockWidget;
	MarkTreeView * m_treeView;
	MarkInfoWidget * m_markInfoWidget;
	//Menu
	QMenu * m_fileMenu;
	QMenu * m_viewMenu;
	//Status bar
	QStatusBar * m_statusBar;
	//Actions
	QAction * m_openAction;
	QAction * m_saveAction;
	QAction * m_openMarkAction;

	void createWidget();
	void createMenu();
    void createActions();
    void createStatusBar();
	void createMarkTreeView();
	AbstractSliceDataModel * replaceSliceModel(AbstractSliceDataModel* model);
	MarkModel * replaceMarkModel( MarkModel * model);
	QAbstractTableModel * replaceProfileModel(QAbstractTableModel * model);
	//void setupModels(const MRC & mrc);
	QAbstractTableModel * setupProfileModel(const MRC & mrc);

};

#endif // MAINWINDOW_H
