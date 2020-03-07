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
    QTimer *clearTimer=nullptr;
    Ui::entryDialog *ui=nullptr;

public:
    explicit QEntryDialog(QWidget *parent = 0);
    ~QEntryDialog();

    virtual int exec();

private slots:
    void clearSlot(void);
};


#endif // QENTRYDIALOG_H
