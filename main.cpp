#include "widgets/mainwindow.h"

#include <QApplication>
#include <QSurfaceFormat>
#include <QDebug>
#include <QOpenGLContext>

int main(int argc, char *argv[])
{
	//QSurfaceFormat format;
	//format.setDepthBufferSize(24);
	//format.setStencilBufferSize(8);
	//format.setVersion(3, 3);
	//format.setProfile(QSurfaceFormat::CoreProfile);
	//QSurfaceFormat::setDefaultFormat(format);

	QCoreApplication::setOrganizationName("cadcg");
	QCoreApplication::setApplicationName("MRC Marker");

	//QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts,true);

    QApplication a(argc, argv);

	//auto fmt = QSurfaceFormat::defaultFormat();
	//qDebug() << "Surface Format:" << fmt;

	//auto moduleType = QOpenGLContext::openGLModuleType();
	//qDebug() << moduleType;



    MainWindow w;
    w.show();
    return a.exec();
}
