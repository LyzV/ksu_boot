#include "qctrlform.h"
#include "ui_ctrl_form.h"
#include "bash_cmd.h"
#include <QThread>
#include <QFile>
#include <QMessageBox>
#include "treeitem.h"
#include "qintelhexparcer.h"

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
    kiProtocol=new KiProtocol(this);

    ui->doProgBar->setMinimum(0);
    ui->doProgBar->setMaximum(100);

    con=QObject::connect(this->ui->exitButton, SIGNAL(pressed()), this, SLOT(close()));
    Q_ASSERT(con);
    con=QObject::connect(this->ui->progButton, SIGNAL(pressed()), this, SLOT(progSlot()));
    Q_ASSERT(con);
    //con=QObject::connect(this->ui->delButton, SIGNAL(pressed()), this, SLOT(deleteSlot());
    //Q_ASSERT(con);
    con=QObject::connect(bootService, SIGNAL(programProgressSignal(int, int,QString)), this, SLOT(progressSlot(int, int,QString)));
    Q_ASSERT(con);
    con=QObject::connect(kiProtocol, SIGNAL(eraseSignal(int)), this, SLOT(eraseProgressSlot(int)));
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
        case SFT_KI:
            {
                int max=ui->doProgBar->maximum();
                QIntelHexParcer::ParseResult parseResult;
                QIntelHexParcer::FileRecord record;
                QIntelHexParcer parser;
                QString fullFileName=filePath+"/"+fileName;
                int lineCount;

                ui->errorLabel->hide();
                ui->progButton->setDisabled(true);
                ui->exitButton->setDisabled(true);
                progressSlot(max, 0, TU("Ïðîâåðÿþ ôàéë ïðîøèâêè..."));
                QApplication::processEvents();
                bool ret=parser.verifyHexFile(fullFileName, parseResult);
                if(false==ret)
                {
                    ui->errorLabel->setText(TU("ÎØÈÁÊÀ! ÍÅÊÎÐÐÅÊÒÍÛÉ ÔÀÉË ÏÐÎØÈÂÊÈ."));
                    ui->errorLabel->show();
                    break;
                }
                lineCount=parseResult.lineCount;

                progressSlot(5, 0, TU("Î÷èùàþ ïàìÿòü êîíòðîëëåðà..."));
                QApplication::processEvents();
                int err;
                if(false==kiProtocol->create(2, err))
                {
                    ui->errorLabel->setText(TU("ÎØÈÁÊÀ! ÍÅ ÌÎÃÓ ÎÒÊÐÛÒÜ CAN-ÈÍÒÅÐÔÅÉÑ."));
                    ui->errorLabel->show();
                    break;
                }
                if(false==kiProtocol->connect())
                {
                    ui->errorLabel->setText(TU("ÎØÈÁÊÀ! ÍÅ ÌÎÃÓ ÑÎÅÄÈÍÈÒÜÑß Ñ ÁËÎÊÎÌ ÊÈ."));
                    ui->errorLabel->show();
                    break;
                }
                if(false==kiProtocol->erase(0x3E8000))
                {
                    ui->errorLabel->setText(TU("ÎØÈÁÊÀ! ÍÅ ÌÎÃÓ Î×ÈÑÒÈÒÜ ÏÀÌßÒÜ ÁËÎÊÀ ÊÈ."));
                    ui->errorLabel->show();
                    break;
                }

                progressSlot(parseResult.lineCount, 0, TU("Ïðîãðàììèðóþ êîíòðîëëåð..."));
                if(false==parser.open(fullFileName))
                {
                    ui->errorLabel->setText(TU("ÎØÈÁÊÀ! ÍÅ ÌÎÃÓ ÎÒÊÐÛÒÜ ÔÀÉË ÏÐÎØÈÂÊÈ."));
                    ui->errorLabel->show();
                    break;
                }

                quint32 address;
                QByteArray data;
                int recordCount=0;//current record
                for(
                    ret=parser.firstRecord(record, parseResult);
                    true==ret;
                    ret=parser.nextRecord(record, parseResult)
                   )
                {
                    if(false==parser.toRawData(record, address, data))
                    {//end of file
                        ui->doProgBar->setValue(ui->doProgBar->maximum());
                        ui->doLabel->setText(TU("ÁËÎÊ ÊÈ ÓÑÏÅØÍÎ ÇÀÏÐÎÃÐÀÌÌÈÐÎÂÀÍ."));
                        break;
                    }
                    if(0!=data.count())
                    {
                        if(false==kiProtocol->program(address, data))
                        {
                            ui->errorLabel->setText(TU("ÎØÈÁÊÀ! ÍÅ ÑÌÎÃ ÇÀÏÐÎÃÐÀÌÌÈÐÎÂÀÒÜ."));
                            ui->errorLabel->show();
                            break;
                        }
                        progressSlot(lineCount, recordCount, TU("Ïðîãðàììèðóþ êîíòðîëëåð..."));
                    }
                    ++recordCount;
                }
                parser.close();
                kiProtocol->disconnect();
            }
            break;
        }
    }
    else//STT_KSU
    {
    }
    ui->progButton->setEnabled(true);
    ui->exitButton->setEnabled(true);
    QApplication::processEvents();
    ui->exitButton->setFocus();
    QApplication::processEvents();

}

void QCtrlForm::deleteSlot()
{

}

void QCtrlForm::progressSlot(int max, int progress, const QString &stringProgress)
{
    progress=progress % max;
    progress=(int)((float)progress/max*ui->doProgBar->maximum()+0.5f);
    ui->doProgBar->setValue(progress);
    ui->doLabel->setText(stringProgress);
    QApplication::processEvents();
}

void QCtrlForm::eraseProgressSlot(int seconds)
{
    progressSlot(4, seconds, TU("Î÷èùàþ ïàìÿòü êîíòðîëëåðà..."));
}
