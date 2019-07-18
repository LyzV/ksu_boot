#ifndef QCTRLFORM_H
#define QCTRLFORM_H

#include <QWidget>

namespace Ui { class CtrlForm; }

class QCtrlForm: public QWidget
{
    Q_OBJECT
public:
    explicit QCtrlForm(QWidget *parent=nullptr);
    ~QCtrlForm();

private:
    Ui::CtrlForm *ui;
};

#endif // QCTRLFORM_H
