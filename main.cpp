#include "treemodel.h"
#include "qksutreeview.h"
#include "qentrydialog.h"
#include <QObject>
#include <QApplication>
#include <QCommandLineParser>
#include <QStringList>
#include <QProcess>
#include <QDialog>
#include <QTreeView>
#include <QFile>
#include "bash_cmd.h"
#include "qbootstrap.h"
#include <QSettings>


static void testParser();

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("RIMERA");
    QCoreApplication::setApplicationName("ksu_boot");
    QCoreApplication::setApplicationVersion("1.0.1");

testParser();

    QCommandLineParser parser;
    parser.setApplicationDescription("Second time boot KSU");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("directory", "Current Work Directory");

    QCommandLineOption currentWorkDirectoryOption(QStringList() << "d" << "work_directory",
                                                  "Current work directory",
                                                  "work_directory",
                                                  "/home/root");
    parser.addOption(currentWorkDirectoryOption);
    parser.process(a);
    QString currentWorkDirectoryName=parser.value(currentWorkDirectoryOption);

    TreeModel model(currentWorkDirectoryName);
    QEntryDialog entryDialog;

    QBashCmd::stop_getty();
    //QBashCmd::make_usb_auto_mount();

    int ret=entryDialog.exec();
    if(QDialog::Rejected==ret)
    {//go to main soft
        QStringList bootPathList;
        QBootstrap bootService;
        bootPathList << currentWorkDirectoryName + "/ksu1";
        bootPathList << currentWorkDirectoryName + "/ksu2";
        bootService.create(bootPathList, currentWorkDirectoryName);
        if(true==bootService.bootstrap())
        {
            a.exit(0);
            return 0;
        }
    }

    QSettings distInfo("/home/root/linux-distributive-version.txt", QSettings::IniFormat);
    QString bspVersion=distInfo.value("BSP/version", QVariant("")).toString();
    bspVersion=bspVersion;
    QVariant majorDistVariant=distInfo.value("DIST/major", QVariant(-1));
    bool ok;
    int majorDist=majorDistVariant.toInt(&ok);
    QString distFilter;
    if((false==ok)||(0>majorDist))  distFilter="*";
    else                            distFilter=QString::number(majorDist);

    //run boot loader
    int err;
    if(false==model.Create(distFilter, err))
    {
        a.exit(1);
        return 1;
    }
    QKsuTreeView view(currentWorkDirectoryName);
    view.setModel(&model);
    view.setAutoScroll(true);
    view.setColumnWidth(0, 420);
    view.setFont(QFont("", 15));
    view.setItemsExpandable(true);
    view.setWindowTitle(QObject::tr("Simple Tree Model"));
    view.setGeometry(QRect(0, 0, 640, 480));
    view.show();
    return a.exec();
}


#include "qintelhexparcer.h"
#include "kiprotocol.h"
static void testParser()
{
    KiProtocol protocol;
    QIntelHexParcer::ParseResult parseResult;
    QIntelHexParcer::FileRecord record;
    QIntelHexParcer parser;

    bool ret=parser.verifyHexFile("/home/root/4B-03-02.KI", parseResult);
    if(false==ret)
        return;

    int err;
    if(false==protocol.create(2, err))
        return;

    if(false==protocol.connect())
        return;

    uint16_t ver, type;
    if(false==protocol.hver(ver, type))
    {
        protocol.disconnect();
        return;
    }
    if(false==protocol.erase(0x3E8000, nullptr))
        return;

    if(false==parser.open("/home/root/4B-03-02.KI"))
        return;
    uint32_t exAddress=0x3E0000;
    int count=0;
    for(
        ret=parser.firstRecord(record, parseResult);
        true==ret;
        ret=parser.nextRecord(record, parseResult)
       )
    {
        if(1==record.recordType)
        {//end of file
            break;
        }
        else if(4==record.recordType)
        {//extended address
            //exAddress=(uint32_t)record.data[0]<<16;
        }
        else if(0==record.recordType)
        {//data
            if(false==protocol.program(exAddress+record.address, record.data))
                break;
        }
        else
        {
            break;
        }
        ++count;
    }
    parser.close();
    protocol.disconnect();
}



