#ifndef MRCFILEINFO_H
#define MRCFILEINFO_H

#include <QLabel>
#include <QComboBox>
#include <QTextEdit>
#include <QLayout>
#include <QTreeView>



class MRC;

class MRCFileInfoViewer : public QWidget
{
    Q_OBJECT
public:
    explicit MRCFileInfoViewer(QWidget *parent = nullptr)noexcept;
   void addFileName(const QString & fileName);
   void addItem(const QString & fileName,const QVariant & userData = QVariant());
   void setText(const QString & info);
   void getText()const;
   int count()const;
   QVariant itemData(int index, int role = Qt::UserRole)const;
private:
    QGridLayout * m_layout;
    QLabel * m_label;
    QComboBox * m_filesComboBox;
    QTextEdit * m_filesInfoTextEdit;
private:
    void createConnections();
signals:
    void currentIndexChanged(int index);
    void activated(int index);
public slots:
   void setCurrentIndex(int index);
};



#endif // MRCFILEINFO_H
