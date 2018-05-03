#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <QObject>
#include <QPainter>
#include <QLayout>
#include <QAbstractItemView>

#include "titledsliderwithspinbox.h"
#include "ItemContext.h"


class QMouseEvent;
class QComboBox;
class QPushButton;
class QLabel;
class QGroupBox;
class Histogram:public QWidget
{
    Q_OBJECT
public:
    explicit Histogram(QWidget *parent = nullptr);
    explicit Histogram(QWidget *parent, const QImage & image);
    void setImage(const QImage & image);
    QVector<int> getHist()const;
    QSize sizeHint()const override;
    int getMinimumCursorValue()const;
    int getMaximumCursorValue()const;
    int getBinCount()const;
    void setDragEnable(bool enable);
public slots:
    void setLeftCursorValue(int value);
    void setRightCursorValue(int value);
signals:
    void minValueChanged(int value);
    void maxValueChanged(int value);
protected:
    void paintEvent(QPaintEvent *event)override;
    void mouseMoveEvent(QMouseEvent *event)override;
    void mousePressEvent(QMouseEvent *event)override;
    void mouseReleaseEvent(QMouseEvent *event)override;
private:
    static const int MIN_WIDTH = 300;
    static const int MIN_HEIGHT = 100;
    static const int BIN_COUNT = 256;

    QVector<int> m_hist;
    int m_minValue;
    int m_maxValue;
    size_t m_count;
    QColor m_curColor;
    QImage m_histImage;

    bool m_mousePressed;
    bool m_rightCursorSelected;
    bool m_leftCursorSelected;
    bool m_cursorEnable;

private:
    qreal getXofLeftCursor();
    qreal getXofRightCursor();
};


class HistogramViewer:public QWidget
{
    Q_OBJECT
public:
    explicit HistogramViewer(QWidget * parent = nullptr)noexcept;
    explicit HistogramViewer(QWidget * parent, const QImage & image)noexcept;


    QVector<int> getHist()const;
    void setEnabled(bool enable);

    int getLeftCursorValue()const;
    int getRightCursorValue()const;

	//model interface
	void setModel(DataItemModel * model);
	void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());
	void activateItem(const QModelIndex & index);
public slots:
    //void setLeftCursorValue(int value);
    //void setRightCursorValue(int value);
	void onMinValueChanged(int value);
	void onMaxValueChanged(int value);
    void onResetButton();
    void onFilterButton();
signals:
    void minValueChanged(int value);
    void maxValueChanged(int value);
private:

    void setImage(const QImage & image);
    void updateImage();
	QModelIndex getDataIndex(const QModelIndex & itemIndex);

	void updateActions();
    void createWidgets();

    void updateParameterLayout(const QString & text);

	bool m_internalUpdate;

    QGridLayout * m_mainLayout;
    QGroupBox * m_histogramGroupBox;
    QGridLayout * m_histogramLayout;
    Histogram * m_hist;
    QLabel * m_histNumLabel;
    QSpinBox * m_histNumSpinBox;
    TitledSliderWithSpinBox * m_minSlider;
    TitledSliderWithSpinBox * m_maxSlider;

    QGroupBox * m_filterGroupBox;
    QGridLayout * m_filterLayout;

    QLabel* m_filterLabel;
    QComboBox * m_filterComboBox;
    QPushButton * m_filterButton;

    QGridLayout * m_medianFilterParameterLayout;
    QLabel * m_medianKernelSizeLabel;
    QSpinBox * m_medianKernelSizeSpinBox;

    QGridLayout * m_gaussianFilterParameterLayout;
    QLabel * m_sigmaXLabel;
    QDoubleSpinBox * m_sigmaXSpinBox;
    QLabel * m_sigmaYLabel;
    QDoubleSpinBox * m_sigmaYSpinBox;



    QPushButton* m_reset;
	//
	QAbstractItemModel * m_model;
	QModelIndex m_modelIndex;
	QSharedPointer<ItemContext> m_ptr;
};

#endif // HISTOGRAM_H
