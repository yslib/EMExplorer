#include <QWheelEvent>
#include <QDebug>
#include<queue>

#include "slicewidget.h"
#include "globals.h"
#include "model/sliceitem.h"
#include "model/markitem.h"
#include "model/markmodel.h"

#include <chrono>

#define cimg_display 0 //
#define cimg_OS 0
#include "algorithm/CImg.h"
//#define DEBUG_MARK_ERASE



SliceWidget::SliceWidget(QWidget *parent) :QGraphicsView(parent),
//m_scaleFactor(0.5),
m_currentPaintingSlice(nullptr),
m_paint(false),
m_selection(false),
m_pen(QPen(Qt::black, 5, Qt::SolidLine)),
m_slice(nullptr),
m_paintingItem(nullptr),
m_state(Operation::None),
m_anchorItem(nullptr),
m_paintNavigationView(false)
{
	setScene(new QGraphicsScene(this));
	connect(scene(), &QGraphicsScene::selectionChanged, this, &SliceWidget::selectionChanged);
	setTransformationAnchor(QGraphicsView::NoAnchor);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setDragMode(QGraphicsView::RubberBandDrag);
	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	const auto pixel = createAnchorItemPixmap();
	m_anchorItem = new QGraphicsPixmapItem(pixel);
	m_anchorItem->setVisible(false);
	setStyleSheet(QStringLiteral("border:0px solid white"));

	is_draw_new_mark = false;
	enable_intelligent_scissor = false;
	pause_intelligent_scissor = false;
	tempAuxiliaryLine = nullptr;
}

void SliceWidget::setMarks(const QList<StrokeMarkItem*>& items)
{
	setMarkHelper(items);
}
void SliceWidget::wheelEvent(QWheelEvent *event) {
	event->accept();
}
void SliceWidget::focusInEvent(QFocusEvent* event)
{
	Q_UNUSED(event);
	setStyleSheet(QStringLiteral("border:1px solid red"));
	emit sliceSelected();
}
void SliceWidget::focusOutEvent(QFocusEvent* event)
{
	Q_UNUSED(event);
	setStyleSheet(QStringLiteral("border:0px solid white"));
}

void SliceWidget::paintEvent(QPaintEvent* event)
{
	QGraphicsView::paintEvent(event);

	if (enable_intelligent_scissor && m_state == Operation::Paint)
	{
		QPainter p(this->viewport());
		QFont font("黑体", 15, QFont::Bold,true);
		p.setFont(font);

		if (!pause_intelligent_scissor)
		{
			p.drawText(10, 20, QStringLiteral("AUTO"));
		}
		else
		{
			p.drawText(10, 20, QStringLiteral("MANUAL"));
		}
	}

	if (!m_paintNavigationView)
		return;

	const auto & sliceRectInScene = m_slice->mapRectToScene(m_slice->boundingRect());
	const auto & viewRectInScene = mapToScene(rect()).boundingRect();
	if (viewRectInScene.contains(sliceRectInScene))
		return;
	//const auto & scRect = sceneRect();
	const auto scRect = m_slice->mapRectToScene(m_slice->boundingRect());
	QImage thumbnail(QSize(ThumbnailLength, ThumbnailLength), QImage::Format_RGBA8888_Premultiplied);
	QPainter p0(&thumbnail);
	//render(&p0, thumbnail.rect(), mapFromScene(scRect).boundingRect());		//rendering the scene image
	p0.drawPixmap(thumbnail.rect(), m_slice->pixmap());
	p0.end();
	const auto & navigationRect = thumbnail.rect();
	const auto f1 = navigationRect.width() / scRect.width(), f2 = navigationRect.height() / scRect.height();
	QPainter p(&thumbnail);

	p.setPen(QPen(Qt::red, 2));			// TODO:: There may be a bug

	p.drawRect(QRect(
		f1 * (viewRectInScene.x() - scRect.x()),			//transform from view rectangle to thumbnail rectangle
		f2 * (viewRectInScene.y() - scRect.y()),
		f1 * viewRectInScene.width(),
		f2 * viewRectInScene.height()));			//draw the zoom rectangle onto the thumbnail

	p.drawRect(thumbnail.rect());
	p.end();
	QPainter p2(this->viewport());//draw the zoom image
	const auto s = size();

	p2.drawPixmap(thumbnailRect(sliceRectInScene, viewRectInScene), QPixmap::fromImage(thumbnail));
	p2.end();

}

