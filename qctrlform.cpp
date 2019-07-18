#include "qctrlform.h"
#include "ui_ctrl_form.h"
#include <QThread>
#include <QMessageBox>

#define TU(s) codec->toUnicode(s)

QCtrlForm::QCtrlForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CtrlForm)
{
    bool con;

    codec=QTextCodec::codecForName("CP1251");

    ui->setupUi(this);
    ui->errorLabel->hide();
    ui->doLabel->hide();
    ui->doProgBar->hide();

    ui->doProgBar->setMinimum(0);
    ui->doProgBar->setMaximum(100);

    con=QObject::connect(this->ui->exitButton, SIGNAL(pressed()), this, SLOT(close()));
    Q_ASSERT(con);
    con=QObject::connect(this->ui->progButton, SIGNAL(pressed()), this, SLOT(progSlot()));
    Q_ASSERT(con);
}

QCtrlForm::~QCtrlForm()
{
    delete ui;
}

int QCtrlForm::Exec(const QString &file)
{
    ui->fileLabel->setText(file);
    ui->errorLabel->hide();
    ui->doLabel->hide();
    ui->doProgBar->hide();
    ui->progButton->setFocus();
    return exec();
}

void QCtrlForm::progSlot()
{
    ui->doLabel->setText(TU("Программирую..."));
    ui->doProgBar->setValue(0);
    ui->doLabel->show();
    ui->doProgBar->show();
    QApplication::processEvents();
    for(int i=0; i<100; ++i)
    {
        ui->doProgBar->setValue(i);
        for(int j=0; j<10; ++j)
        {
            QApplication::processEvents();
            QThread::msleep(10);
        }
    }
    ui->doProgBar->setValue(ui->doProgBar->maximum());
    ui->errorLabel->show();
    ui->doLabel->setText(TU("Контроллер успешно запрограммирован!"));
}
