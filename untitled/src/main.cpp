#include "ui/MainWindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 组织名/应用名会影响 QSettings 与 QStandardPaths 的落地位置，
    // 现在设好，M2 的数据库路径就直接可用，不用回头改。
    QCoreApplication::setOrganizationName(QStringLiteral("gqssb"));
    QCoreApplication::setApplicationName(QStringLiteral("ACM"));
    QCoreApplication::setApplicationVersion(QStringLiteral("0.1.0"));

    MainWindow w;
    w.show();
    return a.exec();
}
