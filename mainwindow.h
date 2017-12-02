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
#include <QtCharts>
#include "testinfodialog.h"
#include "mrc.h"
#include "histogram.h"

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

private:
    Ui::MainWindow *ui;

    int m_currentContext;
    QVector<MRCContext> m_mrcs;

    QLabel * m_imageLabel;
    Histogram * m_hist;
    QPixmap m_image;
    QBitmap m_mask;

    TestInfoDialog * TESTINFO;

private:
    //void _initHistogram();

    void _createMRCContext(const MRC & mrc);
    void _createMRCContext(MRC && mrc);
    void _setMRCContext(int index);
    void _saveMRCContext();
    void _updateGrayThreshold(int minGray,int maxGray);
    void _displayImage(QSize size = QSize());
    void _init();
    void _destroy();
};

#endif // MAINWINDOW_H
