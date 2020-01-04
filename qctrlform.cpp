#include "qctrlform.h"
#include "ui_ctrl_form.h"
#include "bash_cmd.h"
#include <QThread>
#include <QFile>
#include <QMessageBox>
#include "treeitem.h"

#define TU(s) codec->toUnicode((s))

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
    con=QObject::connect(this->ui->delButton, SIGNAL(pressed()), this, SLOT(deleteSlot());
    Q_ASSERT(con);
}

QCtrlForm::~QCtrlForm()
{
    delete ui;
}

int QCtrlForm::Exec(int &storageType, int &softType, QString &filePath, QString &fileName)
{
    this->storageType=storageType;
    this->softType=softType;
    this->filePath=filePath;
    this->fileName=fileName;


    ui->fileLabel->setText(TU("¬€¡–¿Õ ‘¿…À: ")+this->fileName);
    ui->errorLabel->hide();
    ui->doLabel->hide();
    ui->doProgBar->hide();
    ui->progButton->setFocus();
    return exec();
}

bool QCtrlForm::copySoft(const QString &from, const QString &to)
{
    QFile fromFile(from);
    if(false==fromFile.exists())
        return false;
    QFile toFile(to);
    if(true==toFile.exists())
    {
        if(false==toFile.remove())
            return false;
    }
    if(false==fromFile.copy(to))
        return false;

    return true;
}

void QCtrlForm::progSlot()
{
    ui->errorLabel->hide();
    ui->doLabel->setText(TU("œ–Œ√–¿ÃÃ»–”ﬁ..."));
    ui->doLabel->show();
    ui->doProgBar->setValue(0);
    ui->doProgBar->show();
    QApplication::processEvents();
    if(STT_USB==storageType)
    {
        switch(softType)
        {
        default:
        case SFT_KSUWORK:
            {
                ui->doProgBar->setValue(10);
                QApplication::processEvents();
                QString fullFileName=filePath+"/"+fileName;
                if(true==copySoft(fullFileName, "/home/root/ksu"))
                {
                    QBashCmd::set_file_execute_mode(fullFileName);
                    QBashCmd::flush_storage();
                    ui->doLabel->setText(TU("”—œ≈ÿÕŒ «¿œ–Œ√–¿ÃÃ»–Œ¬¿À!"));
                    ui->doProgBar->setValue(100);
                }
                else
                {
                    ui->errorLabel->show();
                }
                QApplication::processEvents();
            }
            break;
        case SFT_KSUBOOT:
        case SFT_KI: break;
        }
    }
    else//STT_KSU
    {
    }
    ui->exitButton->setFocus();

//    ui->doLabel->setText(TU("œ–Œ√–¿ÃÃ»–”ﬁ..."));
//    ui->doProgBar->setValue(0);
//    ui->doLabel->show();
//    ui->doProgBar->show();
//    QApplication::processEvents();
//    for(int i=0; i<100; ++i)
//    {
//        ui->doProgBar->setValue(i);
//        for(int j=0; j<10; ++j)
//        {
//            QApplication::processEvents();
//            QThread::msleep(10);
//        }
//    }
//    ui->doProgBar->setValue(ui->doProgBar->maximum());
//    ui->errorLabel->show();
    //    ui->doLabel->setText(TU("» «ƒ≈—‹  ¿ Œ…-“Œ “≈ —“..."));
}

void QCtrlForm::deleteSlot()
{

}
