#include "qctrlform.h"
#include "ui_ctrl_form.h"
#include "bash_cmd.h"
#include <QThread>
#include <QFile>
#include <QMessageBox>
#include "treeitem.h"

#define TU(s) codec->toUnicode((s))

QCtrlForm::QCtrlForm(const QString &workDirectory, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CtrlForm)
{
    bool con;
    this->workDirectory=workDirectory;

    codec=QTextCodec::codecForName("CP1251");

    ui->setupUi(this);
    ui->errorLabel->hide();
    ui->doLabel->hide();
    ui->doProgBar->hide();

    ui->copyButton->setDisabled(true);
    ui->delButton->setDisabled(true);

    bootService=new QBootstrap(this);

    ui->doProgBar->setMinimum(0);
    ui->doProgBar->setMaximum(100);

    con=QObject::connect(this->ui->exitButton, SIGNAL(pressed()), this, SLOT(close()));
    Q_ASSERT(con);
    con=QObject::connect(this->ui->progButton, SIGNAL(pressed()), this, SLOT(progSlot()));
    Q_ASSERT(con);
    //con=QObject::connect(this->ui->delButton, SIGNAL(pressed()), this, SLOT(deleteSlot());
    //Q_ASSERT(con);
    con=QObject::connect(bootService, SIGNAL(programProgressSignal(int,QString)), this, SLOT(progressSlot(int,QString)));
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


    ui->fileLabel->setText(TU("ÂÛÁÐÀÍ ÔÀÉË: ")+this->fileName);
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
    ui->doLabel->setText(TU(""));
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
                QStringList bootPathList;
                bootPathList << workDirectory + "/ksu1" << workDirectory + "/ksu2";
                bootService->create(bootPathList, "/home/root");
                QString errorString;
                if(true==bootService->programSoft(filePath+"/"+fileName, errorString))
                {
                    ui->doLabel->setText(TU("ÓÑÏÅØÍÎ ÇÀÏÐÎÃÐÀÌÌÈÐÎÂÀË!"));
                    ui->doProgBar->setValue(100);
                }
                else
                {
                    ui->errorLabel->setText(errorString);
                    ui->errorLabel->show();
                }
                QApplication::processEvents();
            }
            break;
        case SFT_KSUBOOT:
        {
            QStringList bootPathList;
            bootPathList << workDirectory + "/ksu_boot1" << workDirectory + "/ksu_boot2";
            bootService->create(bootPathList, "/home/root");
            QString errorString;
            if(true==bootService->programSoft(filePath+"/"+fileName, errorString))
            {
                ui->doLabel->setText(TU("ÓÑÏÅØÍÎ ÇÀÏÐÎÃÐÀÌÌÈÐÎÂÀË!"));
                ui->doProgBar->setValue(100);
            }
            else
            {
                ui->errorLabel->setText(errorString);
                ui->errorLabel->show();
            }
            QApplication::processEvents();
        }
        break;
        case SFT_KI: break;
        }
    }
    else//STT_KSU
    {
    }
    ui->exitButton->setFocus();
}

void QCtrlForm::deleteSlot()
{

}

void QCtrlForm::progressSlot(int progress, const QString &stringProgress)
{
    ui->doProgBar->setValue(progress);
    ui->doLabel->setText(stringProgress);
    QApplication::processEvents();
}
