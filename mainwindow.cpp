#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_triggered()
{
    QString fileName =QFileDialog::getOpenFileName(this,tr("OpenFile"),tr("/Users/Ysl/Downloads/ETdataSegmentation"),tr("Image Files(*.mrc)"));
    if(fileName == ""){
        return;
    }
   // ui->comboBox->addItem(fileName);
    m_mrcs.push_back(MRC(fileName));
    if(m_mrcs.back().isOpened() == false){
        QMessageBox::critical(NULL, "Error", tr("Can't open this file.\n%1").arg(fileName),
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    }else{
        QString headerInfo = m_mrcs.back().getMRCInfo();
        ui->textEdit->setText(headerInfo);
        m_imageView->setPixmap(QPixmap::fromImage(m_mrcs.back().getSlice(0)));
    }

}

void MainWindow::_init()
{
    m_imageView =  new QLabel(this);
    QHBoxLayout * layout = new QHBoxLayout(this);
    layout->addWidget(m_imageView);
    ui->groupBox->setLayout(layout);
}
