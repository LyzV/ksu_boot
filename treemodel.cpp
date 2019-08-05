#include "treemodel.h"
#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QDateTime>
#include <QRegExp>

#define TU(s) tr(s)

const QString TreeModel::sROOT(tr("Root"));
const QString TreeModel::sSTORAGE(tr("Storage"));
const QString TreeModel::sSOFT(tr("Soft"));
const QString TreeModel::sFILE(tr("File"));

#define HEADER1 TU("ПРОШИВКА")
#define HEADER2 TU("ДАТА")

TreeModel::TreeModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    codec=QTextCodec::codecForName("CP1251");

    KsuWorkFilter.append("*.ksu");
    KsuBootFilter.append("*.ldr");
    KiFilter.append("*.ki");

    layoutAboutToBeChanged();
    rootItem = new TreeItem;
    Q_ASSERT(rootItem);
    rootItem->Columns.append(HEADER1);
    rootItem->Columns.append(HEADER2);

    loadContent("/home/root",
                KsuWorkFilter,
                KsuBootFilter,
                KiFilter);
    layoutChanged();

}

TreeModel::~TreeModel()
{
    delete rootItem;
}

bool TreeModel::Create(int &err)
{
    return UsbNotifier.Start(err);
}

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
            case WT_ROOT: return QString(sROOT);
            case WT_STORAGE: return QString(sSTORAGE);
            case WT_SOFT: return QString(sSOFT);
            case WT_FILE: return QString(sFILE);
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

void TreeModel::loadContent(const QString &path,
                            const QStringList &ksuwork_filt,
                            const QStringList &ksuboot_filt,
                            const QStringList &ki_filt)
{
    TreeItem *storage_item=new TreeItem(WT_STORAGE, rootItem);
    if(nullptr==storage_item) return;

    int storage_type;
    QRegExp exp("/media/usb-sd[a-z][0-9]");
    if(true==exp.exactMatch(path)) storage_type=STT_USB;
    else                           storage_type=STT_KSU;

    storage_item->Columns.append(Storage2String(storage_type));
    storage_item->Columns.append("");
    storage_item->createStorage(storage_type, path);

    QString full_path=path+"/RIMERA/FIRMWARE";
    QDir dir(full_path);
    if(false==dir.exists()) return;

    loadSoft(*storage_item, SFT_KSUWORK , full_path, ksuwork_filt);
    loadSoft(*storage_item, SFT_KSUBOOT , full_path, ksuboot_filt);
    loadSoft(*storage_item, SFT_KI      , full_path, ki_filt     );
}

QString TreeModel::Soft2String(int soft_type) const
{
    switch(soft_type)
    {
    case SFT_KSUWORK: return QString(TU("Рабочее ПО блока КСУ:"));
    case SFT_KSUBOOT: return QString(TU("ПО загрузчика блока КСУ:"));
    case SFT_SYS: return QString(TU("Системное ПО:"));
    case SFT_KI: return QString(TU("Рабочее ПО блока КИ:"));
    }
    return QString("");
}

QString TreeModel::Storage2String(int storage_type) const
{
    switch(storage_type)
    {
    default: break;
    case STT_KSU: return QString(TU("Блок КСУ:"));
    case STT_USB: return QString(TU("USB-flash накопитель:"));
    }
    return QString("");
}

