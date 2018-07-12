#include <vector>
#include <QColorDialog>
#include <QPainter>
#include <qevent.h>
#include "TransferFunction1DWidget.h"
#include "ModelData.h"
#include "Volume.h"

using namespace std;

TransferFunction1DWidget::TransferFunction1DWidget(ModelData *modelData, QWidget * parent)
	: QWidget(parent)
{
	this->modelData = modelData;
	showIsoStat   = false;
	inEditing     = false;
	selectIndex   = -1;
	keyPoints.push_back(KeyPoint(0, 0, Qt::black));
	keyPoints.push_back(KeyPoint(255, 1, Qt::white));
	calc1DTF();
}

TransferFunction1DWidget::~TransferFunction1DWidget()
{

}

QSize TransferFunction1DWidget::minimumSizeHint() const
{
	return QSize(312, 160);
}

QSize TransferFunction1DWidget::sizeHint() const
{
	return QSize(312, 160);
}

void TransferFunction1DWidget::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	int margin = (width() - 258) / 2;
	rect = QRect(margin + 5, 10, 258, 122);
	drawGrid(painter);
	drawHistogram(painter);
	drawTransferFunction(painter);
}

void TransferFunction1DWidget::mousePressEvent(QMouseEvent *event)
{
	QPoint point = event->pos();
	if(checkInRect(point.x(), point.y())) {
		if (event->buttons() & Qt::LeftButton) {
			inEditing = true;
			int x = point.x() - rect.left() - 1;
			double y = (rect.bottom() - 1 - point.y()) / 120.0;
			selectPointIndex(x, y);
			update();
		}
		else if (event->buttons() & Qt::RightButton) {
			size_t i, rangeStart, rangeEnd;
			int x = point.x() - rect.left() - 1;
			double y = (rect.bottom() - 1 - point.y()) / 120.0;
			for(i = 0; i < keyPoints.size(); ++i) {
				if(abs(x - (int)keyPoints[i].scalar) < 5) {
					// check the next neighbor point
					if(i != keyPoints.size() - 1 && abs(x - keyPoints[i].scalar) > abs(keyPoints[i + 1].scalar - x))
						i = i + 1;
					break;
				}
				if(i != keyPoints.size() - 1 && x >= keyPoints[i].scalar && x < keyPoints[i + 1].scalar) {
					rangeStart = i;
					rangeEnd   = i + 1;
				}
			}
			size_t index = (i == keyPoints.size()) ? rangeStart : i;
			QColor color = QColorDialog::getColor(keyPoints[index].color);
			if(color.isValid()) {
				if(i < keyPoints.size())
					keyPoints[i].color = color;
				else {
					keyPoints[rangeStart].color = color;
					keyPoints[rangeEnd].color   = color;
				}
				calc1DTF();
				update();
				emit TF1DChanged();
			}
		}
	}
}

void TransferFunction1DWidget::mouseMoveEvent(QMouseEvent *event)
{
	if (event->buttons() & Qt::LeftButton) {
		QPoint point = event->pos();
		if(selectIndex >= 0 && checkInRect(point.x(), point.y())) {
			int x = point.x() - rect.left() - 1;
			x = Max(Min(x, MRRight), MRLeft);
			double y = Max(Min((rect.bottom() - 1 - point.y()) / 120.0, 1.0), 0.0);
			keyPoints[selectIndex].scalar = x;
			keyPoints[selectIndex].alpha = y;
			calc1DTF();
			update();
		}
	}
}

void TransferFunction1DWidget::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) {
		if(selectIndex != -1) {		// has modified
			inEditing = false;
			selectIndex = -1;
			update();
			emit TF1DChanged();
		}
	}
}

/*
 * calculate 1DTF
 */
