#ifndef TRANSFERFUNCION1DWIDGET_H
#define TRANSFERFUNCION1DWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QPainter;
class QPaintEvent;
QT_END_NAMESPACE

class Volume;
class ModelData;

class TransferFunction1DWidget : public QWidget
{
	Q_OBJECT

public:
	TransferFunction1DWidget(ModelData *modelData, QWidget * parent = 0);
	~TransferFunction1DWidget();

	QSize minimumSizeHint() const;
	QSize sizeHint() const;

	// attributes
	void showIsosurfaceStatistic(bool flag)   { showIsoStat = flag; }

	// calculate 1DTF
	void calc1DTF();

	// get 1DTF
	void getTF1D(unsigned char TF[256][4]);

	// save / load 1DTF
	void loadTF1D(const char *filename);
	void saveTF1D(const char *filename);

signals:
	void TF1DChanged();

public slots:
	void setShowStatistic(int state);
	void clear1DTF();

protected:
	void paintEvent(QPaintEvent* event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);

protected:
	// TF editing
	bool checkInRect(int x, int y)  { return (x > rect.left() && x < rect.right() && y > rect.top() && y < rect.bottom()); }
	void selectPointIndex(int x, double y);

	// draw functions
	void drawGrid(QPainter& painter);
	void drawHistogram(QPainter& painter);
	void drawLineDDA(int x1, int y1, int x2, int y2, const QColor& color1, const QColor& color2, QPainter& painter);
	void drawTransferFunction(QPainter& painter);

private:
	// Model data
	ModelData *modelData;

	// show histogram flag
	bool showIsoStat;

	// Rect Size
	QRect rect;

	// <scalar value, alpha> [0, 255] [0, 1]
	// for TF1DRGBA, double points between beginning and ending
	struct KeyPoint {
		KeyPoint(unsigned char s, double a, QColor c) : scalar(s), alpha(a), color(c) {}
		unsigned char scalar;
		double alpha;
		QColor color;
	};
	bool inEditing;
	int selectIndex, MRLeft, MRRight;
	std::vector<KeyPoint> keyPoints;
	double TF[256][4];
};


#endif