#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    qRegisterMetaType<FolderInfoPointer>();

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
