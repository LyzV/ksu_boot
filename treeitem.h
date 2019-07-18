#ifndef TREEITEM_H
#define TREEITEM_H

#include <QVector>
#include <QVariant>
#include <QFileInfo>
#include <QDir>
#include <QString>
#include "bootnamespase.h"

class TreeItem
{
public:
    //����������� ��� Root
    explicit TreeItem(QString column1,
                      QString column2);
    //����������� ��� Storage
    explicit TreeItem(QString column1,
                      QString column2,
                      Boot::WhatsStorage storage,
                      TreeItem &parentItem);
    //����������� ��� Device
    explicit TreeItem(QString column1,
                      QString column2,
                      Boot::WhatsSoft soft,
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
    Boot::WhatsThis whatsThis() const;
    Boot::WhatsStorage whatsStorage() const;
    Boot::WhatsSoft whatsSoft() const;
    const QFileInfo &fileInfo() const;
    const QDir &dirInfo() const;



private:
    QVector<TreeItem *> Rows;//�������� ��������
    TreeItem *Parent;//������ ��� ��������

    QString Column1="";//� ��� ������ 2 �������
    QString Column2="";//� ��� ������ 2 �������
    Boot::WhatsStorage WhatIsStorage;//��� �� ��������: ���� ���; USB-����
    Boot::WhatsSoft WhatIsSoft;//��� �� ��: ������� �� ���; �� ���������� ���; ��������� ��; �� ����� ��; �� ����� ���
    Boot::WhatsThis WhatIsThis;//��� �� �������: ������; ���������; ����������; ����.
    QFileInfo FileInfo;//������ � ����� (��� File)
    QDir Dir;//������ � ���������� (��� Storage, Device)
};

#endif // TREEITEM_H
