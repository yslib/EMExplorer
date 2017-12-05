#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPicture>
#include <QImage>
#include <QPixmap>
#include <QBitmap>
#include <QPainter>
#include <QFileDialog>
#include <QMessageBox>
#include <QLayout>
#include <QLabel>
#include "testinfodialog.h"
#include "mrc.h"
#include "histogram.h"
#include "zoomviwer.h"
#include "sliceviewer.h"
#include "MRCDataModel.h"

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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionOpen_triggered();
    void on_sliceSlider_sliderMoved(int position);
    void on_maxGraySlider_sliderMoved(int position);
    void on_minGraySlider_sliderMoved(int position);
    void on_sliceSlider_valueChanged(int value);
	void onZoomRegionChanged(QRectF region);
private:
    Ui::MainWindow *ui;

    int m_currentContext;
    QVector<MRCContext> m_mrcs;
	SliceViewer * m_sliceViewer;
    Histogram * m_histogramViewer;
	ZoomViwer * m_zoomViewer;
	QVector<MRCDataModel> m_mrcDataModels;

private:
	void _addMRCDataModel(const MRCDataModel & model);
	void _addMRCDataModel(MRCDataModel && model);
	void _setMRCDataModel(int index);
	void _saveMRCDataModel();
    //void _initHistogram();

    void _createMRCContext(const MRC & mrc);
    void _createMRCContext(MRC && mrc);
    void _setMRCContext(int index);
    void _saveMRCContext();
    void _updateGrayThreshold(int minGray,int maxGray);
    //void _displayImage(QSize size = QSize());
    void _init();
    void _destroy();
};

#endif // MAINWINDOW_H
