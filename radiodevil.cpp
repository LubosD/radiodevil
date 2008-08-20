#include <QApplication>
#include <QSettings>
#include <iostream>
#include "Settings.h"
#include "MainWindow.h"
#include "SoxReader.h"

using namespace std;

void playWithAlsa();

int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	MainWindow* wnd;
	
	QCoreApplication::setOrganizationName("Dolezel");
	QCoreApplication::setOrganizationDomain("dolezel.info");
	QCoreApplication::setApplicationName("radiodevil");
	
	SoxReader::globalInit();
	initSettings();
	app.setQuitOnLastWindowClosed(false);
	
	int retval;
	
	wnd = new MainWindow;
	wnd->show();
	retval = app.exec();
	
	delete wnd;
	exitSettings();
	
	return retval;
}
/*
void playWithAlsa()
{
	QStringList devices = AlsaMixer::devices();
	
	foreach(QString e, devices)
		cout << "Device name: " << e.toStdString() << endl;
	
	AlsaMixer mixer(0);
	QList<AlsaMixer::MixerElem> elems;
	
	elems = mixer.elems();
	
	foreach(AlsaMixer::MixerElem e, elems)
		cout << "Element name: " << e.strName.toStdString() << endl;
}
*/
