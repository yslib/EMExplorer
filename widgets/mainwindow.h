#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include "model/mrcdatamodel.h"
QT_BEGIN_NAMESPACE
class QTableView;
class QStandardItemModel;
class QAbstractTableModel;
class QAction;
class QSettings;
class QScrollArea;
QT_END_NAMESPACE
class SliceEditorWidget;
class MarkModel;
class ProfileWidget;
class MRC;
class MarkManagerWidget;
class MarkInfoWidget;
class RenderParameterWidget;
class TF1DEditor;
class RenderWidget;
class SliceToolWidget;
class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
protected:
	void closeEvent(QCloseEvent* event)Q_DECL_OVERRIDE;
private:
	void createWidget();
	void createMenu();
	void createActions();
	void createStatusBar();
	void createMarkTreeView();
	void open();
	bool saveMark();
	void openMark();
	void saveAs();
	void writeSettingsForDockWidget(QDockWidget *dock, QSettings* settings);
	void readSettingsForDockWidget(QDockWidget * dock, QSettings* settings);
	void writeSettingsForImageView(SliceEditorWidget * view, QSettings * settings);
	void readSettingsForImageView(SliceEditorWidget * view, QSettings * settings);
	void readSettings();
	void writeSettings();
	void setDefaultLayout();

	AbstractSliceDataModel * replaceSliceModel(AbstractSliceDataModel* model);
	MarkModel * replaceMarkModel(MarkModel * model);
	QAbstractTableModel * replaceProfileModel(QAbstractTableModel * model);
	QAbstractTableModel * setupProfileModel(const MRC & mrc);

	//Widgets
	QDockWidget * m_volumeViewDockWidget;
	RenderWidget * m_volumeView;

	RenderParameterWidget * m_renderParameterWidget;
	SliceToolWidget * m_sliceToolWidget;
	QScrollArea * m_scrollAreaWidget;
	QDockWidget * m_controlDockWidget;

	QDockWidget * m_imageViewDockWidget;
	SliceEditorWidget * m_imageView;

	QDockWidget * m_profileViewDockWidget;
	ProfileWidget * m_profileView;

	QDockWidget * m_treeViewDockWidget;
	MarkManagerWidget * m_treeView;

	QDockWidget * m_markInfoDockWidget;
	MarkInfoWidget * m_markInfoWidget;

	//QDockWidget *m_tfEditorDockWidget;
	//TF1DEditor * m_tfEditorWidget;
	//Menu
	QMenu * m_fileMenu;
	QMenu * m_viewMenu;
	//Status bar
	QStatusBar * m_statusBar;
	//Actions
	QAction * m_openAction;
	QAction * m_saveAction;
	QAction * m_openMarkAction;
	QAction * m_setDefaultLayoutAction;
};

#endif // MAINWINDOW_H
