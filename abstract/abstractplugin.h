#ifndef ABSTRACTPLUGIN_H
#define ABSTRACTPLUGIN_H

#include <QWidget>
//#include "imageviewer.h"
enum class SliceType;
class SliceItem;
class AbstractSliceDataModel;
class SliceWidget;
class AbstractPlugin:public QWidget
{
public:
    AbstractPlugin(SliceType type,const QString & name,SliceWidget * view = nullptr, AbstractSliceDataModel * model = nullptr, QWidget * parent = nullptr);
	virtual  ~AbstractPlugin(){}
protected slots:
	virtual void sliceChanged(int index);
	virtual void sliceSelected(const QPoint & pos);
	virtual void sliceOpened(int index);
	virtual void slicePlayStoped(int index);
	virtual void slicePlaying(int index);
protected:
	SliceItem * sliceItem();
	QImage originalImage(int index);
	SliceWidget * view();
	QString sliceName()const;
private:
	Q_OBJECT
	AbstractSliceDataModel * m_model;
	SliceWidget *m_view;
	SliceType m_type;
	QString m_sliceName;

	friend class SliceEditorWidget;
};

#endif // ABSTRACTPLUGIN_H