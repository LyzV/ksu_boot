#include "treeitem.h"

//Для корня
TreeItem::TreeItem(QString column1,
                   QString column2)
    : Parent(nullptr), Column1(column1), Column2(column2)
{
    WhatIsThis=Boot::Root;
    WhatIsStorage=Boot::OtherSrorage;
    WhatIsSoft=Boot::OtherSoft;
}

//Для хранилища
TreeItem::TreeItem(QString column1,
                   QString column2,
                   Boot::WhatsStorage storage,
                   TreeItem &parentItem)
    : Parent(&parentItem), Column1(column1), Column2(column2)
{
    WhatIsThis=Boot::Storage;
    WhatIsStorage=storage;
    WhatIsSoft=Boot::OtherSoft;
}

//Для устройства
TreeItem::TreeItem(QString column1,
                   QString column2,
                   Boot::WhatsSoft soft,
                   const QDir &dir,
                   TreeItem &parentItem)
    : Parent(&parentItem), Column1(column1), Column2(column2)
{
    WhatIsThis=Boot::Device;
    WhatIsStorage=parentItem.whatsStorage();
    WhatIsSoft=soft;
    Dir=dir;
}

//Для файла
TreeItem::TreeItem(QString column1,
                   QString column2,
                   const QFileInfo &file_info,
                   TreeItem &parentItem)
    : Parent(&parentItem), Column1(column1), Column2(column2)
{
    WhatIsThis=Boot::File;
    WhatIsStorage=parentItem.whatsStorage();;
    WhatIsSoft=parentItem.whatsSoft();
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

Boot::WhatsThis TreeItem::whatsThis() const
{
    return WhatIsThis;
}

Boot::WhatsStorage TreeItem::whatsStorage() const
{
    return WhatIsStorage;
}

Boot::WhatsSoft TreeItem::whatsSoft() const
{
    return WhatIsSoft;
}

const QFileInfo &TreeItem::fileInfo() const
{
    return FileInfo;
}

const QDir &TreeItem::dirInfo() const
{
    return Dir;
}
