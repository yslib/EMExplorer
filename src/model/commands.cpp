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
    if(m_widget)
        m_widget->removeItem(m_type, m_mark);
}

void AddMarkCommand::redo()
{
    m_widget->addMark(m_type, m_mark);
    m_mark->show();
}
