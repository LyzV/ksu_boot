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
#include "gpio.h"
#include "maindialog.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("RIMERA");
    QCoreApplication::setApplicationName("ksu_boot");
    QCoreApplication::setApplicationVersion("1.0.6");

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

    //watch dog
    gpio_open();
    gpio_wdt_reset();

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

    MainDialog mainDialog(&model, currentWorkDirectoryName);
    mainDialog.show();
    return a.exec();
}