void SliceWidget::mouseDoubleClickEvent(QMouseEvent* event) // 开启智能剪刀时，结束绘制一个mark 准备绘制新的
{
	if (m_state == Operation::Paint && enable_intelligent_scissor && is_draw_new_mark) {

		// 删除shortest path辅助线
		if (tempAuxiliaryLine != nullptr)
		{
			delete tempAuxiliaryLine;
			tempAuxiliaryLine = nullptr;
		}

		//删除temp path辅助线
		if (m_paintingItem != nullptr)
		{
			delete m_paintingItem;
			m_paintingItem = nullptr;
		}

		//构建结果图形并绘制
		m_resultItem = new StrokeMarkItem(m_currentPaintingSlice);
		m_resultItem->setFlags(QGraphicsItem::ItemIsSelectable);
		m_resultItem->setPen(m_pen);
		
		for (int i = 0; i < path.length(); i++)
		{
			auto temp = path[i];
			for (int i = 0; i < temp.length(); i++)
			{
				m_resultItem->appendPoint(temp[i]);
			}
		}

		//首尾相连
		if (path[0].last() != path.last().last())
		{
			m_resultItem->appendPoint(path[0].last());
		}

		if (m_resultItem->polygon().size() > 3)
			emit markAdded(m_resultItem);
		else
			delete m_resultItem;

		path.clear();
		m_paintViewPointsBuffer.clear();
		auxiliaryLinePath.clear();
		is_draw_new_mark = false;

		m_currentPaintingSlice = nullptr;
	}
	event->accept();
	return;
}

void SliceWidget::keyPressEvent(QKeyEvent* event) //撤回
{
	if ((event->modifiers() == Qt::ControlModifier) && (event->key() == Qt::Key_Z))
	{
		if (m_state == Operation::Paint)
		{
			if (m_paintViewPointsBuffer.length()>1)
			{
				m_paintViewPointsBuffer.pop_back();

				//删除之间画的那一段
				path.pop_back();

				//重新绘制
				if (m_paintingItem != nullptr)
					delete m_paintingItem;

				if (tempAuxiliaryLine != nullptr)
				{
					delete tempAuxiliaryLine;
					tempAuxiliaryLine = nullptr;
				}

				m_paintingItem = new StrokeMarkItem(m_currentPaintingSlice);
				m_paintingItem->setFlags(QGraphicsItem::ItemIsSelectable);
				QPen pen(m_pen.color(), 5, Qt::DotLine, Qt::RoundCap, Qt::RoundJoin);

				m_paintingItem->setPen(pen);
				
				for (int i = 0; i < path.length(); i++)
				{
					auto temp = path[i];
					for (int i = 0; i < temp.length(); i++)
					{
						m_paintingItem->appendPoint(temp[i]);
					}
				}
			}
		}
	}
	else if (event->key() == Qt::Key_Space) 
	{
		if (m_state == Operation::Paint)
		{
			pause_intelligent_scissor = !pause_intelligent_scissor; //按空格切换手动和辅助模式
			
			// 删除shortest path辅助线
			if (tempAuxiliaryLine != nullptr)
			{
				delete tempAuxiliaryLine;
				tempAuxiliaryLine = nullptr;
			}

			if (pause_intelligent_scissor) setMouseTracking(false);
			else setMouseTracking(true);
			update();
		}
	}
	event->accept();
	return;

}

