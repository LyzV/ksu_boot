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

    //����������� ��� Root
    explicit TreeItem(QString column1,
                      QString column2);
    //����������� ��� Storage
    explicit TreeItem(QString column1,
                      QString column2,
                      TreeItem &parentItem);
    //����������� ��� Device
    explicit TreeItem(QString column1,
                      QString column2,
                      const QDir &dir,
                      TreeItem &parentItem);
    //����������� ��� File
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
    QVector<TreeItem *> Rows;//�������� ��������
    TreeItem *Parent;//������ ��� ��������

    QString Column1="";//� ��� ������ 2 �������
    QString Column2="";//� ��� ������ 2 �������
    WhatsThis Whats;//��� �� �������: ������; ���������; ����������; ����.
    QFileInfo FileInfo;//������ � ����� (��� File)
    QDir Dir;//������ � ���������� (��� Storage, Device)
};

#endif // TREEITEM_H
