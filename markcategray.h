#ifndef MARKCATEGRAY_H
#define MARKCATEGRAY_H

#include <QWidget>
#include <QDialog>
QT_BEGIN_NAMESPACE
class QLabel;
class QLineEdit;
class QPushButton;
class QColor;
class QGridLayout;
QT_END_NAMESPACE

class MarkCategray : public QDialog
{
public:
	explicit MarkCategray(QWidget *parent = nullptr);
signals:
	void resultReceived(const QString & name, const QColor & color);
private slots:
	void createButtonClick();
private:
	Q_OBJECT
	//Widgets
	QLabel * m_nameLabel;
	QLineEdit * m_name;
	QLabel * m_colorLabel;
	QLineEdit * m_colorDisplay;
	QPushButton * m_colorButton;
	QPushButton * m_createButton;

	//Layout
	QGridLayout * m_layout;

	//Data
	QColor m_color;

};

#endif // MARKCATEGRAY_H
