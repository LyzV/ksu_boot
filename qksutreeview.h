#ifndef QKSUTREEVIEW_H
#define QKSUTREEVIEW_H

#include <QTreeView>
#include <QKeyEvent>
#include "qctrlform.h"
#include <QTextCodec>

class QKsuTreeView : public QTreeView
{
    Q_OBJECT

    QCtrlForm CtrlForm;
    QTextCodec *codec;
public:
    explicit QKsuTreeView(QWidget *parent=nullptr);
    ~QKsuTreeView();

protected:
    void keyPressEvent(QKeyEvent *event) override;

};

#endif // QKSUTREEVIEW_H
