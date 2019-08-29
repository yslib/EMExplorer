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
    if(!first)
        m_widget->addItem(m_type, m_mark);
    m_widget->markAddedHelper(m_type, m_mark);  
    first = false;
}
