#include <QBoxLayout>
#include <QCheckBox>
#include <QPushButton>
#include "ClassificationWidget.h"
#include "TransferFunction1DWidget.h"

ClassificationWidget::ClassificationWidget(ModelData *model, QWidget * parent)
	: QWidget(parent)
{
	QVBoxLayout *mainLayout = new QVBoxLayout;

	// Transfer function widget
	QHBoxLayout *horizonLayout = new QHBoxLayout();
	TF1DWidget = new TransferFunction1DWidget(model, this);
	horizonLayout->addWidget(TF1DWidget);
	mainLayout->addLayout(horizonLayout);

	// TF1D Control
	horizonLayout = new QHBoxLayout();
	showStatistic = new QCheckBox("Statistic");
	connect(showStatistic, SIGNAL(stateChanged(int)), TF1DWidget, SLOT(setShowStatistic(int)));
	horizonLayout->addWidget(showStatistic);
	clearTF1D = new QPushButton("Clear TF1D");
	connect(clearTF1D, SIGNAL(clicked()), TF1DWidget, SLOT(clear1DTF()));
	horizonLayout->addWidget(clearTF1D);
	horizonLayout->addStretch();
	mainLayout->addLayout(horizonLayout);

	mainLayout->addStretch();

	setLayout(mainLayout);
}

ClassificationWidget::~ClassificationWidget()
{

}

QSize ClassificationWidget::minimumSizeHint() const
{
	return QSize(312, 150);
}

QSize ClassificationWidget::sizeHint() const
{
	return QSize(312, 150);
}
