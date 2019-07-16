#include "qksutreeview.h"

QKsuTreeView::QKsuTreeView(QWidget *parent)
    : QTreeView(parent)
{

}

QKsuTreeView::~QKsuTreeView()
{

}

//#include <QModelIndexList>
void QKsuTreeView::keyPressEvent(QKeyEvent *event)
{
    int key;
    switch(event->type())
    {
    default: QTreeView::keyPressEvent(event); break;
    case QEvent::KeyPress:
        {
            key=event->key();
            if(
               (Qt::Key_Enter==key)||
               (Qt::Key_Return==key)
              )
            {
                QModelIndex index=this->currentIndex();
                if(false==index.isValid())
                {
                    QTreeView::keyPressEvent(event);
                }
                else
                {
                    if(
                            QString::compare("file")
                            this->model()->data(index, Qt::WhatsThisRole).toString()
                      )
                    if(this->isExpanded(index)) this->collapse(index);
                    else                        this->expand  (index);

                }
            }
            else
                QTreeView::keyPressEvent(event);

        }
        break;
    }
}
