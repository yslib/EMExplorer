#include "widgets/markcategorydialog.h"
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QColor>
#include <QColorDialog>
#include <QGridLayout>
#include <QMessageBox>
#include <QDebug>
MarkCategoryDialog::MarkCategoryDialog(QWidget *parent) :QDialog(parent)
{

	m_layout = new QGridLayout;

	m_nameLabel = new QLabel(QStringLiteral("Category Name:"));
	m_name = new QLineEdit;
	m_layout->addWidget(m_nameLabel, 0, 0);
	m_layout->addWidget(m_name, 0, 1);

	m_colorLabel = new QLabel(QStringLiteral("Color:"));
	m_colorDisplay = new QLineEdit;
	m_colorDisplay->setReadOnly(true);
	m_colorButton = new QPushButton(QStringLiteral("Choose..."));
	m_layout->addWidget(m_colorLabel, 1, 0);
	m_layout->addWidget(m_colorDisplay, 1, 1);
	m_layout->addWidget(m_colorButton, 1, 2);
	
	m_createButton = new QPushButton(QStringLiteral("Create"));
	m_layout->addWidget(m_createButton, 2,0,1,3);
	m_color = Qt::black;
	m_colorDisplay->setStyleSheet(QString("background-color:rgb(0,0,0)"));

	connect(m_colorButton, &QPushButton::clicked, [this]()
	{
		m_color = QColorDialog::getColor(Qt::black, this, QStringLiteral("Color"),QColorDialog::DontUseNativeDialog);
		m_colorDisplay->setStyleSheet(QString("background-color:rgb(%1,%2,%3)").arg(m_color.red()).arg(m_color.green()).arg( m_color.blue()));
	});
	connect(m_createButton, &QPushButton::clicked, this, &MarkCategoryDialog::createButtonClick);
	setLayout(m_layout);
}

void MarkCategoryDialog::createButtonClick()
{
	if(m_color.isValid() == false)
	{
		QMessageBox::critical(this, QStringLiteral("Error"), QStringLiteral("Please choose a color"));
		return;
	}
	QString name = m_name->text();
	if(name.isEmpty() == true)
	{
		QMessageBox::critical(this, QStringLiteral("Error"), QStringLiteral("Please input a name"));
		return;
	}
	emit resultReceived(name, m_color);
	close();
}
