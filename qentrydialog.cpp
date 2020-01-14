#include "qentrydialog.h"
#include "ui_entry_dialog.h"

QEntryDialog::QEntryDialog(QWidget *parent):
    QDialog(parent),
    ui(new Ui::entryDialog)
{
    ui->setupUi(this);

    timer=new QTimer(this);
    Q_ASSERT(timer);
    connect(ui->entryButton, SIGNAL(pressed()), this, SLOT(accept()));
    connect(timer, SIGNAL(timeout()), this, SLOT(reject()));
}

QEntryDialog::~QEntryDialog()
{
    delete ui;
}

int QEntryDialog::exec()
{
    timer->start(2000);
    return QDialog::exec();
}


