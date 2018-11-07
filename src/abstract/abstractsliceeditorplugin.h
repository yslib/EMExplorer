#ifndef ABSTRACTPLUGIN_H
#define ABSTRACTPLUGIN_H

#include <QWidget>

enum class SliceType;


class SliceItem;
class AbstractSliceDataModel;
class SliceWidget;
class SliceEditorWidget;

class AbstractSliceEditorPlugin:public QWidget
{
public:
    AbstractSliceEditorPlugin(SliceEditorWidget * sliceEditor, QWidget * parent = nullptr);
	virtual  ~AbstractSliceEditorPlugin() = default;

	//AbstractSliceEditorPlugin(const AbstractSliceEditorPlugin &) = delete;
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
	virtual void updateDataModel();		//Is signal better?
private:
	Q_OBJECT
	void setSliceEditor(SliceEditorWidget * widget);
	SliceEditorWidget * m_sliceEditor;
};


class AbstractSliceViewPlugin:public AbstractSliceEditorPlugin {
public:
	AbstractSliceViewPlugin(SliceType type, SliceEditorWidget * sliceEditor, QWidget * parent = nullptr);
protected:
	virtual SliceType sliceType()const;
private:
	SliceType m_sliceType;
};



#endif // ABSTRACTPLUGIN_H
