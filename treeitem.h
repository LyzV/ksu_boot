#ifndef TREEITEM_H
#define TREEITEM_H

#include <QVector>
#include <QVariant>
#include <QFileInfo>
#include <QDir>
#include <QString>

class TreeItem
{
public:
    enum WhatsThis
    {
        Root,
        Storage,
        Device,
        File
    };

    explicit TreeItem(WhatsThis whats_this,
                      const QVector<QVariant> &columns,
                      TreeItem *parentItem = nullptr);
    ~TreeItem();

    void appendRow(TreeItem *row);
    TreeItem *Row(int row_number);
    int RowCount() const;
    int ColumnCount() const;
    QVariant ColumnData(int column_number) const;
    int RowNumber() const;
    TreeItem *parentItem();
    QString whatsThis() const;
    QFileInfo fileInfo() const;

private:
    QVector<TreeItem *> Rows;//Дочерние элементы
    QVector<QVariant> Columns;//Дочерние элементы - листья
    TreeItem *Parent;//Ссылка над родителя
    WhatsThis Whats;//Что за элемент: хранилище; устройство; файл
    QFileInfo FileInfo;//Данные о файле (для File)
    QDir Dir;//Данные о директории (для Storage, Device)
};

#endif // TREEITEM_H
