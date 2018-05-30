#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QPixmap>
#include <QFileDialog>
#include <QAction>
#include "mrc.h"
#include "histogram.h"
#include "zoomviwer.h"
#include "sliceviewer.h"
#include "ItemContext.h"
#include "pixelviewer.h"
#include "mrcfileinfoviewer.h"



namespace Ui {
class MainWindow;
}

struct MRCContext{
    int currentMinGray;
    int currentMaxGray;
    int maxSlice;
    int minSlice;
    int currentSlice;
    double currentScale;
    MRC mrcFile;
    QVector<QPixmap> images;

};

class QTableView;
class QStandardItemModel;
class Histogram;
class HistogramViewer;
class SliceViewer;
class PixelViewer;
class ImageViewer;
class MRCFileInfoViewer;
class ImageView;

class QTreeView;
class ProfileView;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void open();
	void save();
    void saveAs();

	void onTreeViewDoubleClicked(const QModelIndex & index);
private:
    Ui::MainWindow *ui;
    int m_currentContext;
    QVector<MRCContext> m_mrcs;
	QVector<ItemContext> m_mrcDataModels;

	//QComboBox * m_filesComboBox;
	//QTableView * m_infoView;
	//QVector<QAbstractItemModel *> m_infoModels;

	QTreeView * m_treeView;
	DataItemModel * m_treeViewModel;

    HistogramViewer * m_histogramView;
    PixelViewer * m_pixelViewer;

    QAction * m_actionColor;
    QAction * m_actionOpen;
    //test
    ImageView * m_imageView;
	
	ProfileView * m_profileView;
private:
    void createActions();
    void createStatusBar();
    void createDockWindows();
	void setupInfo(const QString & text);

	QAbstractTableModel * setupProfileModel(const MRC & mrc);

	void addMRCDataModel(const ItemContext & model);
	void addMRCDataModel(ItemContext && model);
	void setMRCDataModel(int index);
	void saveMRCDataModel();
    void deleteMRCDataModel(int index);
    void allControlWidgetsEnable(bool enable);
    void _destroy();
};

#endif // MAINWINDOW_H
