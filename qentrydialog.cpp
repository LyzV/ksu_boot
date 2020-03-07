#include "qentrydialog.h"
#include "ui_entry_dialog.h"

QEntryDialog::QEntryDialog(QWidget *parent):
    QDialog(parent),
    ui(new Ui::entryDialog)
{
    ui->setupUi(this);

    timer=new QTimer(this);
    clearTimer=new QTimer(this);
    Q_ASSERT(timer);
    connect(ui->entryButton, SIGNAL(pressed()), this, SLOT(accept()));
    connect(timer, SIGNAL(timeout()), this, SLOT(reject()));
    connect(clearTimer, SIGNAL(timeout()), this, SLOT(clearSlot()));
}

QEntryDialog::~QEntryDialog()
{
    delete ui;
}

int QEntryDialog::exec()
{

    clearTimer->start(4000);
    timer->start(4100);
    return QDialog::exec();
}

void QEntryDialog::clearSlot()
{
    ui->entryButton->setText("");
}