void TransferFunction1DWidget::calc1DTF()
{
	vector<double> gradient;
	// calculate the gradients
	if(keyPoints[0].scalar == keyPoints[1].scalar)
		gradient.push_back(0);
	else
		gradient.push_back((keyPoints[1].alpha - keyPoints[0].alpha) / (keyPoints[1].scalar - keyPoints[0].scalar));
	for(size_t i = 1; i < keyPoints.size() - 1; ++i) {
		if(keyPoints[i - 1].scalar == keyPoints[i - 1].scalar)
			gradient.push_back(0);
		else
			gradient.push_back((keyPoints[i + 1].alpha - keyPoints[i - 1].alpha) / (keyPoints[i + 1].scalar - keyPoints[i - 1].scalar));
	}
	if(keyPoints[keyPoints.size() - 2].scalar == keyPoints[keyPoints.size() - 1].scalar)
		gradient.push_back(0);
	else
		gradient.push_back((keyPoints[keyPoints.size() - 1].alpha - keyPoints[keyPoints.size() - 2].alpha) / (keyPoints[keyPoints.size() - 1].scalar - keyPoints[keyPoints.size() - 2].scalar));

	// interpolate the transfer function
	for(size_t i = 0; i < keyPoints.size() - 1; ++i) {
		double deltaT = keyPoints[i + 1].scalar - keyPoints[i].scalar;
		double P0 = keyPoints[i].alpha, P1 = keyPoints[i + 1].alpha;
		double V0 = gradient[i] * deltaT, V1 = gradient[i + 1] * deltaT;
		double A = 2 * P0 - 2 * P1 + V0 + V1;
		double B = -3 * P0 + 3 * P1 - 2 * V0 - V1;
		double C = V0;
		double D = P0;

		for(size_t j = keyPoints[i].scalar; j <= keyPoints[i + 1].scalar; ++j) {
			double t     = deltaT > 0 ? (j - keyPoints[i].scalar) / deltaT : 0;
			double alpha = max(0.0, min(1.0, ((A * t + B) * t + C) * t + D));
			TF[j][0] = (keyPoints[i].color.red()   * (1 - t) + keyPoints[i + 1].color.red()   * t) / 255.0;
			TF[j][1] = (keyPoints[i].color.green() * (1 - t) + keyPoints[i + 1].color.green() * t) / 255.0;
			TF[j][2] = (keyPoints[i].color.blue()  * (1 - t) + keyPoints[i + 1].color.blue()  * t) / 255.0;
			TF[j][3] = alpha;
		}
	}
}

void TransferFunction1DWidget::getTF1D(unsigned char uTF[256][4])
{
	for(size_t i = 0; i < 256; ++i) {
		uTF[i][0] = unsigned char(Min(TF[i][0] * 255 + 0.5, 255.0));
		uTF[i][1] = unsigned char(Min(TF[i][1] * 255 + 0.5, 255.0));
		uTF[i][2] = unsigned char(Min(TF[i][2] * 255 + 0.5, 255.0));
		uTF[i][3] = unsigned char(Min(TF[i][3] * 255 + 0.5, 255.0));
	}
}

/*
 * Load Transfer Function
 */
void TransferFunction1DWidget::loadTF1D(const char *filename)
{
	FILE* fp = fopen(filename, "r");
	if(!fp) {
		cout<<"Can't open file "<<filename<<endl;
		return;
	}

	int keyNumber = 0;
	fscanf(fp, "%d\n", &keyNumber);

	int scalar;
	double a, r, g, b;
	keyPoints.clear();
	for(int i = 0; i < keyNumber; ++i) {
		fscanf(fp, "%d %lf %lf %lf %lf\t", &scalar, &a, &r, &g, &b);
		keyPoints.push_back(KeyPoint(scalar, a, QColor(r * 255, g * 255, b * 255)));
	}
	
	fclose(fp);
	calc1DTF();	
	update();
	emit TF1DChanged();
}

/*
 * Save Transfer Function
 */
void TransferFunction1DWidget::saveTF1D(const char *filename)
{
	FILE* fp = fopen(filename, "w");
	if(!fp) {
		cout<<"Can't open file "<<filename<<endl;
		return;
	}

	// Key points
	fprintf(fp, "%d\n", (int)keyPoints.size());
	int scalar;
	for(size_t i = 0; i < keyPoints.size(); ++i) {
		scalar = keyPoints[i].scalar;
		fprintf(fp, "%d %lf %lf %lf %lf\t", scalar, keyPoints[i].alpha, keyPoints[i].color.red() / 255.0, keyPoints[i].color.green() / 255.0, keyPoints[i].color.blue() / 255.0);
	}

	fclose(fp);
}

