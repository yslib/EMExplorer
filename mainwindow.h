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
#include <QSlider>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QLayout>
#include <QLabel>
#include <QComboBox>
#include <QAction>
#include <QColorDialog>
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
	void onSliceViewerDrawingFinished(const QPicture & p);
	void onColorActionTriggered();
private:
    Ui::MainWindow *ui;
    int m_currentContext;
    QVector<MRCContext> m_mrcs;
	QVector<MRCDataModel> m_mrcDataModels;
private:		//ui

	QLabel * m_mrcFileLabel;
	QComboBox * m_mrcFileCBox;
	

	SliceViewer * m_sliceViewer;
	QLabel * m_sliceLabel;
	QSlider * m_sliceSlider;
	QSpinBox *m_sliceSpinBox;

    Histogram * m_histogramViewer;
	QLabel * m_histMinLabel;
	QLabel * m_histMaxLabel;
	QSlider * m_histMinSlider;
	QSlider * m_histMaxSlider;
	QSpinBox * m_histMinSpinBox;
	QSpinBox *m_histMaxSpinBox;

	ZoomViwer * m_zoomViewer;
	QLabel * m_zoomLabel;
	QSlider * m_zoomSlider;
	QDoubleSpinBox * m_zoomSpinBox;

	//actions
	QAction * actionColor;


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
    void _initUI();
	void _connection();
    void _destroy();
};

#endif // MAINWINDOW_H
