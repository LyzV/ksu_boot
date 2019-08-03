#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QObject>
#include <QAbstractItemModel>
#include <QDir>
#include <QTextCodec>
#include "treeitem.h"

class TreeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit TreeModel(QObject *parent = nullptr);
    ~TreeModel() override;

    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

private:
    QString Soft2String(int soft_type);
    QString Storage2String(int storage_type);

    TreeItem *rootItem;
    QTextCodec *codec;//Translate from cp1251 to utf8
};

#endif // TREEMODEL_H
