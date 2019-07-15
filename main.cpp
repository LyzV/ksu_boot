#include "treemodel.h"
#include "qksutreeview.h"
#include <QApplication>
#include <QTreeView>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TreeModel model;
    QKsuTreeView view;
    view.setModel(&model);
    view.setAutoScroll(true);
    view.setColumnWidth(0, 420);
    view.setFont(QFont("", 15));
    view.setItemsExpandable(true);
    view.setWindowTitle(QObject::tr("Simple Tree Model"));
    view.setGeometry(QRect(0, 0, 640, 480));
    view.show();
    return a.exec();
}
