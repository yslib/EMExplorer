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
class QButtonGroup;
QT_END_NAMESPACE
class SliceEditorWidget;
class CategoryControlWidget;
class SliceControlWidget;

class MarkModel;
class ProfileWidget;

class MRC;
class MarkManager;
class RenderParameterWidget;
class TF1DEditor;
class RenderWidget;

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
	void open(const QString& fileName);
	void openMark(const QString & fileName);
	bool eventFilter(QObject* watched, QEvent* event) override;
    ~MainWindow() override = default;
protected:
	void closeEvent(QCloseEvent* event)Q_DECL_OVERRIDE;
private:

	enum FocusState {
		FocusInTopSliceView = 1,
		FocusInRightSliceView = 2,
		FocusInFrontSliceView = 4,
		FocusInVolumeView = 8,
		FocusInSliceWidget = 16,
		FocusInSliceView = FocusInTopSliceView | FocusInRightSliceView | FocusInFrontSliceView,
	};

	void initializeForSliceViewWindow();
	void initializeForVolumeViewWindow();
	void initializeForSliceWithMarkingViewWindow();

	void createWidget();
	void createMenu();
	void createActions();
	void createStatusBar();
	void createSliceEditorPlugins();
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

	void pixelViewActionTriggered();
	void histogramViewActionTriggered();


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

	CategoryControlWidget * m_sliceToolControlWidget;
	SliceControlWidget * m_sliceControlWidget;

	QScrollArea * m_scrollAreaWidget;
	QDockWidget * m_controlDockWidget;

	QDockWidget * m_imageViewDockWidget;
	SliceEditorWidget * m_imageView;

	QDockWidget * m_profileViewDockWidget;
	ProfileWidget * m_profileView;

	MarkManager * m_markManager;
	QDockWidget * m_markManagerDockWidget;
	
	//Menu
	QMenu * m_fileMenu;
    QMenu * m_editMenu;
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
    QAction * m_undoAction;
    QAction * m_redoAction;

	//
	QButtonGroup *m_markButtonGroup;
	QToolButton *m_markAction;
	QToolButton *m_markEraseAction;
	QToolButton *m_markSelectionAction;
	QToolButton *m_anchorAction;

	QToolButton *m_markDeletionAction;
	QToolButton *m_zoomInAction;
	QToolButton *m_zoomOutAction;
	QToolButton *m_resetAction;

	//QHash<QAction*,QToolButton*> m_pluginButtons;

	QToolButton *m_pixelViewAction;
	QToolButton *m_histogramAction;

	// State
	FocusState m_currentFocus;

};

#endif // MAINWINDOW_H
