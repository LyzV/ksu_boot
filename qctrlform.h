#ifndef QCTRLFORM_H
#define QCTRLFORM_H

#include <QDialog>
#include <QTextCodec>
#include <QString>
#include "qbootstrap.h"
#include "kiprotocol.h"

namespace Ui { class CtrlForm; }

class QCtrlForm: protected QDialog
{
    Q_OBJECT
public:
    explicit QCtrlForm(const QString &workDirectory, QWidget *parent=nullptr);
    ~QCtrlForm();

    int Exec(int &storageType, int &softType, QString &filePath, QString &fileName);

private:
    Ui::CtrlForm *ui;
    QTextCodec *codec;
    int storageType;
    int softType;
    QString fileName;
    QString filePath;
    QString workDirectory;
    QBootstrap *bootService=nullptr;
    KiProtocol *kiProtocol=nullptr;

    bool copySoft(const QString &from, const QString &to);


private slots:
    void progSlot(void);
    void deleteSlot(void);
    void progressSlot(int max, int progress, const QString &stringProgress);
    void eraseProgressSlot(int seconds);
};

#endif // QCTRLFORM_H
