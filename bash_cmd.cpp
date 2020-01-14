#include "bash_cmd.h"

#include <QProcess>
#include <QThread>
#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
//#include <QHostAddress>


void QBashCmd::stop_getty(void)
{
    QProcess process;
    QString program("systemctl");
    QStringList arguments;
    arguments << "stop" << "getty@tty1.service";
    process.start(program, arguments);
    process.waitForFinished();
    arguments.clear();
    arguments << "disable" << "getty@tty1.service";
    process.start(program, arguments);
    process.waitForFinished();
}

bool QBashCmd::flush_storage(void)
{
    QProcess process;
    QString program("sync");
    process.start(program);
    process.waitForFinished();
    QFile file;
    file.setFileName("/proc/sys/vm/drop_caches");
    if(false==file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;
    QTextStream stream(&file);
    stream << "3\n";
    file.flush();
    file.close();
    return true;
}

bool QBashCmd::make_usb_auto_mount(void)
{
    //0) Check file 10-usb.rules in folder /etc/udev/rules.d
    QString file_name="/etc/udev/rules.d/10-usb.rules";
    if(true==QFile::exists(file_name)) return true;

    //1) Make file
    QString text="";
    text+="SUBSYSTEM==\"block\", KERNEL==\"sd[a-z][0-9]\", ACTION==\"add\", RUN+=\"/bin/mkdir -p /media/usb-%k\"\n";
    text+="SUBSYSTEM==\"block\", KERNEL==\"sd[a-z][0-9]\", ACTION==\"add\", RUN+=\"/bin/mount -t vfat -o auto,sync /dev/%k /media/usb-%k\"\n";
    text+="SUBSYSTEM==\"block\", KERNEL==\"sd[a-z][0-9]\", ACTION==\"remove\", RUN+=\"/bin/umount /media/usb-%k/\"\n";
    text+="SUBSYSTEM==\"block\", KERNEL==\"sd[a-z][0-9]\", ACTION==\"remove\", RUN+=\"/bin/rm -r /media/usb-%k/\"\n";
    QFile file;
    file.setFileName(file_name);
    if(false==file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;
    QTextStream stream(&file);
    stream << text;
    file.flush();
    file.close();

    //2) Reread rules: udevadm control --reload-rules
    QProcess process;
    QString program("udevadm");
    QStringList arguments;
    arguments << "control" << "--reload-rules";
    process.start(program, arguments);
    process.waitForFinished();

    return true;
}

bool QBashCmd::rmdir(const QString &dirName)
{
    QProcess process;
    QStringList arguments;
    QProcess::ExitStatus exitStatus;
    arguments << "-rf" << dirName;
    process.start("rm", arguments);
    if(false==process.waitForFinished())
        return false;
    exitStatus=process.exitStatus();
    if(QProcess::NormalExit!=exitStatus)
        return false;

    return true;
}

//void QBashCmd::set_ipv4_address(uint32_t addr)
//{//ifconfig eth0 192.168.3.11
//    QProcess process;
//    QString program("ifconfig");
//    QStringList arguments;
//    QHostAddress haddr(addr);
//    arguments << "eth0" << haddr.toString();
//    process.start(program, arguments);
//    process.waitForFinished();
//}

//void QBashCmd::set_ipv4_mask(uint32_t mask)
//{//ifconfig eth0 netmask 255.255.255.240
//    QProcess process;
//    QString program("ifconfig");
//    QStringList arguments;
//    QHostAddress haddr(mask);
//    arguments << "eth0" << "netmask" << haddr.toString();
//    process.start(program, arguments);
//    process.waitForFinished();
//}

//void QBashCmd::set_ipv4_gateway(uint32_t addr)
//{// route add default gw 192.168.3.10 eth0
//    QProcess process;
//    QString program("route");
//    QStringList arguments;
//    QHostAddress haddr(addr);
//    arguments << "add" << "default" << "gw" << haddr.toString() << "eth0";
//    process.start(program, arguments);
//    process.waitForFinished();
//}

void QBashCmd::set_file_execute_mode(const QString &file)
{
    QProcess process;
    QString program("chmod");
    QStringList arguments;
    arguments << "+x" << file;
    process.start(program, arguments);
    process.waitForFinished();
}

bool QBashCmd::goto_main_soft(const QString &workDirectory)
{
    QFileInfo mainSoftFile(workDirectory + "/ksu");
    if(false==mainSoftFile.exists())
        return false;
    if(false==mainSoftFile.isExecutable())
        return false;
    QProcess process;
    QStringList arguments;
    QString program(mainSoftFile.absoluteFilePath());
    arguments.clear();
    return process.startDetached(program, arguments, workDirectory);
}
