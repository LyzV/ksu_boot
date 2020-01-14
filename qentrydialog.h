#ifndef QENTRYDIALOG_H
#define QENTRYDIALOG_H

#include <QDialog>
#include <QTextCodec>
#include <QString>
#include <QTimer>

namespace Ui { class entryDialog; }

class QEntryDialog: public QDialog
{
    Q_OBJECT

    QTimer *timer=nullptr;
    Ui::entryDialog *ui=nullptr;

public:
    explicit QEntryDialog(QWidget *parent = 0);
    ~QEntryDialog();

    virtual int exec();
};


#endif // QENTRYDIALOG_H
