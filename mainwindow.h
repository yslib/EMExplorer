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
#include "MRCDataModel.h"
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


class Histogram;
class HistogramViewer;
class SliceViewer;
class PixelViewer;
class ImageViewer;
class MRCFileInfoViewer;
class ImageView;

class QTreeView;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:


    void onActionOpenTriggered();
    void onMRCFilesComboBoxIndexActivated(int index);

    void onMaxGrayValueChanged(int position);
    void onMinGrayValueChanged(int position);
    void onSliceValueChanged(int value);

	void onZSliderValueChanged(int value);
	void onYSliderValueChanged(int value);
	void onXSliderValueChanged(int value);

    //void onZoomValueChanged(int value);
    void onZoomDoubleSpinBoxValueChanged(double d);

    void onZoomRegionChanged(const QRectF &region);
	void onSliceViewerDrawingFinished(const QPicture & p);
	void onColorActionTriggered();
	void onSaveActionTriggered();
    void onSaveDataAsActionTriggered();


	void onTreeViewDoubleClicked(const QModelIndex & index);

private:
    Ui::MainWindow *ui;
    int m_currentContext;
    QVector<MRCContext> m_mrcs;
	QVector<MRCDataModel> m_mrcDataModels;
private:
    static constexpr int ZOOM_SLIDER_MAX_VALUE=100;
private:		//ui

	QLabel * m_mrcFileLabel;
	QComboBox * m_mrcFileCBox;
    MRCFileInfoViewer * m_fileInfoViewer;

	QTreeView * m_treeView;
	InformationModel * m_treeViewModel;

	//NestedSliceViewer *m_nestedSliceViewer;

    Histogram * m_histogram;
    HistogramViewer * m_histogramView;
	ZoomViwer * m_zoomViewer;
    PixelViewer * m_pixelViewer;
	//actions
    QAction * m_actionColor;
    QAction * m_actionOpen;
    //test
    //ImageViewer * m_imageViewer;
    ImageView * m_imageView;
private:
    void createActions();
    void createStatusBar();
    void createDockWindows();

	void addMRCDataModel(const MRCDataModel & model);
	void addMRCDataModel(MRCDataModel && model);
	void setMRCDataModel(int index);
	void saveMRCDataModel();
    void deleteMRCDataModel(int index);
    void allControlWidgetsEnable(bool enable);
    void updateGrayThreshold(int minGray,int maxGray);
    void _initUI();
	void _connection();
    void _destroy();
};

#endif // MAINWINDOW_H
