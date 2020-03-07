#include "maindialog.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QString>
#include <QCoreApplication>

#define TU(s) codec->toUnicode(s) //tr(s)

MainDialog::MainDialog(QAbstractItemModel *model, const QString &workDirectory, QWidget *parent)
    : QDialog(parent)
{
    Q_ASSERT(model);

    codec=QTextCodec::codecForName("CP1251");

    QVBoxLayout *boxLayout = new QVBoxLayout;
    boxLayout->setMargin(0);
    //boxLayout->setSpacing(10);
    QLabel *helpLabel0 = new QLabel(TU(""));
    QString versioStr(TU("ÂÅÐÑÈß ÏÎ ÇÀÃÐÓÇ×ÈÊÀ: "));
    versioStr+=QCoreApplication::applicationVersion();
    QLabel *helpLabel1 = new QLabel(versioStr);
    QLabel *helpLabel2 = new QLabel(TU("ÄËß ÂÛÕÎÄÀ Â ÐÀÁÎ×ÅÅ ÏÎ ÍÀÆÌÈÒÅ \"ÌÅÍÞ\".")); Q_ASSERT(helpLabel1);
    QLabel *helpLabel3 = new QLabel(TU("ÄËß ÎÁÍÎÂËÅÍÈß ÏÎ ÂÑÒÀÂÜÒÅ USB-flash,")); Q_ASSERT(helpLabel2);
    QLabel *helpLabel4 = new QLabel(TU("ÂÛÁÅÐÈÒÅ ÏÎ, ÍÀÆÌÈÒÅ \"ÂÂÎÄ\".")); Q_ASSERT(helpLabel3);

    helpLabel0->setFont(QFont("", 5));
    helpLabel1->setFont(QFont("", 15));
    helpLabel2->setFont(QFont("", 15));
    helpLabel3->setFont(QFont("", 15));
    helpLabel4->setFont(QFont("", 15));
    boxLayout->addWidget(helpLabel0, 0, Qt::AlignHCenter);
    boxLayout->addWidget(helpLabel1, 0, Qt::AlignHCenter);
    boxLayout->addWidget(helpLabel2, 0, Qt::AlignHCenter);
    boxLayout->addWidget(helpLabel3, 0, Qt::AlignHCenter);
    boxLayout->addWidget(helpLabel4, 0, Qt::AlignHCenter);

    treeView = new QKsuTreeView(workDirectory, this); Q_ASSERT(treeView);
    treeView->setModel(model);
    boxLayout->addWidget(treeView);
    setLayout(boxLayout);

    treeView->setAutoScroll(true);
    treeView->setColumnWidth(0, 420);
    treeView->setFont(QFont("", 15));
    treeView->setItemsExpandable(true);
    treeView->setWindowTitle(TU("ÏÐÎÃÐÀÌÌÀ ÇÀÃÐÓÇ×ÈÊÀ ÊÑÓ"));

    this->setGeometry(QRect(0, 0, 640, 480));
}
