#ifndef CLASSIFICATIONWIDGET_H
#define CLASSIFICATIONWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QCheckBox;
class QPushButton;
QT_END_NAMESPACE

class ModelData;
class TransferFunction1DWidget;

class ClassificationWidget : public QWidget
{
public:
	ClassificationWidget(ModelData *model, QWidget * parent = 0);
	~ClassificationWidget();

	QSize minimumSizeHint() const;
	QSize sizeHint() const;

	TransferFunction1DWidget *getTF1DWidget() const { return TF1DWidget; }

private:
	TransferFunction1DWidget *TF1DWidget;
	QCheckBox *showStatistic;
	QPushButton *clearTF1D;
};


#endif