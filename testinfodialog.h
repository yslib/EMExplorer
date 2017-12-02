#ifndef TESTINFODIALOG_H
#define TESTINFODIALOG_H
#include <QDialog>
#include <QTextEdit>
#include <QString>
#include <QLayout>

class TestInfoDialog:public QDialog
{
public:
    TestInfoDialog(QWidget * parent = nullptr);
    void setText(const QString & info);
    void appendText(const QString & info);
    void clearText();
private:
    QTextEdit * m_textEditor;
};

#endif // TESTINFODIALOG_H
