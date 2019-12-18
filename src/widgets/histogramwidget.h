#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <QPainter>
#include <QLayout>
#include <QAbstractItemView>
#include <QVector2D>

#include "titledsliderwithspinbox.h"
#include "abstract/abstractsliceeditorplugin.h"


class QMouseEvent;
class QComboBox;
class QPushButton;
class QLabel;
class QGroupBox;
class SliceEditorWidget;
class TitledSliderWidthDoubleSpinBox;
class DoubleSlider;



class Histogram:public QWidget
{
    Q_OBJECT
public:
    explicit Histogram(QWidget *parent = nullptr);
    explicit Histogram(QWidget *parent, const QImage & image);
    void setImage(const QImage & image);
    QVector<double> getHist()const;
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
	void drawHist(QPainter * painter);
	QVector2D wtos(const QVector2D & p);

    static const int MIN_WIDTH = 300;
    static const int MIN_HEIGHT = 100;
    static const int BIN_COUNT = 256;

    QVector<double> m_hist;
	bool m_histUpdate;
    int m_minValue;
    int m_maxValue;
    size_t m_count;
    QColor m_curColor;
    QImage m_histImage;
    QLabel * m_preview;

    bool m_mousePressed;
    bool m_rightCursorSelected;
    bool m_leftCursorSelected;
	bool m_cursorEnable;

	int m_maxGrayValue;

	QVector2D xRange;       ///< range in x direction
	QVector2D yRange;       ///< range in y direction
	QPixmap cache;			///< pixmap for caching the painted histogra
	int padding;           ///< additional border of the widget
	int arrowLength;       ///< length of the arrows at the end of coordinate axes
	int arrowWidth;        ///< width of the arrows at the end of coordinate axes
	QString xAxisText;     ///< caption of the x axis
	QString yAxisText;     ///< caption of the y axis
	QVector2D gridSpacing;  ///< width and height of the underlying grid

private:
    qreal getXofLeftCursor();
    qreal getXofRightCursor();
};

class ImagePreviewWidget:public QWidget
{
    Q_OBJECT
public:
    explicit ImagePreviewWidget(const QImage & image,QWidget * parent = nullptr);
    explicit ImagePreviewWidget(QWidget * parent = nullptr);
    void setImage(const QImage & image);
	QImage image()const;
private:
    QLabel * m_preview;
    QImage m_image;
    Histogram * m_hist;
};

/**
 * \brief This is a widget supply some image processing features for slice in \a SliceEditorWidget
 * 
 * \sa SliceEditorWidget
 */

class EqualizationControlWidget:public QWidget {
	Q_OBJECT
public:
	EqualizationControlWidget(QWidget * parent);
	int equalizationLevel()const;
	int minValue()const;
	int maxValue()const;
	void reset();
signals:
	void valueChanged();
private:
	TitledSliderWithSpinBox * m_equalLevel;
	DoubleSlider* m_doubleSlier;
};

class FilterControlWidget:public QWidget 
{
	Q_OBJECT
public:
	FilterControlWidget(QWidget * parent);
	double sigmaX()const;
	double sigmaY()const;
	int medianKernelSize()const;
	QString text()const;
	void reset();
signals:
	void filter();
private:
	void updateLayout(const QString & text);
	QLabel* m_filterLabel;
	QComboBox * m_filterComboBox;
	QPushButton * m_filterButton;
	QVBoxLayout * m_controllerLayout;
	TitledSliderWithSpinBox* m_medianKernelController;
	TitledSliderWidthDoubleSpinBox *m_sigmaXController;
	TitledSliderWidthDoubleSpinBox *m_sigmaYController;
};

class BrightnessContrastControlWidget:public QWidget {
	Q_OBJECT
public:
	BrightnessContrastControlWidget(QWidget * parent = nullptr);
	double contrast()const;
	double brightness()const;
	void reset();
signals:
	void valueChanged();
private:
	TitledSliderWidthDoubleSpinBox * m_contrastFactor;
	TitledSliderWidthDoubleSpinBox * m_brightnessFactor;
	
};


class HistogramWidget:public AbstractSliceViewPlugin
{
    Q_OBJECT
public:
	explicit HistogramWidget(SliceType type,
		SliceEditorWidget * sliceEidtor = nullptr,
		QWidget * parent = nullptr);
private slots:
	void onMinValueChanged(int value);
	void onMaxValueChanged(int value);
    void resetOriginalImage();
signals:
    void minValueChanged(int value);
    void maxValueChanged(int value);
	void commit(const QImage & image);
protected slots:
	void updateDataModel()override;
private:
	void initWidgets();
	void init();
	void createWidgets();

	void filterImage();
    void histEqualizeImage();
	void updateContrastAndBrightness();
    //Histogram * m_hist;
    ImagePreviewWidget * m_preview;
	FilterControlWidget * m_filterWidget;
	BrightnessContrastControlWidget * m_brightWidget;
	EqualizationControlWidget * m_equliWidget;

	QImage m_result;
    QPushButton* m_reset;
	QPushButton* m_apply;
	QTabWidget * m_tabWidget;
};





#endif // HISTOGRAM_H
