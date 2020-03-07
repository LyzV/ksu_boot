#include "qksutreeview.h"
#include "treemodel.h"
#include "qbootstrap.h"
#include <QApplication>

#define TU(s) codec->toUnicode(s) //tr(s)

QKsuTreeView::QKsuTreeView(const QString &workDirectory, QWidget *parent):
    QTreeView(parent),
    CtrlForm(workDirectory)
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
                        int storageType;
                        int softType;
                        QString fileName;
                        QString filePath;

                        const TreeModel *treeModel=reinterpret_cast<const TreeModel *>(index.model());
                        if(treeModel->getFile(index, storageType, softType, filePath, fileName))
                            CtrlForm.Exec(storageType, softType, filePath, fileName);
                    }
                }
            }
            else if(
                    (Qt::Key_Home  ==key)||
                    (Qt::Key_Escape==key)
                   )
            {
                QStringList bootPathList;
                QBootstrap bootService;
                bootPathList.append("/home/root/ksu1");
                bootPathList.append("/home/root/ksu2");
                bootService.create(bootPathList, "/home/root");
                if(true==bootService.bootstrap())
                    QApplication::quit();
            }
            else
            {
                QTreeView::keyPressEvent(event);
            }
        }
        break;
    }
}

