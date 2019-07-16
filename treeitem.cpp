#include "treeitem.h"

TreeItem::TreeItem(const QVector<QVariant> &columns, TreeItem *parentItem)
    : Columns(columns), Parent(parentItem)
{

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
    return Columns.count();
}

QVariant TreeItem::ColumnData(int column_number) const
{
    if (column_number < 0 || column_number >= Columns.size())
        return QVariant();
    return Columns.at(column_number);
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
