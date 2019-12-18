#ifndef WIDGETPLUGININTERFACE_H
#define WIDGETPLUGININTERFACE_H

#include <QObject>

class WidgetPluginInterface:public QObject
{

public:
    WidgetPluginInterface();
    virtual QWidget * createWidget(QWidget * parent) = 0;
    virtual ~WidgetPluginInterface();
};

Q_DECLARE_INTERFACE(WidgetPluginInterface,"com.none.MRCEditor.WidgetPluginInterface/1.0");

class SliceWidgetPluginInterface:public WidgetPluginInterface
{

public:
    SliceWidgetPluginInterface();
    QWidget * createWidget(QWidget * parent)=0;

    ~SliceWidgetPluginInterface();
};

#endif // WIDGETPLUGININTERFACE_H
