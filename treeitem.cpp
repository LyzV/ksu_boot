#include "treeitem.h"


void TreeItem::createStorage(int type, const QString &path)
{
    if(WT_STORAGE!=whats) return;

    ExData.insert(DK_TYPE, type);
    ExData.insert(DK_PATH, path);
}

void TreeItem::createSoft(int type, const QString &name, const QStringList &filt)
{
    if(WT_SOFT!=whats) return;

    ExData.insert(DK_TYPE, type);
    ExData.insert(DK_NAME, name);
    ExData.insert(DK_FILT, filt);
}

