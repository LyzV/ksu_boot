#ifndef TREEITEM_H
#define TREEITEM_H

#include <QVector>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QHash>

//What is the element?
#define WT_ROOT     0
#define WT_STORAGE  1
#define WT_SOFT     2
#define WT_FILE     3

#define ST_KSU  0
#define ST_USB  1

#define SF_KSUWORK  0
#define SF_KSUBOOT  1
#define SF_KI       2
#define SF_KPT      3
#define SF_SYS      4

//Data Key
#define DK_TYPE   0
#define DK_PATH   1
#define DK_NAME   2
#define DK_FILT   3

//storage type:
#define STT_KSU  0
#define STT_USB  1

//soft type
#define SFT_KSUWORK  0
#define SFT_KSUBOOT  1
#define SFT_KI       2
#define SFT_SYS      4



class TreeItem
{
    int whats=WT_ROOT;
    TreeItem *Parent=nullptr;

public:
    explicit TreeItem(int whats=WT_ROOT, TreeItem *parent=nullptr){ this->whats=whats; Parent=parent; }
    ~TreeItem(){ qDeleteAll(Rows); }

    int WhatIsThis() const { return whats; }
    TreeItem *parentItem() const { return Parent; }

    void createStorage(int type, const QString &path);
    void createSoft(int type, const QString &name, const QStringList &filt);

    QVector<TreeItem *> Rows;
    QStringList Columns;
    QHash<int, QVariant> ExData;
};

#endif // TREEITEM_H
