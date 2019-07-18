#ifndef QCTRLFORM_H
#define QCTRLFORM_H

#include <QDialog>
#include <QTextCodec>
#include <QString>

namespace Ui { class CtrlForm; }

class QCtrlForm: protected QDialog
{
    Q_OBJECT
public:
    explicit QCtrlForm(QWidget *parent=nullptr);
    ~QCtrlForm();

    int Exec(const QString &file);

private:
    Ui::CtrlForm *ui;
    QTextCodec *codec;
private slots:
    void progSlot(void);
};

#endif // QCTRLFORM_H
