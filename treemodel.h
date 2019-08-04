#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QObject>
#include <QAbstractItemModel>
#include <QDir>
#include <QTextCodec>
#include "treeitem.h"

#define WTS_ROOT    "Root"
#define WTS_STORAGE "Storage"
#define WTS_SOFT    "Soft"
#define WTS_FILE    "File"

class TreeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit TreeModel(QObject *parent = nullptr);
    ~TreeModel() override;

    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int column, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

private:
    QString Soft2String(int soft_type) const;
    QString Storage2String(int storage_type) const;
    bool isSoftValid(int soft_type) const;
    bool isStorageValid(int storage_type) const;

    void loadSoft(TreeItem &storage_item, int type, const QString &path, const QStringList &filt);
    void loadContent(const QString &path, const QStringList &ksu_filt, const QStringList &ki_filt);

    TreeItem *rootItem;
    QTextCodec *codec;//Translate from cp1251 to utf8
    QString mount;
    bool mount_flag;
};

#endif // TREEMODEL_H
