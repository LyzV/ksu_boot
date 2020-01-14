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

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setApplicationName("ksu_boot");
    QCoreApplication::setApplicationVersion("1.0.2");

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
        bootPathList << currentWorkDirectoryName + "/ksu1";
        bootPathList << currentWorkDirectoryName + "/ksu2";
        if(true==QBootstrap::bootstrap(bootPathList, currentWorkDirectoryName))
        {
            a.exit(0);
            return 0;
        }
    }

    //run boot loader
    int err;
    if(false==model.Create(err))
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



