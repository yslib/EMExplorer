#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPicture>
#include <QImage>
#include <QFileDialog>
#include <QMessageBox>
#include <QLayout>
#include <QLabel>
#include "mrc.h"

namespace Ui {
class MainWindow;
}

struct MRCContext{
    MRC mrcFile;
    int minGray;
    int maxGray;
    int slice;
    double scale;

};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionOpen_triggered();

private:
    Ui::MainWindow *ui;
    int m_currentMinGray;
    int m_currentMaxGray;
    int m_currentSlice;
    double m_currentScale;
    //QVector<MRC> mrcFiles;
    QVector<MRC> m_mrcs;
    //QPicture * m_picture;
    QLabel * m_imageView;
private:
    void _setMRCContext(int index);
    void _init();
    void _destroy();
};

#endif // MAINWINDOW_H
