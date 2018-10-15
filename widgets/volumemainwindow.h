#ifndef VOLUMEMAINWINDOW_H
#define VOLUMEMAINWINDOW_H

#include <QMainWindow>
#include "slicecontrolwidget.h"

class RenderWidget;
class RenderParameterWidget;
class SliceControlWidget;



class VolumeMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit VolumeMainWindow(QWidget *parent = nullptr);
	void open(const QString & fileName);
	void openMark(const QString & fileName);
private:
	RenderWidget * m_volumeWidget;
	RenderParameterWidget * m_volumeControlWidget;
	SliceControlWidget * m_sliceControlWidget;
	QDockWidget * m_controlDockWidget;
	QToolBar * m_toolBar;
};

#endif // VOLUMEMAINWINDOW_H