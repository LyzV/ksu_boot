#ifndef QKSUTREEVIEW_H
#define QKSUTREEVIEW_H

#include <QTreeView>
#include <QKeyEvent>
#include "qctrlform.h"
#include <QTextCodec>
#include <QString>

class QKsuTreeView : public QTreeView
{
    Q_OBJECT

    QCtrlForm CtrlForm;
    QTextCodec *codec;
public:
    explicit QKsuTreeView(const QString &workDirectory, QWidget *parent=nullptr);
    ~QKsuTreeView();

protected:
    void keyPressEvent(QKeyEvent *event) override;

signals:
    void quitSignal();

};

#endif // QKSUTREEVIEW_H
