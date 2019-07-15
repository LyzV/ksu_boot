#ifndef TREEITEM_H
#define TREEITEM_H

#include <QVector>
#include <QVariant>

class TreeItem
{
public:
    explicit TreeItem(const QVector<QVariant> &data, TreeItem *parentItem = nullptr);
    ~TreeItem();

    void appendChild(TreeItem *child);
    TreeItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    TreeItem *parentItem();

private:
    QVector<TreeItem *> m_childItems;//Дочерние элементы (те которые имеют свои листья)
    QVector<QVariant> m_itemData;//Дочерние элементы - листья
    TreeItem *m_parentItem;//Ссылка над родителя
};

#endif // TREEITEM_H