void SliceWidget::mousePressEvent(QMouseEvent *event)
{
	//if (event->isAccepted())
	//	return;
	const auto button = event->button();
	const auto viewPos = event->pos();
	const auto scenePos = mapToScene(viewPos);
	
	m_prevViewPoint = viewPos;
	auto items = scene()->items(scenePos);

	if (button == Qt::RightButton) { // ignore right button event
		
		event->accept();
		return;
	}

	for (const auto & item : items) {
		auto * itm = qgraphicsitem_cast<SliceItem*>(item);
		if (itm == m_slice) {	 // Operations must perform on slice item.

			//if (button == Qt::RightButton) {			// ignore right button event
			//	event->accept();
			//	return;
			//}

			const auto itemPoint = m_slice->mapFromScene(scenePos);
			emit sliceSelected(itemPoint.toPoint());

			if (m_state == Operation::Paint) {
				if (enable_intelligent_scissor)
				{
					if (!is_draw_new_mark) //刚开始绘制新的mark
					{
						is_draw_new_mark = true;

						m_currentPaintingSlice = m_slice;
						m_paintingItem = new StrokeMarkItem(m_currentPaintingSlice);
						m_paintingItem->setFlags(QGraphicsItem::ItemIsSelectable);
						QPen pen(m_pen.color(), 5, Qt::DotLine, Qt::RoundCap, Qt::RoundJoin);
						m_paintingItem->setPen(pen);

						auto snap_point = snapPoint(itemPoint.toPoint());//对用户点击到的区域周围14X14的像素搜索，snap到附近梯度最小的点				
						m_paintingItem->appendPoint(snap_point); 

						m_paintViewPointsBuffer.push_back(snap_point);

						QVector<QPoint> tempPath;
						tempPath.push_back(snap_point);
						path.push_back(tempPath);

					}
					else //再先前的mark上继续添加线段
					{
						if (!pause_intelligent_scissor)
						{
							QVector<QPoint> tempPath;
							for (auto i = auxiliaryLinePath.length() - 1; i > 0; i--) //将确定的线段加入到目标中
							{
								m_paintingItem->appendPoint(auxiliaryLinePath[i]);
								tempPath.push_back(auxiliaryLinePath[i]);
							}
							path.push_back(tempPath);

							m_paintViewPointsBuffer.push_back(snapPoint(itemPoint.toPoint())); //对用户点击到的区域周围14X14的像素搜索，snap到附近梯度最小的点
						}
						else
						{
							if (m_currentPaintingSlice != nullptr)
							{
								m_paintingItem->appendPoint(itemPoint.toPoint());
								auxiliaryLinePath.clear();
								auxiliaryLinePath.push_back(itemPoint.toPoint());//用auxiliaryLinePath来存用户手绘的那一段
							}
						}
					}
					
				}
				else
				{
					m_currentPaintingSlice = m_slice;
					m_paintingItem = new StrokeMarkItem(m_currentPaintingSlice);
					m_paintingItem->setFlags(QGraphicsItem::ItemIsSelectable);
					m_paintingItem->setPen(m_pen);
					m_paintingItem->appendPoint(itemPoint);
				}
				
				event->accept();
				return;
			}
			else if (m_state == Operation::Selection) {
				// Selecting items automatically by calling default event handler of the QGraphicsView
				return QGraphicsView::mousePressEvent(event);
			}
			else if (m_state == Operation::Erase) {
				m_erasingMarks.clear();
				return;
			}
			else if (m_state == Operation::None) {
				// Set Anchor
				const auto siz = m_anchorItem->pixmap().size();
				const auto originItemPoint = QPointF(siz.width() / 2.0, siz.height() / 2.0);
				m_anchorItem->setPos(itemPoint - originItemPoint);
				m_anchorItem->setVisible(true);
				event->accept();
				return;
			}
		}
	}
	event->accept();
	return;
}

