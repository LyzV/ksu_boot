#include "qksutreeview.h"
#include "bootnamespase.h"
#include "qctrlform.h"

QKsuTreeView::QKsuTreeView(QWidget *parent)
    : QTreeView(parent)
{

}

QKsuTreeView::~QKsuTreeView()
{

}

#include <QDialog>
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
                    int whats=this->model()->data(index, Boot::WhatsThis_UserRole).toInt();
                    switch(whats)
                    {
                    default:
                    case (int)Boot::Root: QTreeView::keyPressEvent(event); break;
                    case (int)Boot::Storage:
                        {
                            if(this->isExpanded(index)) this->collapse(index);
                            else                        this->expand  (index);
                        }
                        break;
                    case (int)Boot::Device:
                        {
                            if(this->isExpanded(index)) this->collapse(index);
                            else                        this->expand  (index);
                        }
                        break;
                    case (int)Boot::File:
                        {
                            //QDialog dialog;
                            //dialog.setGeometry(10, 10, 100, 100);
                            //dialog.exec();
                            QCtrlForm *ctrl_form=new QCtrlForm;
                            ctrl_form->setParent(this);
                            ctrl_form->show();


                        }
                        break;
                    }
                }
            }
            else
                QTreeView::keyPressEvent(event);

        }
        break;
    }
}
