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
    void onActionOpenTriggered();
    void onMRCFilesComboBoxIndexChanged(int index);
    void onMaxGrayValueChanged(int position);
    void onMinGrayValueChanged(int position);
    void onSliceValueChanged(int value);
    //void onZoomValueChanged(int value);
    void onZoomDoubleSpinBoxValueChanged(double d);

    void onZoomRegionChanged(const QRectF &region);
	void onSliceViewerDrawingFinished(const QPicture & p);
	void onColorActionTriggered();
	void onSaveActionTriggered();
    void onSaveDataAsActionTriggered();

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
    void _deleteMRCDataModel(int index);

    void _allControlWidgetsEnable(bool enable);

    void _updateGrayThreshold(int minGray,int maxGray);
    void _initUI();
	void _connection();
    void _destroy();
};

#endif // MAINWINDOW_H