void SliceWidget::mouseMoveEvent(QMouseEvent *event)
{

	// Note that the returned value for event->button() is always Qt::NoButton for mouse move events.
	const auto viewPos = event->pos();
	//const auto scenePos = mapToScene(viewPos);
	if (m_paintNavigationView && m_state != Operation::Paint) {			// move the navigation rect with mouse
		const auto & sliceRectInScene = m_slice->mapRectToScene(m_slice->boundingRect());
		const auto & viewRectInScene = mapToScene(rect()).boundingRect();
		const auto tRect = thumbnailRect(sliceRectInScene, viewRectInScene);
		if (tRect.contains(viewPos)) {
			// Mouse click in thumbnail, mapping the click position to real slice position

			const auto imageSize = m_slice->pixmap().size();
			const int x = static_cast<double>(viewPos.x() - tRect.left()) / (0.2*width())*imageSize.width();
			const int y = static_cast<double>(viewPos.y() - tRect.top()) / (0.2*height())*imageSize.height();
			centerOn(m_slice->mapToScene(x, y));
			return;
		}
	}


	if (event->buttons() == Qt::RightButton) {		// Move
		const auto delta = viewPos - m_prevViewPoint;
		m_prevViewPoint = viewPos;
		translate(delta.x(), delta.y());
		emit viewMoved(delta);
		event->accept();
		return;
	}
	else if(m_state == Operation::Paint)		// Drawing a mark
	{
		if (enable_intelligent_scissor)
		{
			if (!pause_intelligent_scissor)
			{
				if (m_paintViewPointsBuffer.length() > 0)
				{
					if (tempAuxiliaryLine != nullptr)
						delete tempAuxiliaryLine;

					QPen pen;
					QVector<qreal> dashes;
					qreal space = 3;
					dashes << 5 << space << 5 << space;
					pen.setDashPattern(dashes);
					pen.setWidth(2);
					pen.setColor(m_pen.color());

					tempAuxiliaryLine = new StrokeMarkItem(m_slice);
					tempAuxiliaryLine->setPen(pen);
					auxiliaryLinePath.clear();

					auto width = m_slice->pixmap().width();
					auto height = m_slice->pixmap().height();
					auto pos_x = m_slice->mapFromScene(mapToScene(viewPos)).toPoint().x();
					auto pos_y = m_slice->mapFromScene(mapToScene(viewPos)).toPoint().y();

					auto delta = abs(m_paintViewPointsBuffer.last().x() - pos_x) + abs(m_paintViewPointsBuffer.last().y() - pos_y);
					if (pos_x > 0 && pos_x < width && pos_y>0 && pos_y < height && delta < 256) // border checking && if the delta more than 256 stop generate path
					{
						auxiliaryLinePath = getShortestPath(m_paintViewPointsBuffer.last(), snapPoint(QPoint(pos_x, pos_y)));

						for (auto i = auxiliaryLinePath.length() - 1; i > 0; i--)
						{
							tempAuxiliaryLine->appendPoint(auxiliaryLinePath[i]);
						}
					}

				}
			}
			else
			{
				if (m_currentPaintingSlice != nullptr)
				{
					auto itemPoint = m_currentPaintingSlice->mapFromScene(mapToScene(viewPos)).toPoint();
					m_paintingItem->appendPoint(itemPoint);
					auxiliaryLinePath.push_back(itemPoint);
				}
			}
		
		}
		else
		{
			if (m_currentPaintingSlice != nullptr)
			{
				m_paintingItem->appendPoint(m_currentPaintingSlice->mapFromScene(mapToScene(viewPos)));
			}
		}
		event->accept();
		return;
		
	}
	else if (m_state == Operation::Selection)
	{
		return QGraphicsView::mouseMoveEvent(event);
	}
	else if (m_state == Operation::Erase)
	{
		const auto scPos = mapToScene(viewPos);
		auto items = scene()->items(scPos);
		for (const auto i : items) {
			auto m = qgraphicsitem_cast<StrokeMarkItem*>(i);
			if (m)
			{
				const auto itr = m_erasingMarks.find(m);
				if(itr == m_erasingMarks.end()) {
					m_erasingMarks << m;
					m->beginErase();
#ifdef DEBUG_MARK_ERASE
					qDebug() << "Erasing:" << m;
#endif
				}
				m->erase(m->mapFromScene(scPos), 10);
			}
		}
		return;
	}

	QGraphicsView::mouseMoveEvent(event);
}

