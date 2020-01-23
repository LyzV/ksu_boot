#include "treemodel.h"
#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QDateTime>
#include <QRegExp>

#define TU(s) codec->toUnicode(s) //tr(s)

const QString TreeModel::sROOT(tr("Root"));
const QString TreeModel::sSTORAGE(tr("Storage"));
const QString TreeModel::sSOFT(tr("Soft"));
const QString TreeModel::sFILE(tr("File"));

#define HEADER1 TU("œ–Œÿ»¬ ¿")
#define HEADER2 TU("ƒ¿“¿")

TreeModel::TreeModel(const QString &workDirectory, QObject *parent)
    : QAbstractItemModel(parent)
{
    this->workDirectory=workDirectory;
    codec=QTextCodec::codecForName("CP1251");

    rootItem = new TreeItem;
    Q_ASSERT(rootItem);
    rootItem->Columns.append(HEADER1);
    rootItem->Columns.append(HEADER2);
}

TreeModel::~TreeModel()
{
    if(nullptr==rootItem) delete rootItem;
    if(nullptr==Worker) delete Worker;
}

bool TreeModel::Create(const QString &distFilter, int &err)
{
    this->distFilter=distFilter;

    KsuWorkFilter.append("*." + distFilter + ".ksu");
    KsuBootFilter.append("*." + distFilter + ".ldr");
    KiFilter.append("*.ki");
    loadContent(this->workDirectory,
                KsuWorkFilter,
                KsuBootFilter,
                KiFilter);
    layoutChanged();

    QUsbWorker *Worker=new QUsbWorker;
    if(nullptr==Worker)
    {
        err=TASK_MEMORY_ERR;
        return false;
    }
    Controller *ctrl=new Controller(this);
    if(nullptr==ctrl)
    {
        delete Worker;
        Worker=nullptr;
        err=TASK_MEMORY_ERR;
        return false;
    }
    if(false==ctrl->Create(Worker, err))
    {
        delete Worker;
        Worker=nullptr;
        return false;
    }
    bool ret=ctrl->WorkerStart(QThread::NormalPriority, nullptr, err);
    if(true==ret)
    {
        connect(Worker, SIGNAL(MountSignal(QString,QString,bool)),
                this  , SLOT  (MountSlot  (QString,QString,bool)));
    }
    return ret;
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

bool TreeModel::getFile(const QModelIndex &index, int &storageType, int &softType, QString &filePath, QString &fileName) const
{
    if(!index.isValid())
        return false;

    const TreeItem *fileItem=reinterpret_cast<const TreeItem *>(index.internalPointer());
    if(nullptr==fileItem) return false;
    if(WT_FILE!=fileItem->WhatIsThis()) return false;
    fileName=fileItem->Columns.at(0);

    const TreeItem *softItem=fileItem->parentItem();
    if(nullptr==softItem) return false;
    if(WT_SOFT!=softItem->WhatIsThis()) return false;
    softType=softItem->ExData[DK_TYPE].toInt();
    filePath=softItem->ExData[DK_PATH].toString();

    const TreeItem *storageItem=softItem->parentItem();
    if(nullptr==storageItem) return false;
    if(WT_STORAGE!=storageItem->WhatIsThis()) return false;
    storageType=storageItem->ExData[DK_TYPE].toInt();

    return true;
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
    soft_item->createSoft(type, path, filt);

    QDir dir(path);
    if(false==dir.exists()) return;

    QFileInfoList file_list=dir.entryInfoList(filt);
    for(int i=0; i<file_list.count(); ++i)
    {
        TreeItem *file_item=new TreeItem(WT_FILE, soft_item);
        if(nullptr==file_item) return;
        file_item->Columns.append(file_list.at(i).fileName());
        QDateTime dt=file_list.at(i).created();
        //QDateTime dt=file_list.at(i).birthTime();
        file_item->Columns.append(dt.toString("yy/MM/dd hh:mm"));
    }
}

void TreeModel::loadContent(const QString &path,
                            const QStringList &ksuwork_filt,
                            const QStringList &ksuboot_filt,
                            const QStringList &ki_filt)
{
    beginResetModel();

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

    endResetModel();
}

void TreeModel::removeUsbContent(void)
{
    if(2>rootItem->Rows.count()) return;
    int row=rootItem->Rows.count()-1;
    TreeItem *storage_item=rootItem->Rows.at(row);
    if(nullptr!=storage_item)
    {
        if(Storage2String(STT_USB)!=storage_item->Columns[0])
            return;
    }

    beginResetModel();
    rootItem->Rows.remove(row);
    delete storage_item;
    endResetModel();
}

void TreeModel::MountSlot(const QString &mpoint, const QString &device, bool mount_flag)
{
    Q_UNUSED(device);

    if(true==mount_flag)
    {//mounted
        loadContent(mpoint,
                    KsuWorkFilter,
                    KsuBootFilter,
                    KiFilter);
    }
    else
    {//unmounted
        removeUsbContent();
    }
}

QString TreeModel::Soft2String(int soft_type) const
{
    switch(soft_type)
    {
    case SFT_KSUWORK: return QString(TU("–¿¡Œ◊≈≈ œŒ ¡ÀŒ ¿  —”:"));
    case SFT_KSUBOOT: return QString(TU("«¿√–”«◊»  ¡ÀŒ ¿  —”:"));
    case SFT_SYS: return QString(TU("—»—“≈ÃÕŒ≈ œŒ ¡ÀŒ ¿  —”:"));
    case SFT_KI: return QString(TU("–¿¡Œ◊≈≈ œŒ ¡ÀŒ ¿  »:"));
    }
    return QString("");
}

QString TreeModel::Storage2String(int storage_type) const
{
    switch(storage_type)
    {
    default: break;
    case STT_KSU: return QString(TU("¡ÀŒ   —”:"));
    case STT_USB: return QString(TU("USB-FLASH ÕŒ—»“≈À‹:"));
    }
    return QString("");
}

