#include "widgets/slicemainwindow.h"
#include "widgets/volumemainwindow.h"
#include "widgets/mainwindow.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QTimer>
#include <QSurfaceFormat>

#include <vector>
#include <qmath.h>


int main(int argc, char *argv[])
{
	QCoreApplication::setOrganizationName("cadcg");
	QCoreApplication::setApplicationName("EMExplorer");


	//qDebug() << (~c);
//#ifndef  WIN32
//    QSurfaceFormat fmt;
//    fmt.setVersion(3,3);
//    fmt.setDepthBufferSize(24);
//    fmt.setStencilBufferSize(8);
//    fmt.setProfile(QSurfaceFormat::CoreProfile);
//    QSurfaceFormat::setDefaultFormat(fmt);
//#endif
	


	QApplication a(argc, argv);
	QCommandLineParser parser;
	parser.addOptions({
			{QStringLiteral("f"), QStringLiteral("Slice data or marks data file path."), QStringLiteral("path")},	// An option with value
			{QStringLiteral("s"), QStringLiteral("Open a main window which only has slice view.")},					// An option
			{QStringLiteral("v"), QStringLiteral("Open a window which only has a volume view.")},
			{QStringLiteral("m"), QStringLiteral("Mark data file path"),QStringLiteral("path")},
			{QStringLiteral("l"), QStringLiteral("Open a slice window which has marking function")}
		});

	parser.process(a);

	if (parser.isSet("s")) {
		SliceMainWindow w;
		if (parser.isSet("f")) {
			w.open(parser.value("f"));
			if (parser.isSet("m")) {
				w.openMark(parser.value("m"));
			}
		}
		w.show();
		return a.exec();
	}
	else if (parser.isSet("v")) {
		VolumeMainWindow w;
		// Note:
		// OpenGL context is initialized when event loop starts, so a delayed operation using context needs to be performed
		if (parser.isSet("f")) {
			QTimer::singleShot(10, [&w, &parser]() {w.open(parser.value("f")); });
			if (parser.isSet("m")) {
				QTimer::singleShot(15, [&w, &parser]() {w.open(parser.value("m")); });
			}
		}

		w.show();
		return a.exec();
	}
	else {
		MainWindow w;
		w.show();
		return a.exec();
	}

}
