#include "qksutreeview.h"
#include "treemodel.h"

#define TU(s) tr(s)

QKsuTreeView::QKsuTreeView(QWidget *parent):
    QTreeView(parent),
    CtrlForm(nullptr)
{
    codec=QTextCodec::codecForName("CP1251");
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
                    QString whats=this->model()->data(index, Qt::WhatsThisRole).toString();
                    if(0==whats.compare(TreeModel::sROOT))
                    {
                        QTreeView::keyPressEvent(event);
                    }
                    else if(0==whats.compare(TreeModel::sSTORAGE))
                    {
                        if(this->isExpanded(index)) this->collapse(index);
                        else                        this->expand  (index);
                    }
                    else if(0==whats.compare(TreeModel::sSOFT))
                    {
                        if(this->isExpanded(index)) this->collapse(index);
                        else                        this->expand  (index);
                    }
                    else if(0==whats.compare(TreeModel::sFILE))
                    {
                        QString file_name(TU("РџСЂРѕС€РёРІРєР°: "));
                        file_name+=this->model()->data(index, Qt::DisplayRole).toString();
                        CtrlForm.Exec(file_name);
                    }
                }
            }
            else
                QTreeView::keyPressEvent(event);

        }
        break;
    }
}