void SliceWidget::mouseReleaseEvent(QMouseEvent *event)
{
	if (m_state == None)
		return;

	if (event->button() == Qt::RightButton) {
		event->accept();
		return;
	}

	Qt::MouseButton button = event->button();

	if (m_state == Operation::Paint)			//create a mark
	{

		if (!enable_intelligent_scissor)
		{
			if (m_currentPaintingSlice == nullptr)
			{
				event->accept();
				return;
			}

			m_paintingItem->appendPoint(m_currentPaintingSlice->mapFromScene(mapToScene(event->pos())));
			if (m_currentPaintingSlice == m_slice) {
				if (m_paintingItem->polygon().size() > 3)
					emit markAdded(m_paintingItem);
				else
					delete m_paintingItem;
			}
			
			m_currentPaintingSlice = nullptr;
		}
		else if(pause_intelligent_scissor)
		{
			if (m_currentPaintingSlice == nullptr)
			{
				event->accept();
				return;
			}

			auxiliaryLinePath.push_back(m_currentPaintingSlice->mapFromScene(mapToScene(event->pos())).toPoint());
			if (auxiliaryLinePath.length() > 3)
			{
				path.push_back(auxiliaryLinePath);
				m_paintViewPointsBuffer.push_back(auxiliaryLinePath.last());
				m_paintingItem->appendPoint(auxiliaryLinePath.last());
			}
		}
		
		event->accept();
		return;
	}
	else if (m_state == Operation::Erase)
	{

		for (const auto m : m_erasingMarks)
		{
			bool empty;
			auto residues = m->endErase(true, &empty);

#ifdef DEBUG_MARK_ERASE
			qDebug() << "mouseReleaseEvent: End Erase: Residue count:" << residues.size() << " Empty:"<<empty;
#endif

			for (const auto &n : residues)
			{
				emit markAdded(n);
				n->setFlags(QGraphicsItem::ItemIsSelectable);
				n->setPen(m_pen);
			}
			if (empty)
			{
				QModelIndex index = m->modelIndex();
				auto model = const_cast<QAbstractItemModel*>(index.model());
				// Temporally
				model->removeRow(index.row(), model->parent(index));
			}
		}

		event->accept();
		return;
	}

	QGraphicsView::mouseReleaseEvent(event);
}

QRect SliceWidget::thumbnailRect(const QRectF & sliceRect, const QRectF & viewRect)const
{
	//const auto w = 0.2*width(), h = 0.2*height();
	auto s = sliceRect.size().toSize();
	s.scale(width()*0.2, height()*0.2, Qt::KeepAspectRatio);

	const auto W = width(), H = height();

	//if(sliceRect.contains(viewRect))
	//{
	//	return QRect(0,0,w,h);
	//}else if(sliceRect.contains(viewRect.topLeft()))
	//{
	//	return QRect(W-w,H-h,w,h);
	//}else if(sliceRect.contains(viewRect.bottomLeft()))
	//{
	//	return QRect(W-w,0,w,h);
	//}else if(sliceRect.contains(viewRect.topRight()))
	//{
	//	
	//}else if(sliceRect.contains(viewRect.bottomRight()))
	//{
	//	return QRect(0, 0, w, h);
	//}
	//return QRect(0,0,w,h);


	return { 0,int(H - s.height()),s.width(),s.height() };
}

QGraphicsItem * SliceWidget::createMarkItem()
{
	return nullptr;
}

QPixmap SliceWidget::createAnchorItemPixmap(const QString & fileName)
{
	const auto length = 12;
	QRect target(0, 0, length, length);

	QPixmap pixmap(target.size());
	pixmap.fill(Qt::transparent);
	QPainter painter(&pixmap);

	if (fileName.isEmpty() == false)
	{
		QPixmap image(fileName);
		painter.drawPixmap(target, image, image.rect());
	}
	else
	{
		painter.setPen(QPen(Qt::yellow, 2, Qt::SolidLine));
		painter.drawLine(0, length / 2, length, length / 2);
		painter.drawLine(length / 2, 0, length / 2, length);
	}
	return pixmap;
}

