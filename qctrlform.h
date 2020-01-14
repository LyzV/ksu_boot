#ifndef QCTRLFORM_H
#define QCTRLFORM_H

#include <QDialog>
#include <QTextCodec>
#include <QString>

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

    bool copySoft(const QString &from, const QString &to);


private slots:
    void progSlot(void);
    void deleteSlot(void);
};

#endif // QCTRLFORM_H
