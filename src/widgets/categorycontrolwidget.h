#ifndef IMAGEVIEWCONTROLPANEL_H
#define IMAGEVIEWCONTROLPANEL_H

#include <QWidget>

enum class SliceType;
QT_BEGIN_NAMESPACE
class QLabel;
class QWheelEvent;
class QToolBar;
class QMenu;
class QGroupBox;
class QToolButton;
class QCheckBox;
class QComboBox;
class QPushButton;
class QTimer;
QT_END_NAMESPACE
class TitledSliderWithSpinBox;
class SliceEditorWidget;
class MarkModel;
class AbstractSliceDataModel;

/**
 * \brief This class is used for adding new mark category for SliceEditorWidget.
 */

class CategoryControlWidget:public QWidget
{
	Q_OBJECT
public:
    CategoryControlWidget(SliceEditorWidget * canvas, QWidget * parent = nullptr);
	void setImageCanvas(SliceEditorWidget * canvas);
	QString currentCategoryName()const;
	QColor currentCategoryColor()const;
	int categoryCount()const;
private:
	void createWidgets();

	void connections();

	void onMarkModelChanged();

	void setCategoryInfoPrivate(const QVector<QPair<QString, QColor>>& cates);

	void addCategoryInfoPrivate(const QString & name, const QColor & color);

	QIcon createColorIcon(const QColor & color);

	void onCategoryAdded();

	void colorChanged();



	enum class PlayDirection 
	{
		Forward,
		Backward
	};
	SliceEditorWidget * m_canvas = nullptr;
	MarkModel * m_markModel = nullptr;
	
	//Widgets on edit toolbar
	QLabel * m_categoryLabel = nullptr;
	QComboBox * m_categoryCBBox = nullptr;
	QLabel * m_penSizeLabel= nullptr;
	QComboBox* m_penSizeCBBox= nullptr;
	QToolButton *m_colorAction=nullptr;
	QToolButton *m_addCategoryAction=nullptr;

	//friend class SliceEditorWidget;
};

#endif // IMAGEVIEWCONTROLPANEL_H