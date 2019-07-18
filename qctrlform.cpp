#include "qctrlform.h"
#include "ui_ctrl_form.h"

QCtrlForm::QCtrlForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CtrlForm)
{
    ui->setupUi(this);
}

QCtrlForm::~QCtrlForm()
{
    delete ui;
}
