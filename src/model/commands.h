#ifndef COMMANDS_H
#define COMMANDS_H

#include <QUndoCommand>
#include "widgets/sliceeditorwidget.h"
#include "model/markmodel.h"
#include "model/sliceitem.h"

class AddMarkCommand : public QUndoCommand
{
public:
    AddMarkCommand(SliceItem *slice);
    ~AddMarkCommand() override;
    void undo() override;
    void redo() override;
    void setMark(const QPen &pen){m_mark->setPen(pen);m_mark->setFlag(QGraphicsItem::ItemIsSelectable);}
    void addPoint(const QPointF& p){m_mark->appendPoint(p);}
    int size(){return m_mark->polygon().size();}
    void setType(SliceType type){m_type = type;}
    void setWidget(SliceEditorWidget *widget){m_widget = widget;}


private:
    SliceEditorWidget *m_widget;
    StrokeMarkItem * m_mark;
    SliceType m_type;
    bool first = true;
};

#endif // COMMANDS_H
