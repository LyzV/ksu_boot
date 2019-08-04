#include "treemodel.h"
#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QDateTime>

#define TU(s) tr(s)

#define HEADER1 TU("ПРОШИВКА")
#define HEADER2 TU("ДАТА")

TreeModel::TreeModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    codec=QTextCodec::codecForName("CP1251");

    rootItem = new TreeItem;
    Q_ASSERT(rootItem);
    rootItem->Columns.append(HEADER1);
    rootItem->Columns.append(HEADER2);
    //loadContent(*rootItem, QApplication::applicationDirPath(), QApplication::applicationDirPath());

}

TreeModel::~TreeModel()
{
   delete rootItem;
}

#include <QFont>
QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    int column=index.column();
    if(2<=column) return QVariant();

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
    switch(role)
    {
    default: break;
    case Qt::DisplayRole: return item->Columns.at(index.column());
    case Qt::WhatsThisRole:
        {
            switch(item->WhatIsThis())
            {
            default: break;
            case WT_ROOT: return QString(WTS_ROOT);
            case WT_STORAGE: return QString(WTS_STORAGE);
            case WT_SOFT: return QString(WTS_SOFT);
            case WT_FILE: return QString(WTS_FILE);
            }
        }
        break;
    }

    return QVariant();
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index);
}

QVariant TreeModel::headerData(int column, Qt::Orientation orientation, int role) const
{
    if(2<=column) return QVariant();
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->Columns.at(column);
    return QVariant();
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if(2<=column) return QModelIndex();
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem;
    if (false==parent.isValid())  parentItem = rootItem;
    else                          parentItem = static_cast<TreeItem*>(parent.internalPointer());
    if(nullptr==parentItem) return QModelIndex();
    if(row>=parentItem->Rows.count()) return QModelIndex();

    TreeItem *childItem = parentItem->Rows[row];
    if(nullptr==childItem) return QModelIndex();

    return createIndex(row, column, childItem);
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
    if(parent.column()>0)
        return 0;

    if (!parent.isValid()) parentItem = rootItem;
    else                   parentItem = static_cast<TreeItem*>(parent.internalPointer());

    return parentItem->Rows.count();
}

int TreeModel::columnCount(const QModelIndex &parent) const
{
    if (false==parent.isValid()) return rootItem->Columns.count();
    return static_cast<TreeItem*>(parent.internalPointer())->Columns.count();
}

bool TreeModel::isSoftValid(int soft_type) const
{
    switch(soft_type)
    {
    default: return false;
    case SFT_KSUWORK:
    case SFT_KSUBOOT:
    case SFT_KI:
    case SFT_SYS: break;
    }
    return true;
}

bool TreeModel::isStorageValid(int storage_type) const
{
    switch(storage_type)
    {
    default: return false;
    case STT_KSU:
    case STT_USB: break;
    }
    return true;
}

void TreeModel::loadSoft(TreeItem &storage_item, int type, const QString &path, const QStringList &filt)
{
    if(WT_STORAGE!=storage_item.WhatIsThis()) return;
    if(false==isSoftValid(type)) return;

    TreeItem *soft_item=new TreeItem(WT_SOFT, &storage_item);
    if(nullptr==soft_item) return;
    soft_item->Columns.append(this->Soft2String(type));
    soft_item->Columns.append("");
    soft_item->ExData.insert(DK_PATH, path);
    soft_item->ExData.insert(DK_FILT, filt);

    QDir dir(path);
    if(false==dir.exists()) return;
    if(true==dir.isEmpty()) return;

    QFileInfoList file_list=dir.entryInfoList(filt);
    for(int i=0; i<file_list.count(); ++i)
    {
        TreeItem *file_item=new TreeItem(WT_FILE, soft_item);
        if(nullptr==file_item) return;
        file_item->Columns.append(file_list.at(i).fileName());
        QDateTime dt=file_list.at(i).created();//birthTime();
        file_item->Columns.append(dt.toString("yy/MM/dd hh:mm"));
    }
}

void TreeModel::loadContent(const QString &path, const QStringList &ksu_filt, const QStringList &ki_filt)
{

}

//TreeItem * TreeModel::loadDeviceContent(Boot::WhatsSoft soft, TreeItem &storage_row, QDir device_dir, QStringList filters)
//{
//    TreeItem *device_row=new TreeItem(Soft2String(soft), TU(""), soft, device_dir, storage_row);
//    Q_ASSERT(device_row);

