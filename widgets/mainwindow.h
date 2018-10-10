#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include "model/mrcdatamodel.h"
enum class SliceType;
QT_BEGIN_NAMESPACE
class QTableView;
class QStandardItemModel;
class QAbstractTableModel;
class QAction;
class QSettings;
class QScrollArea;
class QToolButton;
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
class SliceControlWidget;



class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	explicit MainWindow(QWidget *parent = 0);
	void open(const QString& fileName);
	void openMark(const QString & fileName);
	bool eventFilter(QObject* watched, QEvent* event) override;
	~MainWindow();
protected:
	void closeEvent(QCloseEvent* event)Q_DECL_OVERRIDE;
private:

	enum FocusState {
		FocusInTopSliceView = 1,
		FocusInRightSliceView = 2,
		FocusInFrontSliceView = 4,
		FocusInVolumeView = 8,
		FocusInSliceView = FocusInTopSliceView | FocusInRightSliceView | FocusInFrontSliceView,
		FocusInSliceWidget = FocusInSliceView
	};

	void createWidget();
	void createMenu();
	void createActions();
	void createStatusBar();
	void createMarkTreeView();
	bool saveMark();

	void saveAs();
	void writeSettingsForDockWidget(QDockWidget *dock, QSettings* settings);
	void readSettingsForDockWidget(QDockWidget * dock, QSettings* settings);
	void writeSettingsForImageView(SliceEditorWidget * view, QSettings * settings);
	void readSettingsForImageView(SliceEditorWidget * view, QSettings * settings);
	void readSettings();
	void writeSettings();
	void setDefaultLayout();
	void setParallelLayout();

	void updateActionsAndControlPanelByWidgetFocus(FocusState state);
	void updateActionsBySelectionInSliceView();
	void sliceViewSelected(SliceType type);

	AbstractSliceDataModel * replaceSliceModel(AbstractSliceDataModel* model);
	MarkModel * replaceMarkModel(MarkModel * model);
	QAbstractTableModel * replaceProfileModel(QAbstractTableModel * model);
	QAbstractTableModel * setupProfileModel(const MRC & mrc);

	//Widgets
	QDockWidget * m_volumeViewDockWidget;
	RenderWidget * m_volumeView;

	RenderParameterWidget * m_volumeControlWidget;
	SliceToolWidget * m_sliceToolControlWidget;
	SliceControlWidget * m_sliceControlWidget;
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

	//Menu
	QMenu * m_fileMenu;
	QMenu * m_viewMenu;

	//Status bar
	QStatusBar * m_statusBar;

	// Tool Bar
	QToolBar *m_toolBar;
		
	// Actions
	QAction * m_openAction;
	QAction * m_saveAction;
	QAction * m_openMarkAction;
	QAction * m_setDefaultLayoutAction;
	QAction * m_parallelLayoutAction;

	QToolButton *m_markAction;
	QToolButton *m_markSelectionAction;

	QToolButton *m_markDeletionAction;
	QToolButton *m_zoomInAction;
	QToolButton *m_zoomOutAction;
	QToolButton * m_resetAction;
	QToolButton *m_pixelViewAction;
	QToolButton *m_histogramAction;


	// State


};

#endif // MAINWINDOW_H
