#include "treemodel.h"
#include <QApplication>
#include <QDir>
#include <QFile>
#include <QFileInfoList>

#define TU(s) codec->toUnicode(s)

TreeModel::TreeModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    codec=QTextCodec::codecForName("CP1251");
    QString col1=codec->toUnicode("Прошивка");
    QString col2=codec->toUnicode("Дата");

    rootItem = new TreeItem({ col1, col2});
    loadContent(rootItem, QApplication::applicationDirPath(), QApplication::applicationDirPath());

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
            return item->data(index.column());
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
        return rootItem->data(section);

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

    TreeItem *childItem = parentItem->child(row);
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

    return createIndex(parentItem->row(), 0, parentItem);
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

    return parentItem->childCount();
}

int TreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    return rootItem->columnCount();
}

#include <QStringList>
#include <QFileInfo>
#include <QFileInfoList>
#include <QDateTime>
void TreeModel::loadContent(TreeItem *parent, QDir curr_dir, QDir usb_dir)
{

    {//Носитель - КСУ
        //Создаём и добавляем устройство
        QVector<QVariant> storage_vector(1);
        storage_vector[0]=TU("Носитель: блок КСУ");
        TreeItem *storage_item=new TreeItem(storage_vector, parent);
        parent->appendChild(storage_item);

        {//ПО КСУ
            QDir dir=curr_dir;

            QVector<QVariant> device_vector(2);
            device_vector[0]=TU("Рабочее ПО для блока КСУ");
            device_vector[1]="--";
            TreeItem *device_item=new TreeItem(device_vector, storage_item);
            storage_item->appendChild(device_item);

            dir.mkdir("KSU"); dir.cd("KSU");
            QStringList filters; filters.append(QString("*.ksu"));
            QFileInfoList file_info_list=dir.entryInfoList(filters, QDir::Files);
            for(int file_num=0; file_num<file_info_list.count(); ++file_num)
            {
                QVector<QVariant> file_vector(2);
                file_vector[0]=file_info_list[file_num].fileName();
                QDateTime dt=file_info_list[file_num].created();
                file_vector[1]=dt.toString("yy/MM/dd hh:mm");
                TreeItem *file_item=new TreeItem(file_vector, device_item);
                device_item->appendChild(file_item);
            }
        }
        {//ПО КИ
            QDir dir=curr_dir;

            QVector<QVariant> device_vector(2);
            device_vector[0]=TU("ПО для блока КИ");
            device_vector[1]="--";
            TreeItem *device_item=new TreeItem(device_vector, storage_item);
            storage_item->appendChild(device_item);

            dir.mkdir("KI"); dir.cd("KI");
            QStringList filters; filters.append(QString("*.ki"));
            QFileInfoList file_info_list=dir.entryInfoList(filters, QDir::Files);
            for(int file_num=0; file_num<file_info_list.count(); ++file_num)
            {
                QVector<QVariant> file_vector(2);
                file_vector[0]=file_info_list[file_num].fileName();
                QDateTime dt=file_info_list[file_num].created();
                file_vector[1]=dt.toString("yy/MM/dd hh:mm");
                TreeItem *file_item=new TreeItem(file_vector, device_item);
                device_item->appendChild(file_item);
            }
        }
        {//ПО загрузчика КСУ
            QDir dir=curr_dir;

            QVector<QVariant> device_vector(2);
            device_vector[0]=TU("ПО загрузчика блока КСУ");
            device_vector[1]="--";
            TreeItem *device_item=new TreeItem(device_vector, storage_item);
            storage_item->appendChild(device_item);

            dir.mkdir("LDR"); dir.cd("LDR");
            QStringList filters; filters.append(QString("*.ldr"));
            QFileInfoList file_info_list=dir.entryInfoList(filters, QDir::Files);
            for(int file_num=0; file_num<file_info_list.count(); ++file_num)
            {
                QVector<QVariant> file_vector(2);
                file_vector[0]=file_info_list[file_num].fileName();
                QDateTime dt=file_info_list[file_num].created();
                file_vector[1]=dt.toString("yy/MM/dd hh:mm");
                TreeItem *file_item=new TreeItem(file_vector, device_item);
                device_item->appendChild(file_item);
            }
        }
    }




    {//Носитель - USB
        //Создаём и добавляем устройство
        QVector<QVariant> storage_vector(1);
        storage_vector[0]=TU("Носитель: USB-flash накопитель");
        TreeItem *storage_item=new TreeItem(storage_vector, parent);
        parent->appendChild(storage_item);

        {//ПО КСУ
            QDir dir=usb_dir;

            QVector<QVariant> device_vector(2);
            device_vector[0]=TU("Рабочее ПО для блока КСУ");
            device_vector[1]="--";
            TreeItem *device_item=new TreeItem(device_vector, storage_item);
            storage_item->appendChild(device_item);

            dir.mkdir("KSU"); dir.cd("KSU");
            QStringList filters; filters.append(QString("*.ksu"));
            QFileInfoList file_info_list=dir.entryInfoList(filters, QDir::Files);
            for(int file_num=0; file_num<file_info_list.count(); ++file_num)
            {
                QVector<QVariant> file_vector(2);
                file_vector[0]=file_info_list[file_num].fileName();
                QDateTime dt=file_info_list[file_num].created();
                file_vector[1]=dt.toString("yy/MM/dd hh:mm");
                TreeItem *file_item=new TreeItem(file_vector, device_item);
                device_item->appendChild(file_item);
            }
        }
        {//ПО КИ
            QDir dir=usb_dir;

            QVector<QVariant> device_vector(2);
            device_vector[0]=TU("ПО для блока КИ");
            device_vector[1]="--";
            TreeItem *device_item=new TreeItem(device_vector, storage_item);
            storage_item->appendChild(device_item);

            dir.mkdir("KI"); dir.cd("KI");
            QStringList filters; filters.append(QString("*.ki"));
            QFileInfoList file_info_list=dir.entryInfoList(filters, QDir::Files);
            for(int file_num=0; file_num<file_info_list.count(); ++file_num)
            {
                QVector<QVariant> file_vector(2);
                file_vector[0]=file_info_list[file_num].fileName();
                QDateTime dt=file_info_list[file_num].created();
                file_vector[1]=dt.toString("yy/MM/dd hh:mm");
                TreeItem *file_item=new TreeItem(file_vector, device_item);
                device_item->appendChild(file_item);
            }
        }
        {//ПО загрузчика КСУ
            QDir dir=usb_dir;

            QVector<QVariant> device_vector(2);
            device_vector[0]=TU("ПО загрузчика блока КСУ");
            device_vector[1]="--";
            TreeItem *device_item=new TreeItem(device_vector, storage_item);
            storage_item->appendChild(device_item);

            dir.mkdir("LDR"); dir.cd("LDR");
            QStringList filters; filters.append(QString("*.ldr"));
            QFileInfoList file_info_list=dir.entryInfoList(filters, QDir::Files);
            for(int file_num=0; file_num<file_info_list.count(); ++file_num)
            {
                QVector<QVariant> file_vector(2);
                file_vector[0]=file_info_list[file_num].fileName();
                QDateTime dt=file_info_list[file_num].created();
                file_vector[1]=dt.toString("yy/MM/dd hh:mm");
                TreeItem *file_item=new TreeItem(file_vector, device_item);
                device_item->appendChild(file_item);
            }
        }
        {//Системное ПО КСУ
            QDir dir=usb_dir;

            QVector<QVariant> device_vector(2);
            device_vector[0]=TU("Системное ПО блока КСУ");
            device_vector[1]="--";
            TreeItem *device_item=new TreeItem(device_vector, storage_item);
            storage_item->appendChild(device_item);

            dir.mkdir("KRNL"); dir.cd("KRNL");
            QStringList filters; filters.append(QString("*.krnl"));
            QFileInfoList file_info_list=dir.entryInfoList(filters, QDir::Files);
            for(int file_num=0; file_num<file_info_list.count(); ++file_num)
            {
                QVector<QVariant> file_vector(2);
                file_vector[0]=file_info_list[file_num].fileName();
                QDateTime dt=file_info_list[file_num].created();
                file_vector[1]=dt.toString("yy/MM/dd hh:mm");
                TreeItem *file_item=new TreeItem(file_vector, device_item);
                device_item->appendChild(file_item);
            }
        }
    }
}