QVector<QPoint> SliceWidget::getShortestPath(QPoint s_point, QPoint e_point)
{

	std::priority_queue<candidatePoint,std::vector<candidatePoint>,std::greater<candidatePoint>> L_Queue;

	candidatePoint g;
	g.x = s_point.x();
	g.y = s_point.y();
	g.cost = 0;

	L_Queue.push(g);

	QVector<QVector<int>> NodeState(GradientMap.length());
	QVector<QVector<int>> costMap(GradientMap.length());
	QVector<QVector<QPoint>> prevNodeMap(GradientMap.length());

	QVector<int> temp(GradientMap[0].length(),-1);
	QVector<int> temp_cost(GradientMap[0].length(),10000);
	QVector<QPoint> temp_prevNodeMap(GradientMap[0].length(),QPoint(-1,-1));

	NodeState.fill(temp);
	costMap.fill(temp_cost);
	prevNodeMap.fill(temp_prevNodeMap);

	costMap[s_point.x()][s_point.y()] = 0;

	while (!L_Queue.empty() && NodeState[e_point.x()][e_point.y()] != 1) //如果e_point已加入到集合中，则结束
	{
		QPoint q = QPoint(L_Queue.top().x, L_Queue.top().y);
		L_Queue.pop();

		NodeState[q.x()][q.y()] = 1;
		//qDebug() << q;
		
		//for each q's neighborhood 
		for (auto i = -1; i < 2; i++)
		{
			for (auto j = -1; j < 2; j++)
			{
				QPoint r = q + QPoint(i, j);
				//boarder checking
				if (r.x() > NodeState[0].length() - 1 || r.y() > NodeState.length() - 1 || r.x() < 1 || r.y() < 1) continue;

				if (NodeState[r.x()][r.y()] != 1) // if not expanded
				{
					float scale = 1.0;
					if (abs(i) == 1 && abs(j) == 1) { scale = 1.414; } // if diagonal multiple sqrt(2)
					int tempCost = costMap[q.x()][q.y()] + scale * GradientMap[r.x()][r.y()]; // G_temp = g(q)+l(q,r)

					if (NodeState[r.x()][r.y()] == -1)
					{
						costMap[r.x()][r.y()] = tempCost;
						NodeState[r.x()][r.y()] = 0;
						prevNodeMap[r.x()][r.y()] = q;

						candidatePoint r_inqueue;
						r_inqueue.x = r.x();
						r_inqueue.y = r.y();
						r_inqueue.cost = tempCost;

						L_Queue.push(r_inqueue);
					}
					if (NodeState[r.x()][r.y()] == 0 && tempCost < costMap[r.x()][r.y()])
					{
						costMap[r.x()][r.y()] = tempCost;
						prevNodeMap[r.x()][r.y()] = q;
					}
				}
			}
		}
	}

	// 遍历prevNodeMap获取从e_point到s_point的最短路径
	QVector<QPoint> path = QVector<QPoint>();
	path.push_back(e_point);
	while (prevNodeMap[path.back().x()][path.back().y()] != QPoint(-1, -1))
	{
		path.push_back(prevNodeMap[path.back().x()][path.back().y()]);
	}

	//qDebug() <<"path!!!" <<path;

	return path;
}
QPoint SliceWidget::snapPoint(QPoint clickPoint, int range)
{
	int click_X = clickPoint.x();
	int click_Y = clickPoint.y();
	
	QPoint snapPoint = clickPoint;
	auto mincost = GradientMap[click_X][click_Y];
	//对用户点击到的区域周围2*range的像素搜索，snap到附近梯度最小的点
	for (auto i = -range; i < range; i++)
	{
		for (auto j = -range; j < range; j++)
		{
			if (click_X + i < 0 || click_Y + j < 0 || click_X + i >= GradientMap.length() || click_Y + j >= GradientMap[0].length()) continue;

			if (GradientMap[click_X + i][click_Y + j] < mincost) 
			{
				snapPoint = QPoint(click_X + i, click_Y + j);
				mincost = GradientMap[click_X + i][click_Y + j];
			}
			
		}
	}



		
	return snapPoint;
}

inline
void SliceWidget::setMarkHelper(
	const QList<StrokeMarkItem*>& items)
{
	foreach(StrokeMarkItem * item, items)
	{
		item->setVisible(item->visibleState());
		item->setParentItem(m_slice);
	}

}

inline
void SliceWidget::clearSliceMarksHelper(SliceItem * slice)
{
	if (slice == nullptr)
	{
		qWarning("Top slice is empty.");
		return;
	}
	auto children = slice->childItems();
	foreach(QGraphicsItem * item, children)
	{
		item->setVisible(false);
	}
}
/*
 * \brief
 * \param image
 */
