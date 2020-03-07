#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QDialog>
#include "qksutreeview.h"
#include <QTextCodec>


class MainDialog : public QDialog
{
    Q_OBJECT

    QTextCodec *codec=nullptr;
    QKsuTreeView *treeView=nullptr;
public:
    explicit MainDialog(QAbstractItemModel *model, const QString &workDirectory, QWidget *parent=nullptr);
};

#endif // MAINDIALOG_H
