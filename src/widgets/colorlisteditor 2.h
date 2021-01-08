#ifndef COLORLISTEDITOR_H
#define COLORLISTEDITOR_H

#include <QComboBox>

QT_BEGIN_NAMESPACE
class QColor;
class QWidget;
QT_END_NAMESPACE

class ColorListEditor : public QComboBox
{
    Q_OBJECT
public:
    ColorListEditor(QWidget *widget = nullptr);
public:
    QColor color() const;
    void setColor(QColor c);
private:
    void populateList();
};




#endif
