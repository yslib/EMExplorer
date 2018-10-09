#include "widgets/mainwindow.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>


int main(int argc, char *argv[])
{
	QCoreApplication::setOrganizationName("cadcg");
	QCoreApplication::setApplicationName("MRC Marker");

    QApplication a(argc, argv);

	QCommandLineParser parser;
	parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);

	QCommandLineOption mrcFileOptions(QStringLiteral("mrc"),QStringLiteral("MRC file path"),QStringLiteral("path"));
	QCommandLineOption markFileOptions(QStringLiteral("mark"), QStringLiteral("Mark file Path"),QStringLiteral("path"));
	parser.addOption(mrcFileOptions);
	parser.addOption(markFileOptions);

	parser.process(a);

	QString mrcPath = parser.value(mrcFileOptions);
	QString markPath = parser.value(markFileOptions);

    MainWindow w;
    w.show();
	if(mrcPath.isEmpty() == false) {
		w.open(mrcPath);
		w.openMark(markPath);
	}
    return a.exec();
}
