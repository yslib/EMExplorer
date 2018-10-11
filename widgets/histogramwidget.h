#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <QPainter>
#include <QLayout>
#include <QAbstractItemView>

#include "titledsliderwithspinbox.h"
#include "abstract/abstractplugin.h"


class QMouseEvent;
class QComboBox;
class QPushButton;
class QLabel;
class QGroupBox;

class SliceEditorWidget;

class Histogram:public QWidget
{
    Q_OBJECT
public:
    explicit Histogram(QWidget *parent = nullptr);
    explicit Histogram(QWidget *parent, const QImage & image);
    void setImage(const QImage & image);
	void setPixmap(QPixmap * pixmap);
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


class HistogramWidget:public AbstractPlugin
{
    Q_OBJECT
public:
	explicit HistogramWidget(SliceType type,
		const QString & name,
		SliceEditorWidget * sliceEidtor = nullptr,
		QWidget * parent = nullptr);
private slots:

	void onMinValueChanged(int value);
	void onMaxValueChanged(int value);
    void resetOriginalImage();
    void filterImage();


signals:
    void minValueChanged(int value);
    void maxValueChanged(int value);

protected slots:
	void updateDataModel()override;

private:
	void initWidgets();
	void init();
	void createWidgets();
	void createConnections();

    void histEqualizeImage();
    void updateParameterLayout(const QString & text);

	SliceEditorWidget * m_sliceWidget;
	SliceType m_sliceType;

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

    QGridLayout * m_parameterLayout;
    QLabel * m_medianKernelSizeLabel;
    QSpinBox * m_medianKernelSizeSpinBox;
    QLabel * m_sigmaXLabel;
    QDoubleSpinBox * m_sigmaXSpinBox;
    QLabel * m_sigmaYLabel;
    QDoubleSpinBox * m_sigmaYSpinBox;

    QPushButton* m_reset;
	//

	int m_currentIndex;
};





#endif // HISTOGRAM_H
