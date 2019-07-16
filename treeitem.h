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
    QVector<TreeItem *> Rows;//�������� ��������
    QVector<QVariant> Columns;//�������� �������� - ������
    TreeItem *Parent;//������ ��� ��������
    WhatsThis Whats;//��� �� �������: ���������; ����������; ����
    QFileInfo FileInfo;//������ � ����� (��� File)
    QDir Dir;//������ � ���������� (��� Storage, Device)
};

#endif // TREEITEM_H
