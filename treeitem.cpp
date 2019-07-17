#include "treeitem.h"


TreeItem::TreeItem(QString column1,
                   QString column2)
    : Parent(nullptr), Column1(column1), Column2(column2)
{
    Whats=Root;
}

TreeItem::TreeItem(QString column1,
                   QString column2,
                   TreeItem &parentItem)
    : Parent(&parentItem), Column1(column1), Column2(column2)
{
    Whats=Storage;
}

TreeItem::TreeItem(QString column1,
                   QString column2,
                   const QDir &dir,
                   TreeItem &parentItem)
    : Parent(&parentItem), Column1(column1), Column2(column2)
{
    Whats=Device;
    Dir=dir;
}

TreeItem::TreeItem(QString column1,
                   QString column2,
                   const QFileInfo &file_info,
                   TreeItem &parentItem)
    : Parent(&parentItem), Column1(column1), Column2(column2)
{
    Whats=File;
    FileInfo=file_info;
}

TreeItem::~TreeItem()
{
    qDeleteAll(Rows);
}

void TreeItem::appendRow(TreeItem *row)
{
    if(nullptr!=row)
        Rows.append(row);
}

TreeItem *TreeItem::Row(int row_number)
{
    if (row_number < 0 || row_number >= Rows.size())
            return nullptr;
        return Rows.at(row_number);
}

int TreeItem::RowCount() const
{
    return Rows.count();
}

int TreeItem::ColumnCount() const
{
    return 2;
}

QVariant TreeItem::ColumnData(int column_number) const
{
    switch(column_number)
    {
    default: return QVariant();
    case  0: return Column1;
    case  1: return Column2;
    }
}

int TreeItem::RowNumber() const
{
    if (nullptr != Parent)
        return Parent->Rows.indexOf(const_cast<TreeItem*>(this));
    return 0;
}

TreeItem *TreeItem::parentItem()
{
    return Parent;
}

TreeItem::WhatsThis TreeItem::whatsThis() const
{
    return Whats;
}

const QFileInfo &TreeItem::fileInfo() const
{
    return FileInfo;
}

const QDir &TreeItem::dirInfo() const
{
    return Dir;
}
