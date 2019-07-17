#include "treemodel.h"
#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QDateTime>

#define TU(s) codec->toUnicode(s)

TreeModel::TreeModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    codec=QTextCodec::codecForName("CP1251");

    rootItem = new TreeItem(TU("ПРОШИВКА"), TU("ДАТА"));
    Q_ASSERT(rootItem);
    loadContent(*rootItem, QApplication::applicationDirPath(), QApplication::applicationDirPath());

}

TreeModel::~TreeModel()
{
    //saveSettings(QApplication::applicationDirPath());
    delete rootItem;
}

#include <QFont>
QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch(role)
    {
    case Qt::DisplayRole:
        {
            TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
            return item->ColumnData(index.column());
        }
        break;
    case Qt::WhatsThisRole:
        {
            TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
            switch(item->whatsThis())
            {
            case TreeItem::Root:
            default: return QString("Root");

            case TreeItem::Storage: return QString("Storage");
            case TreeItem::Device: return QString("Device");
            case TreeItem::File: return QString("File");
            }
        }
        break;
    default: return QVariant();
    }


}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index);
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->ColumnData(section);

    return QVariant();
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem *childItem = parentItem->Row(row);
    if (childItem)
        return createIndex(row, column, childItem);
    return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parentItem();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->RowNumber(), 0, parentItem);
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    return parentItem->RowCount();
}

int TreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<TreeItem*>(parent.internalPointer())->ColumnCount();
    return rootItem->ColumnCount();
}

TreeItem * TreeModel::loadDeviceContent(const QString &column1, TreeItem &storage_row, QDir device_dir, QStringList filters)
{
    TreeItem *device_row=new TreeItem(column1, TU(""), device_dir, storage_row);
    Q_ASSERT(device_row);

    QFileInfoList file_info_list=device_dir.entryInfoList(filters, QDir::Files);
    for(int file_num=0; file_num<file_info_list.count(); ++file_num)
    {
        QFileInfo file_info(file_info_list[file_num]);
        QDateTime dt=file_info.created();
        TreeItem *file_row=new TreeItem(file_info.fileName(), dt.toString("yy/MM/dd hh:mm"), file_info, *device_row);
        Q_ASSERT(file_row);
        device_row->appendRow(file_row);
    }
    return device_row;
}

void TreeModel::loadContent(TreeItem &parent, QDir curr_dir, QDir usb_dir)
{
    {//Носитель - КСУ
        //Создаём и добавляем устройство
        TreeItem *storage_row=new TreeItem(TU("Носитель: блок КСУ"), TU(""), parent);
        Q_ASSERT(storage_row);
        parent.appendRow(storage_row);

        {//ПО КСУ
            QDir dir=curr_dir;
            dir.mkdir("KSU"); dir.cd("KSU");
            QStringList filters; filters.append(QString("*.ksu"));
            TreeItem *device_row=loadDeviceContent(TU("Рабочее ПО для блока КСУ"), *storage_row, dir, filters);
            storage_row->appendRow(device_row);
        }
        {//ПО КИ
            QDir dir=curr_dir;
            dir.mkdir("KI"); dir.cd("KI");
            QStringList filters; filters.append(QString("*.ki"));
            TreeItem *device_row=loadDeviceContent(TU("ПО для блока КИ"), *storage_row, dir, filters);
            storage_row->appendRow(device_row);
        }
        {//ПО загрузчика КСУ
            QDir dir=curr_dir;
            dir.mkdir("LDR"); dir.cd("LDR");
            QStringList filters; filters.append(QString("*.ldr"));
            TreeItem *device_row=loadDeviceContent(TU("ПО загрузчика блока КСУ"), *storage_row, dir, filters);
            storage_row->appendRow(device_row);
        }
    }


    {//Носитель - USB
        //Создаём и добавляем устройство
        TreeItem *storage_row=new TreeItem(TU("Носитель: USB-flash накопитель"), TU(""), parent);
        Q_ASSERT(storage_row);
        parent.appendRow(storage_row);

        {//ПО КСУ
            QDir dir=usb_dir;
            dir.mkdir("KSU"); dir.cd("KSU");
            QStringList filters; filters.append(QString("*.ksu"));
            TreeItem *device_row=loadDeviceContent(TU("Рабочее ПО для блока КСУ"), *storage_row, dir, filters);
            storage_row->appendRow(device_row);
        }
        {//ПО КИ
            QDir dir=usb_dir;
            dir.mkdir("KI"); dir.cd("KI");
            QStringList filters; filters.append(QString("*.ki"));
            TreeItem *device_row=loadDeviceContent(TU("ПО для блока КИ"), *storage_row, dir, filters);
            storage_row->appendRow(device_row);
        }
        {//ПО загрузчика КСУ
            QDir dir=usb_dir;
            dir.mkdir("LDR"); dir.cd("LDR");
            QStringList filters; filters.append(QString("*.ldr"));
            TreeItem *device_row=loadDeviceContent(TU("ПО загрузчика блока КСУ"), *storage_row, dir, filters);
            storage_row->appendRow(device_row);
        }
        {//Системное ПО КСУ
            QDir dir=usb_dir;
            dir.mkdir("KRNL"); dir.cd("KRNL");
            QStringList filters; filters.append(QString("*.krnl"));
            TreeItem *device_row=loadDeviceContent(TU("Системное ПО блока КСУ"), *storage_row, dir, filters);
            storage_row->appendRow(device_row);
        }
    }
}

