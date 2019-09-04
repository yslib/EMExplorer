#include "commands.h"

AddMarkCommand::AddMarkCommand(SliceItem * slice)
{
    m_mark = new StrokeMarkItem(slice);
}

AddMarkCommand::~AddMarkCommand()
{
    delete m_mark;
}

void AddMarkCommand::undo()
{
    m_widget->removeMark(m_mark);
}

void AddMarkCommand::redo()
{
    m_widget->addMark(m_type, m_mark);
}

RemoveMarkCommand::RemoveMarkCommand(SliceEditorWidget * widget)
{
    m_widget = widget;
    for (auto index : m_widget->markModel()->selectionModel()->selectedIndexes()) {
        m_marks.append(static_cast<StrokeMarkTreeItem *>(index.internalPointer())->markItem());
    }
}

RemoveMarkCommand::~RemoveMarkCommand()
{

}

void RemoveMarkCommand::undo()
{
    for(auto mark : m_marks)
        m_widget->addMark(mark->sliceType(), mark);
}

void RemoveMarkCommand::redo()
{
    for(auto mark : m_marks)
        m_widget->removeMark(mark);
}
