#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QList>
#include <QWidget>
#include <QGraphicsScene>
#include <QItemSelection>

enum class SliceType;
QT_BEGIN_NAMESPACE
class QGridLayout;
class QLabel;
class QWheelEvent;
class QGraphicsView;
class QGraphicsScene;
class QMouseEvent;
class QGraphicsSceneMouseEvent;
class QGraphicsSceneWheelEvent;
class QAbstractItemModel;
class QGraphicsItem;
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
class ItemContext;
class MarkModel;
class HistogramWidget;
class PixelWidget;
class MRC;
class AbstractSliceEditorPlugin;
class AbstractSliceDataModel;
class SliceItem;
class SliceWidget;
class RenderWidget;
class CategoryControlWidget;
class CategoryInfo;
class StrokeMarkItem;

class SliceScene :public QGraphicsScene
{
public:
	SliceScene(QObject * parent = nullptr);
protected:
	void mousePressEvent(QGraphicsSceneMouseEvent* event) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QGraphicsSceneMouseEvent* event) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) Q_DECL_OVERRIDE;
	void wheelEvent(QGraphicsSceneWheelEvent* event) Q_DECL_OVERRIDE;
};


struct SliceState {
	int currentTopSliceIndex;
	int currentRightSliceIndex;
	int currentFrontSliceIndex;
	QString currentCategory;
	SliceState() :currentTopSliceIndex(0)
		, currentRightSliceIndex(0)
		, currentFrontSliceIndex(0)
	{}
};

class SliceEditorWidgetPrivate {
public:
	SliceEditorWidgetPrivate():
	state(new SliceState)
	{}
	QScopedPointer<SliceState> state;
};

class SliceEditorWidget :public QWidget
{
	Q_OBJECT
public:
	SliceEditorWidget(QWidget * parent = nullptr,
		bool topSliceVisible = true, 
		bool rightSliceVisible = true,
		bool frontSliceVisible = true,
		AbstractSliceDataModel * model = nullptr);

	bool eventFilter(QObject* watched, QEvent* event) override;
	bool topSliceVisible() const;
	bool rightSliceVisible() const;
	bool frontSliceVisible() const;
	QPen pen()const;
	void setPen(const QPen & pen);
	void setTopSliceVisible(bool enable);
	void setRightSliceVisible(bool enable);
	void setFrontSliceVisible(bool enable);
	void setSliceVisible(SliceType type, bool visible);
	int  currentSliceIndex(SliceType type)const;
	void setSliceIndex(SliceType type, int index);

	QString currentCategory()const;
	void setCurrentCategory(const QString & name);
	bool addCategory(const CategoryInfo & info) const;
	QStringList categories() const;

	SliceWidget * topView()const;
	SliceWidget * rightView()const;
	SliceWidget * frontView()const;
	
	void resetZoom(bool check);
	void zoomIn();
	void zoomOut();
	void setOperation(SliceType type,int opt);

	void setTopSliceVisibility(bool check);
	void setFrontSliceVisibility(bool check);
	void setRightSliceVisibility(bool check);

	void deleteSelectedMarks();

	AbstractSliceDataModel* takeSliceModel(AbstractSliceDataModel* model);
	inline AbstractSliceDataModel * sliceModel()const;

	MarkModel* takeMarkModel(MarkModel* model, bool * success)noexcept;
	MarkModel* markModel();
    ~SliceEditorWidget()override;
signals:

	/**
	 * \brief This signal is emitted when top slice changed
	 * \param index The new current top slice index
	 */
	void topSliceChanged(int index);

	/**
	 * \brief This signal is emitted when right slice changed
	 * \param index The new current right slice index
	 */
	void rightSliceChanged(int index);

	/**
	 * \brief This signal is emitted when front slice changed
	 * \param index The new current front slice index
	 */
	void frontSliceChanged(int index);

	/**
	 * \brief The signal is emitted when slice data model changed
	 */
	void dataModelChanged();

	/**
	 * \brief The signal is emitted when mark model changed
	 */
	void markModelChanged();