void SliceWidget::setImage(const QImage& image)
{
	const auto size = image.size();
	const auto pos = QPoint(-size.width() / 2, -size.height() / 2);

	if (m_slice == nullptr)
	{
		m_slice = new SliceItem(QPixmap::fromImage(image));
		(m_slice)->setFlag(QGraphicsItem::ItemClipsChildrenToShape);
		m_slice->setPos(pos);
		m_anchorItem->setParentItem(m_slice);
		scene()->addItem(m_slice);
		/**
		 *We need to give a exactly scene rect according to the image size for efficiency rendering.
		 *We assume that the size of rect of the scene is two times larger than the size of image.
		 */
		auto rect = image.rect();
		translate(rect.width() / 2, rect.height() / 2);
		rect.adjust(-rect.width(), -rect.height(), 0, 0);
		scene()->setSceneRect(QRectF(-10000, -10000, 20000, 20000));
		// We need to translate the view so as to let the slice is centered in it.
		//scene()->setSceneRect(rect);
	}
	else
	{
		m_slice->setPixmap(QPixmap::fromImage(image));
	}


//不在本地计算梯度图了
//	int width = m_slice->pixmap().width();
//	int height = m_slice->pixmap().height();
//
//	if (width == height) //如果是最大的那个，计算梯度图象
//	{
//		QImage currentImage = m_slice->pixmap().toImage().convertToFormat(QImage::Format_Grayscale8);
//
//		float maxGradient = 0.0;
//
//		cimg_library::CImg<unsigned char> imageHelper(
//			currentImage.bits(),
//			currentImage.bytesPerLine(),			// QImage requires 32-bit aligned for each scanLine, but CImg don't.
//			height,
//			1,
//			true);							// Share data
//
//		const auto mean = imageHelper.mean();
//
//#ifdef _OPENMP
//#pragma omp parallel for
//#endif	
//		//调整对比度
//		for (auto h = 0; h < height; ++h) {
//			const auto scanLine = currentImage.scanLine(h);
//			for (auto w = 0; w < width; ++w) {
//				auto t = scanLine[w] - mean;
//				t *= 3; //Adjust contrast
//				t += mean * 1; // Adjust brightness
//				scanLine[w] = (t > 255.0) ? (255) : (t < 0.0 ? (0) : (t));
//			}
//		}
//		
//		//currentImage.save("contrast.jpg");
//
//		GradientMap.clear();
//
//		for (int i = 0; i < height; i++)
//		{
//			QVector<int> temp;
//			for (int j = 0; j < width; j++)
//			{
//				temp.push_back(255);
//			}
//			GradientMap.push_back(temp);
//		}
//
//		//do not consider
//		QVector<QVector<int>> LaplacianMap;
//
//		for (int i = 0; i < height; i++)
//		{
//			QVector<int> temp;
//			for (int j = 0; j < width; j++)
//			{
//				temp.push_back(0);
//			}
//			LaplacianMap.push_back(temp);
//		}
//
//
//		//平滑
//		//for (auto i = 1; i < width-1; i++)
//		//{
//		//	for (auto j = 1; j < height-1; j++)
//		//	{
//		//		auto p1 = qGray(currentImage.pixel(i - 1, j - 1));
//		//		auto p2 = qGray(currentImage.pixel(i, j - 1));
//		//		auto p3 = qGray(currentImage.pixel(i + 1, j - 1));
//
//		//		auto p4 = qGray(currentImage.pixel(i - 1, j));
//		//		auto p5 = qGray(currentImage.pixel(i, j));
//		//		auto p6 = qGray(currentImage.pixel(i + 1, j));
//
//		//		auto p7 = qGray(currentImage.pixel(i - 1, j + 1));
//		//		auto p8 = qGray(currentImage.pixel(i, j + 1));
//		//		auto p9 = qGray(currentImage.pixel(i + 1, j + 1));
//
//		//		auto pixelValue = (p1+p2+p3+p4+p5+p6+p7+p8+p9) / 9;
//		//		QRgb grayPixel = qRgb(pixelValue, pixelValue, pixelValue);
//
//		//		currentImage.setPixel(i, j, currentImage.pixel(i, j));
//		//	}
//		//}
//		//currentImage.save("test.png");
//
//#ifdef _OPENMP
//#pragma omp parallel for
//#endif			
//		for (auto i = 1; i < width - 1; i++)
//		{
//			for (auto j = 1; j < height - 1; j++)
//			{
//				/*
//				p1 p2 p3
//				p4 p5 p6
//				p7 p8 p9
//				*/
//				auto p1 = qGray(currentImage.pixel(i - 1, j - 1));
//				auto p2 = qGray(currentImage.pixel(i, j - 1));
//				auto p3 = qGray(currentImage.pixel(i + 1, j - 1));
//				
//				auto p4 = qGray(currentImage.pixel(i - 1, j));
//				auto p5 = qGray(currentImage.pixel(i, j));
//				auto p6 = qGray(currentImage.pixel(i + 1, j));
//				
//				auto p7 = qGray(currentImage.pixel(i - 1, j + 1));
//				auto p8 = qGray(currentImage.pixel(i, j + 1));
//				auto p9 = qGray(currentImage.pixel(i + 1, j + 1));
//
//				auto gradient_x = p3+2*p6+p9-p1-2*p4-p7;
//				auto gradient_y = p7+2*p8+p9-p1-2*p2-p3;
//
//				auto gradient = (abs(gradient_x) + abs(gradient_y));
//				
//				auto laplacian = p2 + p4 + p6 + p8 - 4 * p5;
//				//auto gradient = p1 + p2 + p3 + p4 - 8 * p5 + p6 + p7 + p8 + p8;  Laplace
//				GradientMap[i][j] = gradient;
//				LaplacianMap[i][j] = laplacian;
//
//				if (gradient > maxGradient) maxGradient = gradient;
//			}
//		}
//		//qDebug() << maxGradient;
//#ifdef _OPENMP
//#pragma omp parallel for
//#endif	
//		for (auto i = 1; i < width - 1; i++)
//		{
//			for (auto j = 1; j < height - 1; j++)
//			{
//				auto gradient = float(1-GradientMap[i][j]/maxGradient)*255;//inverse
//				GradientMap[i][j] = gradient; //映射到0-255区间
//				//QRgb grayPixel = qRgb(gradient, gradient, gradient);
//				//m_gradientImage->setPixel(i, j, grayPixel);
//			}
//		}
//		//m_gradientImage->save("m_gradientImage.jpg");
//	
//		//计算拉普拉斯零点
//#ifdef _OPENMP
//#pragma omp parallel for
//#endif	
//		for (auto i = 1; i < width - 1; i++)
//		{
//			for (auto j = 1; j < height - 1; j++)
//			{
//				auto laplacian = LaplacianMap[i][j];
//				if (laplacian > 0) laplacian = 255;
//
//				//QRgb laplacianPixel = qRgb(laplacian, laplacian, laplacian);
//				//m_laplacianImage->setPixel(i, j, laplacianPixel);
//			}
//		}
//
//		//m_laplacianImage->save("m_laplacianImage.jpg");
//	}
	//std::cout << "trigger setImage function"<<std::endl;
}