//    QFileInfoList file_info_list=device_dir.entryInfoList(filters, QDir::Files);
//    for(int file_num=0; file_num<file_info_list.count(); ++file_num)
//    {
//        QFileInfo file_info(file_info_list[file_num]);
//        QDateTime dt=file_info.created();//birthTime();
//        TreeItem *file_row=new TreeItem(file_info.fileName(), dt.toString("yy/MM/dd hh:mm"), file_info, *device_row);
//        Q_ASSERT(file_row);
//        device_row->appendRow(file_row);
//    }
//    return device_row;
//}

//void TreeModel::loadContent(TreeItem &parent, QDir curr_dir, QDir usb_dir)
//{
//    {//Носитель - КСУ
//        TreeItem *storage_row=new TreeItem(Storage2String(Boot::KsuStorage), TU(""), Boot::KsuStorage, parent);
//        Q_ASSERT(storage_row);
//        parent.appendRow(storage_row);

//        //Создаём и добавляем устройство
//        {//ПО КСУ
//            QDir dir=curr_dir;
//            dir.mkdir("KSU"); dir.cd("KSU");
//            QStringList filters; filters.append(QString("*.ksu"));
//            TreeItem *device_row=loadDeviceContent(Boot::WorkKSU, *storage_row, dir, filters);
//            storage_row->appendRow(device_row);
//        }
//        {//ПО КИ
//            QDir dir=curr_dir;
//            dir.mkdir("KI"); dir.cd("KI");
//            QStringList filters; filters.append(QString("*.ki"));
//            TreeItem *device_row=loadDeviceContent(Boot::KI, *storage_row, dir, filters);
//            storage_row->appendRow(device_row);
//        }
//        {//ПО загрузчика КСУ
//            QDir dir=curr_dir;
//            dir.mkdir("LDR"); dir.cd("LDR");
//            QStringList filters; filters.append(QString("*.ldr"));
//            TreeItem *device_row=loadDeviceContent(Boot::LoaderKSU, *storage_row, dir, filters);
//            storage_row->appendRow(device_row);
//        }
//    }


//    {//Носитель - USB
//        //Создаём и добавляем устройство
//        TreeItem *storage_row=new TreeItem(Storage2String(Boot::UsbStorage), TU(""), Boot::UsbStorage, parent);
//        Q_ASSERT(storage_row);
//        parent.appendRow(storage_row);

//        {//ПО КСУ
//            QDir dir=usb_dir;
//            dir.mkdir("KSU"); dir.cd("KSU");
//            QStringList filters; filters.append(QString("*.ksu"));
//            TreeItem *device_row=loadDeviceContent(Boot::WorkKSU, *storage_row, dir, filters);
//            storage_row->appendRow(device_row);
//        }
//        {//ПО КИ
//            QDir dir=usb_dir;
//            dir.mkdir("KI"); dir.cd("KI");
//            QStringList filters; filters.append(QString("*.ki"));
//            TreeItem *device_row=loadDeviceContent(Boot::KI, *storage_row, dir, filters);
//            storage_row->appendRow(device_row);
//        }
//        {//ПО КПТ
//            QDir dir=usb_dir;
//            dir.mkdir("KPT"); dir.cd("KPT");
//            QStringList filters; filters.append(QString("*.kpt"));
//            TreeItem *device_row=loadDeviceContent(Boot::KPT, *storage_row, dir, filters);
//            storage_row->appendRow(device_row);
//        }
//        {//ПО загрузчика КСУ
//            QDir dir=usb_dir;
//            dir.mkdir("LDR"); dir.cd("LDR");
//            QStringList filters; filters.append(QString("*.ldr"));
//            TreeItem *device_row=loadDeviceContent(Boot::LoaderKSU, *storage_row, dir, filters);
//            storage_row->appendRow(device_row);
//        }
//        {//Системное ПО КСУ
//            QDir dir=usb_dir;
//            dir.mkdir("KRNL"); dir.cd("KRNL");
//            QStringList filters; filters.append(QString("*.krnl"));
//            TreeItem *device_row=loadDeviceContent(Boot::SysKSU, *storage_row, dir, filters);
//            storage_row->appendRow(device_row);
//        }
//    }
//}

QString TreeModel::Soft2String(int soft_type) const
{
    switch(soft_type)
    {
    case SFT_KSUWORK: return QString(TU("Рабочее ПО для блока КСУ"));
    case SFT_KSUBOOT: return QString(TU("ПО загрузчика блока КСУ"));
    case SFT_SYS: return QString(TU("Системное ПО блока КСУ"));
    case SFT_KI: return QString(TU("Рабочее ПО блока КИ"));
    }
    return QString("");
}

QString TreeModel::Storage2String(int storage_type) const
{
    switch(storage_type)
    {
    default: break;
    case STT_KSU: return QString(TU("Носитель: блок КСУ"));
    case STT_USB: return QString(TU("Носитель: USB-flash накопитель"));
    }
    return QString("");
}

