#ifndef ABSTRACTPLUGIN_H
#define ABSTRACTPLUGIN_H

#include <QWidget>
//#include "imageviewer.h"

enum class SliceType;
class SliceItem;
class AbstractSliceDataModel;
class SliceWidget;
class SliceEditorWidget;

class AbstractPlugin:public QWidget
{
public:
    AbstractPlugin(SliceEditorWidget * sliceEditor, QWidget * parent = nullptr);
	virtual  ~AbstractPlugin(){}

protected:

	int currentIndex(SliceType type)const;

	SliceItem * sliceItem(SliceType type)const;

	QImage originalImage(SliceType type, int index);

	QImage image(SliceType type, int index);

	void setImage(SliceType type,int index, const QImage & image);

	void setCurrentImage(SliceType type,const QImage & image);

	QImage currentImage(SliceType type);

	SliceWidget * view(SliceType type);

protected slots:
	virtual void updateDataModel();

private:

	Q_OBJECT

	void setSliceEditor(SliceEditorWidget * widget);
	SliceEditorWidget * m_sliceEditor;
};

#endif // ABSTRACTPLUGIN_H