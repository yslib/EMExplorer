#ifndef SLICEMAINWINDOW_H
#define SLICEMAINWINDOW_H

#include <QMainWindow>

class SliceEditorWidget;
class CategoryControlWidget;
class SliceControlWidget;
class QToolButton;

class SliceMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit SliceMainWindow(QWidget *parent = nullptr);
	void open(const QString & fileName);
	void openMark(const QString & fileName);

private slots:
	void pixelViewActionTriggered();
	void histogramViewActionTriggered();
private:
	SliceEditorWidget * m_sliceEditorWidget;
	SliceControlWidget * m_sliceControlWidget;
	QDockWidget * m_controlDockWidget;
	QToolBar* m_toolBar;

	QAction * m_sliceControlViewToggleAction;
	QToolButton *m_zoomInAction;
	QToolButton *m_zoomOutAction;
	QToolButton * m_resetAction;

	QToolButton *m_pixelViewAction;
	QToolButton *m_histogramAction;

};

#endif // SLICEMAINWINDOW_H