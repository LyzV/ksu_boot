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

    //Конструктор для Root
    explicit TreeItem(QString column1,
                      QString column2);
    //Конструктор для Storage
    explicit TreeItem(QString column1,
                      QString column2,
                      TreeItem &parentItem);
    //Конструктор для Device
    explicit TreeItem(QString column1,
                      QString column2,
                      const QDir &dir,
                      TreeItem &parentItem);
    //Конструктор для File
    explicit TreeItem(QString column1,
                      QString column2,
                      const QFileInfo &file_info,
                      TreeItem &parentItem);
    ~TreeItem();

    void appendRow(TreeItem *row);
    TreeItem *Row(int row_number);
    int RowCount() const;
    int ColumnCount() const;
    QVariant ColumnData(int column_number) const;
    int RowNumber() const;
    TreeItem *parentItem();
    WhatsThis whatsThis() const;
    const QFileInfo &fileInfo() const;
    const QDir &dirInfo() const;

private:
    QVector<TreeItem *> Rows;//Дочерние элементы
    TreeItem *Parent;//Ссылка над родителя

    QString Column1="";//У нас только 2 колонки
    QString Column2="";//У нас только 2 колонки
    WhatsThis Whats;//Что за элемент: корень; хранилище; устройство; файл.
    QFileInfo FileInfo;//Данные о файле (для File)
    QDir Dir;//Данные о директории (для Storage, Device)
};

#endif // TREEITEM_H