/*
 * slot functions
 */
void TransferFunction1DWidget::setShowStatistic(int state)  
{ 
	showIsoStat = state; 
	update();
}

void TransferFunction1DWidget::clear1DTF()
{
	keyPoints.clear();
	keyPoints.push_back(KeyPoint(0, 0, Qt::black));
	keyPoints.push_back(KeyPoint(255, 1, Qt::white));
	calc1DTF();
	update();
	emit TF1DChanged();
}

/*
 * helper functions
 */
void TransferFunction1DWidget::selectPointIndex(int x, double y)
{
	size_t i;
	// find old points
	for(i = 0; i < keyPoints.size(); ++i) {
		if(abs(x - (int)keyPoints[i].scalar) < 5) {
			// check the next neighbor point
			if(i != keyPoints.size() - 1 && abs(x - keyPoints[i].scalar) > abs(keyPoints[i + 1].scalar - x))
				i = i + 1;
			selectIndex = (int)i;
			if(i == 0) {
				MRLeft = MRRight = 0;
			}
			else if(i == keyPoints.size() - 1) {
				MRLeft = MRRight = 255;
			}
			else {
				MRLeft = keyPoints[i - 1].scalar;
				MRRight = keyPoints[i + 1].scalar;
			}
			keyPoints[i].alpha = y;
			break;
		}
	}

	// no selection, create new point
	if(i == keyPoints.size()) {
		vector<KeyPoint>::iterator iter;
		for(iter = keyPoints.begin(), i = 0; iter != keyPoints.end(); ++iter, ++i) 
			if(iter->scalar > x)
				break;

		if(iter == keyPoints.end())
			cout<<"Error in SelectPointIndex"<<endl;
		selectIndex = (int)i;
		MRLeft  = keyPoints[i - 1].scalar;
		MRRight = keyPoints[i].scalar;
		double t  = (x - keyPoints[i - 1].scalar) / double(keyPoints[i].scalar - keyPoints[i - 1].scalar);
		int  red  = keyPoints[i - 1].color.red()   * (1 - t) + keyPoints[i].color.red()   * t;
		int green = keyPoints[i - 1].color.green() * (1 - t) + keyPoints[i].color.green() * t;
		int blue  = keyPoints[i - 1].color.blue()  * (1 - t) + keyPoints[i].color.blue()  * t;
		QColor color(red, green, blue);
		keyPoints.insert(iter, KeyPoint(x, y, color));
		calc1DTF();
	}

	// check
	if(selectIndex < 0 || selectIndex >= (int)keyPoints.size() || MRLeft > MRRight)
		cout<<"Error in SelectPointIndex Check"<<endl;
}

void TransferFunction1DWidget::drawGrid(QPainter& painter)
{
	QPen   oldPen   = painter.pen();
	QBrush oldBrush = painter.brush();
	
	// rect
	painter.setBrush(QBrush(Qt::white, Qt::SolidPattern));
	painter.drawRect(rect);

	// long dot lines
	painter.setPen(QPen(Qt::gray, 1, Qt::DotLine));
	for(int i = 1; i < 8; ++i)
		painter.drawLine(rect.left() + 1 + i * 32, rect.top(), rect.left() + 1 + i * 32, rect.bottom() + 4);
	for(int i = 1; i < 5; ++i)
		painter.drawLine(rect.left() - 4, rect.bottom() - 1 - i * 24, rect.right() , rect.bottom() - 1 - i * 24);
	painter.setPen(oldPen);

	// short solid lines and text labels
	QString str;
	for(int i = 0; i < 9; ++i) {
		painter.drawLine(rect.left() + 1 + i * 32, rect.bottom(), rect.left() + 1 + i * 32, rect.bottom() + 4);
		str.sprintf("%d", i * 32);
		painter.drawText(rect.left() + i * 32 - 2 * str.length(), rect.bottom() + 14, str);
	}
	for(int i = 0; i < 6; ++i) {
		painter.drawLine(rect.left() - 4, rect.bottom() - 1 - i * 24, rect.left(), rect.bottom() - 1 - i * 24);
		str.sprintf("%.1lf", i / 5.0);
		painter.drawText(rect.left() - 22, rect.bottom() - i * 24 + 3, str);
	}

	painter.setBrush(oldBrush);
	painter.setPen(oldPen);
}

