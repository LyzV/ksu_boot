#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QObject>
#include <QAbstractItemModel>
#include <QDir>
#include <QFile>
#include <QStringList>
#include <QTextCodec>
#include "treeitem.h"
#include "qusbnotifier.h"

class TreeModel : public QAbstractItemModel
{
    Q_OBJECT

    QUsbWorker *Worker=nullptr;
    QString workDirectory;

public:
    explicit TreeModel(const QString &workDirectory, QObject *parent = nullptr);
    ~TreeModel() override;
    bool Create(int &err);

static const QString sROOT;
static const QString sSTORAGE;
static const QString sSOFT;
static const QString sFILE;

    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int column, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    bool getFile(const QModelIndex &index,
                 int &storageType,
                 int &softType,
                 QString &filePath,
                 QString &fileName
                 ) const;

private:
    QString Soft2String(int soft_type) const;
    QString Storage2String(int storage_type) const;
    bool isSoftValid(int soft_type) const;
    bool isStorageValid(int storage_type) const;

    void loadSoft(TreeItem &storage_item, int type, const QString &path, const QStringList &filt);
    void loadContent(const QString &path,
                     const QStringList &ksuwork_filt,
                     const QStringList &ksuboot_filt,
                     const QStringList &ki_filt);
    void removeUsbContent(void);

    TreeItem *rootItem;
    QTextCodec *codec;//Translate from cp1251 to utf8
    QString mount;
    bool mount_flag;
    QStringList KsuWorkFilter, KsuBootFilter, KiFilter;

private slots:
    void MountSlot(const QString &mpoint, const QString &device, bool mount_flag);
};

#endif // TREEMODEL_H
