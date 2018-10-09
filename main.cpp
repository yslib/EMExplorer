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

	const QCommandLineOption mrcFileOptions(QStringLiteral("mrc"), QStringLiteral("MRC file path"),QStringLiteral("path"));
	const QCommandLineOption markFileOptions(QStringLiteral("mark"), QStringLiteral("Mark file Path"),QStringLiteral("path"));
	parser.addOption(mrcFileOptions);
	parser.addOption(markFileOptions);

	parser.process(a);

	const QString mrcPath = parser.value(mrcFileOptions);
	const QString markPath = parser.value(markFileOptions);

    MainWindow w;
    w.show();

	if(mrcPath.isEmpty() == false) {
		w.open(mrcPath);
		w.openMark(markPath);
	}

    return a.exec();
}