void SliceWidget::setDefaultZoom()
{
	fitInView(m_slice->boundingRect(),Qt::KeepAspectRatio);
	const auto size = m_slice->pixmap().size();
	centerOn(m_slice->mapToScene(size.width() / 2, size.height() / 2));
}

void SliceWidget::clearSliceMarks()
{
	clearSliceMarksHelper(m_slice);
	//std::cout << "trigger clearSliceMarks";
}

QList<StrokeMarkItem*> SliceWidget::selectedItems() const
{
	Q_ASSERT_X(scene(), "SliceWidget::selectedItems", "null pointer");
	const auto items = scene()->selectedItems();
	QList<StrokeMarkItem*> marks;
	for (const auto item : items)
		marks << qgraphicsitem_cast<StrokeMarkItem*>(item);
	return marks;
}

int SliceWidget::selectedItemCount() const
{
	Q_ASSERT_X(scene(), "SliceWidget::selectedItems", "null pointer");
	return scene()->selectedItems().size();
}

void SliceWidget::moveSlice(const QPointF& dir)
{
	Q_ASSERT_X(m_slice, "SliceWidget::moveSlice", "null pointer");
	m_slice->moveBy(dir.x(), dir.y());
}

QSize SliceWidget::sizeHint() const
{
	//const auto maxLength = std::max(m_image.width(), m_image.height());
	//if (maxLength < 800)
		//return m_image.size();
	if (m_slice == nullptr)
	{
		return { 0,0 };
	}
	return m_slice->pixmap().size().scaled(700, 700, Qt::KeepAspectRatio);
}
