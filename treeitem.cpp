#include "treeitem.h"

TreeItem::TreeItem(int whats, TreeItem *parent)
{
    this->whats=whats;
    Parent=parent;
    if(nullptr==Parent) return;
    Parent->Rows.append(this);
    number=Parent->Rows.count()-1;
}

TreeItem::~TreeItem()
{
    TreeItem *item;
    while(Rows.count())
    {
        item=Rows.at(0);
        Rows.remove(0);
        if(nullptr!=item)
            delete item;
    }
    qDeleteAll(Rows);
}

void TreeItem::createStorage(int type, const QString &path)
{
    if(WT_STORAGE!=whats) return;

    ExData.insert(DK_TYPE, type);
    ExData.insert(DK_PATH, path);
}

void TreeItem::createSoft(int type, const QString &path, const QStringList &filt)
{
    if(WT_SOFT!=whats) return;

    ExData.insert(DK_TYPE, type);
    ExData.insert(DK_PATH, path);
    ExData.insert(DK_FILT, filt);
}

