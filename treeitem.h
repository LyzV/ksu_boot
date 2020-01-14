#ifndef TREEITEM_H
#define TREEITEM_H

#include <QVector>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QHash>

//What is the element?
#define WT_ROOT     0
#define WT_OTHER    1
#define WT_STORAGE  2
#define WT_SOFT     3
#define WT_FILE     4

//Data Key of Hash Table
#define DK_TYPE   0
#define DK_PATH   1
#define DK_NAME   2
#define DK_FILT   3

//storage types:
#define STT_KSU  0
#define STT_USB  1

//soft types:
#define SFT_KSUWORK  0
#define SFT_KSUBOOT  1
#define SFT_KI       2
#define SFT_SYS      4

//other types:
#define OTH_EXIT    0
#define OTH_HELP    1




class TreeItem
{
    int whats=WT_ROOT;
    TreeItem *Parent=nullptr;
    int number=-1;

public:
    explicit TreeItem(int whats=WT_ROOT, TreeItem *parent=nullptr);
    ~TreeItem();

    int WhatIsThis() const { return whats; }
    TreeItem *parentItem() const { return Parent; }
    int RowNumber() const { return number; }

    void createStorage(int type, const QString &path);
    void createSoft(int type, const QString &path, const QStringList &filt);

    QVector<TreeItem *> Rows;
    QStringList Columns;
    QHash<int, QVariant> ExData;
};

#endif // TREEITEM_H
