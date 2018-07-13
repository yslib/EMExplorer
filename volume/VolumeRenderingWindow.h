#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ModelData.h"

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
QT_END_NAMESPACE

class VolumeRenderWidget;
class BasicControlWidget;
class TF1DEditor;
class TF2DEditor;
class Volume;

class VolumeRenderingWindow : public QMainWindow, public ModelData
{
	Q_OBJECT

public:
	VolumeRenderingWindow(QWidget *parent = 0);
	~VolumeRenderingWindow();

	// show message in status bar
	void showStatusMessage(QString message);

// ModelData Interface
public:
	// volume
	virtual Volume* getVolume();
	// volume name
	virtual string  getVolumeName();
	// 1D transfer function
	virtual void getTransferFunction(float* transferFunction, size_t dimension, float factor);
	virtual unsigned int getTF1DTextureIdx();
	// 2D transfer function
	virtual void getTransferFunction(float* transferFunction, size_t width, size_t height, float factor);

// Actions
private slots:
	void openVolumeData();
	void about();
	void loadVolumeData(const QString &fileName);

private:
	// create main window
	void createActions();
	void createConnections();
	void createMenus();
	void createToolBars();
	void createStatusBar();	
	void createDockWindows();

	// display properties of main window
	void readSettings();
	void writeSettings();

	// load volume data and TF1D
	void setCurrentFile(const QString &fileName);
	QString strippedName(const QString &fullFileName);

private:
	// data
	VolumeRenderWidget *renderWidget;
	BasicControlWidget *controlWidget;
	TF1DEditor *editorTF1D;
	TF2DEditor *editorTF2D;
	QString curDataFile;
	Volume *volumeData;

	QMenu *fileMenu;
	QMenu *viewselMenu;
	QMenu *shaderMenu;
	QMenu *viewMenu;
	QMenu *helpMenu;

	QAction *openAct;
	QAction *exitAct;
	QAction *viewAct;
	QAction *MIPAct;
	QAction *TF1DAct;
	QAction *TF1DShadingAct;
	QAction *TF2DShadingAct;
	QAction *boundingBoxAct;
	QAction *defaultViewAct;
	QAction *positiveXViewAct;
	QAction *negativeXViewAct;
	QAction *positiveYViewAct;
	QAction *negativeYViewAct;
	QAction *positiveZViewAct;
	QAction *negativeZViewAct;
	QAction *aboutAct;

private:
	//Ui::MainWindowClass ui;
};

#endif // MAINWINDOW_H
