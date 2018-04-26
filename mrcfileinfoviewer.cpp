#include "mrcfileinfoviewer.h"
#include "mrc.h"
#include <QDebug>
MRCFileInfoViewer::MRCFileInfoViewer(QWidget *parent)noexcept:QWidget(parent) {
	m_layout = new QGridLayout(this);
	m_label = new QLabel(this);
	m_filesComboBox = new QComboBox(this);
	m_filesInfoTextEdit = new QTextEdit(this);

	m_filesInfoTextEdit->setReadOnly(true);

	m_label->setText(tr("MRCFiles:"));


	m_layout->addWidget(m_label, 0, 0);
	m_layout->addWidget(m_filesComboBox, 0, 1);
	m_layout->addWidget(m_filesInfoTextEdit, 1, 0, 1, 2);


	createConnections();
}
void MRCFileInfoViewer::addFileName(const QString &fileName)
{
	m_filesComboBox->addItem(fileName);
}

void MRCFileInfoViewer::addItem(const QString &fileName, const QVariant &userData)
{
	m_filesComboBox->addItem(fileName, userData);
}

void MRCFileInfoViewer::setText(const QString &info)
{
	m_filesInfoTextEdit->setText(info);
}

void MRCFileInfoViewer::createConnections()
{
	connect(m_filesComboBox, SIGNAL(currentIndexChanged(int)), this, SIGNAL(currentIndexChanged(int)));
	connect(m_filesComboBox, SIGNAL(activated(int)), this, SIGNAL(activated(int)));
}

void MRCFileInfoViewer::setCurrentIndex(int index)
{
	m_filesComboBox->setCurrentIndex(index);
}
int MRCFileInfoViewer::count()const {
	return m_filesComboBox->count();
}
QVariant MRCFileInfoViewer::itemData(int index, int role) const {
	return m_filesComboBox->itemData(index, role);
}




//DataItemModel Defination




