#ifndef TREEITEM_H
#define TREEITEM_H

#include <QVector>
#include <QVariant>

class TreeItem
{
public:
    explicit TreeItem(const QVector<QVariant> &columns, TreeItem *parentItem = nullptr);
    ~TreeItem();

    void appendRow(TreeItem *row);
    TreeItem *Row(int row_number);
    int RowCount() const;
    int ColumnCount() const;
    QVariant ColumnData(int column_number) const;
    int RowNumber() const;
    TreeItem *parentItem();

private:
    QVector<TreeItem *> Rows;//�������� �������� (�� ������� ����� ���� ������)
    QVector<QVariant> Columns;//�������� �������� - ������
    TreeItem *Parent;//������ ��� ��������
};

#endif // TREEITEM_H