void TransferFunction1DWidget::drawHistogram(QPainter& painter)
{
	if(showIsoStat) {
		Volume *volume = modelData->getVolume();
		if(volume == NULL) return;
		
		// draw histogram
		QPen oldPen = painter.pen();
		painter.setPen(QColor(139, 162, 228));
		double  maxValue = volume->getMaxIsoValue();
		double *histogram = volume->getIsosurfaceStat();
		for(int i = 0; i < 256; ++i) {
			int temp = min(int(histogram[i] * 120/ maxValue), 120);
			painter.drawLine(rect.left() + 1 + i, rect.bottom() - 1, rect.left() + 1 + i, rect.bottom() - temp - 1);
		}
		painter.setPen(oldPen);
	}
}

void TransferFunction1DWidget::drawLineDDA(int x1, int y1, int x2, int y2, QColor& color1, QColor& color2, QPainter& painter)
{
	double t, red, green, blue;
	double x = x1, y = y1;
	int dx = x2 - x1, dy = y2 - y1;
	QColor color(color1);

	int steps = (abs (dx) > abs (dy)) ? abs (dx) : abs (dy);
	double xIncrement = dx / (double) steps;
	double yIncrement = dy / (double) steps;

	QPen oldPen = painter.pen();
	painter.setPen(color);
	painter.drawPoint(Round(x), Round(y));
	for(int k = 0; k < steps; ++k) {
		x += xIncrement;
		y += yIncrement;
		t = (steps - k) / (double)steps;
		red   = color1.red()   * (1.0 - t) + color2.red()   * t;
		green = color1.green() * (1.0 - t) + color2.green() * t;
		blue  = color1.blue()  * (1.0 - t) + color2.blue()  * t;
		painter.setPen(QColor(Round(red), Round(green), Round(blue)));
		painter.drawPoint(Round(x), Round(y));
	}
	painter.setPen(oldPen);
}

void TransferFunction1DWidget::drawTransferFunction(QPainter& painter)
{	
	// key points
	for(size_t i = 0; i < keyPoints.size(); ++i) {
		int x = static_cast<int>(rect.left() + 1 + keyPoints[i].scalar);
		int y = static_cast<int>(rect.bottom() - 1 - 119 * keyPoints[i].alpha);
		painter.drawEllipse(QPoint(x, y), 2, 2);
	}

	// TF alpha
	for(int i = 0; i < 255; ++i) {
		int x = rect.left() + 1 + i;
		int startY = static_cast<int>(rect.bottom() - 1 - 119 * TF[i][3]);
		int endY   = static_cast<int>(rect.bottom() - 1 - 119 * TF[i + 1][3]);
		drawLineDDA(x, startY, x + 1, endY, QColor(TF[i][0] * 255, TF[i][1] * 255, TF[i][2] * 255), QColor(TF[i + 1][0] * 255, TF[i + 1][1] * 255, TF[i + 1][2] * 255), painter);
	}

	// TF color
	if(!inEditing) {
		QPen oldPen = painter.pen();
		for(int i = 0; i < 256; ++i) {
			int x = rect.left() + 1 + i;
			int startY = static_cast<int>(rect.bottom() - 1 - 119 * TF[i][3]);
			painter.setPen(QColor(Round(TF[i][0] * 255), Round(TF[i][1] * 255), Round(TF[i][2] * 255)));
			for(int y = startY; y < rect.bottom(); ++y)
				painter.drawPoint(Round(x), Round(y));
		}
		painter.setPen(oldPen);
	}
}