#include "treemodel.h"
#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QDateTime>
#include "bootnamespase.h"

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

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
    switch(role)
    {
    case Qt::DisplayRole: return item->ColumnData(index.column());
    case Qt::WhatsThisRole:
        {
            switch(item->whatsThis())
            {
            default:
            case Boot::Root: return QString("Root");
            case Boot::Storage: return QString("Storage");
            case Boot::Device: return QString("Device");
            case Boot::File: return QString("File");
            }
        }
        break;
    case Boot::WhatsThis_UserRole: return item->whatsThis();
    case Boot::WhatsStorage_UserRole: return item->whatsStorage();
    case Boot::WhatsSoft_UserRole: return item->whatsSoft();
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

TreeItem * TreeModel::loadDeviceContent(Boot::WhatsSoft soft, TreeItem &storage_row, QDir device_dir, QStringList filters)
{
    TreeItem *device_row=new TreeItem(Soft2String(soft), TU(""), soft, device_dir, storage_row);
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
////Конструктор для Storage

void TreeModel::loadContent(TreeItem &parent, QDir curr_dir, QDir usb_dir)
{
    {//Носитель - КСУ
        TreeItem *storage_row=new TreeItem(Storage2String(Boot::KsuStorage), TU(""), Boot::KsuStorage, parent);
        Q_ASSERT(storage_row);
        parent.appendRow(storage_row);

        //Создаём и добавляем устройство
        {//ПО КСУ
            QDir dir=curr_dir;
            dir.mkdir("KSU"); dir.cd("KSU");
            QStringList filters; filters.append(QString("*.ksu"));
            TreeItem *device_row=loadDeviceContent(Boot::WorkKSU, *storage_row, dir, filters);
            storage_row->appendRow(device_row);
        }
        {//ПО КИ
            QDir dir=curr_dir;
            dir.mkdir("KI"); dir.cd("KI");
            QStringList filters; filters.append(QString("*.ki"));
            TreeItem *device_row=loadDeviceContent(Boot::KI, *storage_row, dir, filters);
            storage_row->appendRow(device_row);
        }
        {//ПО загрузчика КСУ
            QDir dir=curr_dir;
            dir.mkdir("LDR"); dir.cd("LDR");
            QStringList filters; filters.append(QString("*.ldr"));
            TreeItem *device_row=loadDeviceContent(Boot::LoaderKSU, *storage_row, dir, filters);
            storage_row->appendRow(device_row);
        }
    }


    {//Носитель - USB
        //Создаём и добавляем устройство
        TreeItem *storage_row=new TreeItem(Storage2String(Boot::UsbStorage), TU(""), Boot::UsbStorage, parent);
        Q_ASSERT(storage_row);
        parent.appendRow(storage_row);

        {//ПО КСУ
            QDir dir=usb_dir;
            dir.mkdir("KSU"); dir.cd("KSU");
            QStringList filters; filters.append(QString("*.ksu"));
            TreeItem *device_row=loadDeviceContent(Boot::WorkKSU, *storage_row, dir, filters);
            storage_row->appendRow(device_row);
        }
        {//ПО КИ
            QDir dir=usb_dir;
            dir.mkdir("KI"); dir.cd("KI");
            QStringList filters; filters.append(QString("*.ki"));
            TreeItem *device_row=loadDeviceContent(Boot::KI, *storage_row, dir, filters);
            storage_row->appendRow(device_row);
        }
        {//ПО КПТ
            QDir dir=usb_dir;
            dir.mkdir("KPT"); dir.cd("KPT");
            QStringList filters; filters.append(QString("*.kpt"));
            TreeItem *device_row=loadDeviceContent(Boot::KPT, *storage_row, dir, filters);
            storage_row->appendRow(device_row);
        }
        {//ПО загрузчика КСУ
            QDir dir=usb_dir;
            dir.mkdir("LDR"); dir.cd("LDR");
            QStringList filters; filters.append(QString("*.ldr"));
            TreeItem *device_row=loadDeviceContent(Boot::LoaderKSU, *storage_row, dir, filters);
            storage_row->appendRow(device_row);
        }
        {//Системное ПО КСУ
            QDir dir=usb_dir;
            dir.mkdir("KRNL"); dir.cd("KRNL");
            QStringList filters; filters.append(QString("*.krnl"));
            TreeItem *device_row=loadDeviceContent(Boot::SysKSU, *storage_row, dir, filters);
            storage_row->appendRow(device_row);
        }
    }
}

QString TreeModel::Soft2String(Boot::WhatsSoft soft)
{
    switch(soft)
    {
    default:
    case Boot::OtherSoft: return QString("");
    case Boot::WorkKSU  : return QString(TU("Рабочее ПО для блока КСУ"));
    case Boot::LoaderKSU: return QString(TU("ПО загрузчика блока КСУ"));
    case Boot::SysKSU   : return QString(TU("Системное ПО блока КСУ"));
    case Boot::KI       : return QString(TU("Рабочее ПО блока КИ"));
    case Boot::KPT      : return QString(TU("Рабочее ПО блока КПТ"));
    }
}

QString TreeModel::Storage2String(Boot::WhatsStorage storage)
{
    switch(storage)
    {
    default:
    case Boot::OtherSrorage: return QString("");
    case Boot::KsuStorage  : return QString(TU("Носитель: блок КСУ"));
    case Boot::UsbStorage  : return QString(TU("Носитель: USB-flash накопитель"));
    }
}

