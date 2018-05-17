#include "appframe.h"

#include <QApplication>
#include <QFile>
#include <QFontDatabase>
#include <QFrame>

using namespace EE;

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("d100");
    QCoreApplication::setOrganizationDomain("d100.site");
    QCoreApplication::setApplicationName("EE");

    QApplication app(argc, argv);
    QFontDatabase::addApplicationFont(":/assets/fonts/amiko-regular.ttf");

    QFile styleFile(":/ee.qss");
    styleFile.open(QFile::ReadOnly);
    QString stylesheet = QString(styleFile.readAll());
    styleFile.close();
    app.setStyleSheet(stylesheet);

    AppFrame window;
    window.restoreUXSettings();
    window.setDisassemblyStateData();

    window.show();
    return app.exec();
}
