#include "testinfodialog.h"

TestInfoDialog::TestInfoDialog(QWidget * parent):QDialog(parent)
{
    m_textEditor = new QTextEdit(this);
    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->addWidget(m_textEditor);
    setLayout(layout);
}

void TestInfoDialog::setText(const QString &info)
{
    m_textEditor->setText(info);
}

void TestInfoDialog::appendText(const QString &info)
{
    m_textEditor->append(info);
}

void TestInfoDialog::clearText()
{
    m_textEditor->clear();
}