	/**
	 * \brief The signal is emitted when the top slice is selected
	 * \param point The selected position on the selected slice
	 */
	void topSliceSelected(const QPoint & point);

	/**
	 * \brief The signal is emitted when the right slice is selected
	 * \param point The selected position on the selected slice
	 */
	void rightSliceSelected(const QPoint & point);

	/**
	 * \brief The signal is emitted when the right slice is selected
	 * \param point The selected position on the selected slice
	 */
	void frontSliceSelected(const QPoint & point);
	
	/**
	 * \brief The signal is emitted when focus enters into on of three slice widget
	 * 
	 * The parameter \a type indicates type of the focused slice
	 * 
	 * \sa SliceType
	 */
	void viewFocus(SliceType type);

	/**
	 * \brief The signal is emitted when a mark in one of three slice widget is selected
	 * 
	 * The first selected mark will be the given as \a item
	 */
	void markSelected(StrokeMarkItem * item);

	/**
	 * \brief This signals is emitted when marks in one of three slice widget are selected.
	 * All selected marks will be given as \a items
	 */
	void marksSelected(QList<StrokeMarkItem*> items);

private slots:

	void _slot_markSelected(StrokeMarkItem * mark);

	void _slot_currentChanged_selectionModel(const QModelIndex & current, const QModelIndex & previous);

	void _slot_selectionChanged_selectionModel(const QItemSelection & selected, const QItemSelection & deselected);

	void _slot_topViewSliceSelection(const QPoint & pos);

	void _slot_rightViewSliceSelection(const QPoint & pos);

	void _slot_frontViewSliceSelection(const QPoint & pos);
private:

	/**
	 * \brief This is a private enum.
	 * 
	 * The enum flags keep the play direction used for slice playing
	 */
	enum class PlayDirection {
		Forward,				///< Forward play
		Backward				///< Backward play
	};

	void updateMarks(SliceType type);
	void updateActions();
	void installMarkModel(MarkModel* model);
	void markAddedHelper(SliceType type, StrokeMarkItem* mark);
	void _slots_singleMarkSelectionChanged();

	//void changeSliceHelper(int value, SliceType type);
	SliceWidget * focusOn();
	static MarkModel * createMarkModel(SliceEditorWidget * view, AbstractSliceDataModel * data);

	QModelIndex _hlp_categoryIndex(const QString& category)const;
	QModelIndex _hlp_categoryAdd(const CategoryInfo & info) const;						

	QModelIndex _hlp_instanceFind(const QString & category, const StrokeMarkItem * item);
	QModelIndex _hlp_instanceAdd(const QString & category, const StrokeMarkItem* mark);
	QStringList categoryText()const;

	bool removeMark(StrokeMarkItem* mark);
	int removeMarks(const QList<StrokeMarkItem*>& marks = QList<StrokeMarkItem*>());

	// Data Members
	SliceEditorWidgetPrivate * const d_ptr;
	Q_DECLARE_PRIVATE(SliceEditorWidget);
	AbstractSliceDataModel * m_sliceModel;
	MarkModel * m_markModel;
	//main layout
	QGridLayout *m_layout;
	SliceWidget * m_topView;
	SliceWidget * m_rightView;
	SliceWidget * m_frontView;
	QCheckBox* m_thumbnailCheckBox;
};

/**
 * \brief Returns the top slice model
 * 
 * \sa AbstractSliceDataModel
 */
inline AbstractSliceDataModel * SliceEditorWidget::sliceModel()const { return m_sliceModel; }

/**
 * \brief Returns the top slice widget
 * 
 * \sa SliceWidget
 */
inline SliceWidget * SliceEditorWidget::topView()const { return m_topView; }

/**
 * \brief Returns the right slice widget
 *
 * \sa SliceWidget
 */
inline
inline SliceWidget * SliceEditorWidget::rightView()const { return m_rightView; }

/**
 * \brief Returns the front slice widget
 *
 * \sa SliceWidget
 */
inline
inline SliceWidget * SliceEditorWidget::frontView()const { return m_frontView; }

#endif // IMAGEVIEWER_H
