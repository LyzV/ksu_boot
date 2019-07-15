#ifndef QKSUTREEVIEW_H
#define QKSUTREEVIEW_H

#include <QTreeView>
#include <QKeyEvent>

class QKsuTreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit QKsuTreeView(QWidget *parent=nullptr);
    ~QKsuTreeView();

protected:
    void keyPressEvent(QKeyEvent *event) override;

};

#endif // QKSUTREEVIEW_H
