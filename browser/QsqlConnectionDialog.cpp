#include "QsqlConnectionDialog.h"

#include "ui_QsqlConnectionDialog.h"

#include <QSqlDatabase>

QSqlConnectionDialog::QSqlConnectionDialog(QWidget *parent)
    : QDialog(parent),
      m_statusBar(new QStatusBar(this))
{
    ui.setupUi(this);

    QStringList drivers = QSqlDatabase::drivers();

    // remove compat names
    drivers.removeAll("QMYSQL3");
    drivers.removeAll("QOCI8");
    //drivers.removeAll("QODBC3");
    drivers.removeAll("QPSQL7");
    drivers.removeAll("QTDS7");

    if (!drivers.contains("QSQLITE"))
        ui.dbCheckBox->setEnabled(false);

    ui.comboDriver->addItems(drivers);

    ui.toolbarLayout->addWidget(m_statusBar);
}

void QSqlConnectionDialog::setStatusBarMsg(const QString& msg)
{
    m_statusBar->showMessage(msg);
}

QSqlConnectionDialog::~QSqlConnectionDialog()
{
}

QString QSqlConnectionDialog::driverName() const
{
    return ui.comboDriver->currentText();
}

QString QSqlConnectionDialog::databaseName() const
{
    return ui.editDatabase->text();
}

QString QSqlConnectionDialog::userName() const
{
    return ui.editUsername->text();
}

QString QSqlConnectionDialog::password() const
{
    return ui.editPassword->text();
}

QString QSqlConnectionDialog::hostName() const
{
    return ui.editHostname->text();
}

int QSqlConnectionDialog::port() const
{
    return ui.portSpinBox->value();
}

void QSqlConnectionDialog::setDefaultParams(const QString& driver, const QString& dbs, const QString& user, const QString& password, const QString& host)
{
    ui.comboDriver->setCurrentText(driver);
    ui.editDatabase->setText(dbs);
    ui.editUsername->setText(user);
    ui.editPassword->setText(password);
    ui.editHostname->setText(host);

    ui.dbCheckBox->setVisible(false);
}

bool QSqlConnectionDialog::useInMemoryDatabase() const
{
    return ui.dbCheckBox->isChecked();
}

void QSqlConnectionDialog::on_okButton_clicked()
{
    if (ui.comboDriver->currentText().isEmpty()) {
        QMessageBox::information(this, tr("No database driver selected"),
                                 tr("Please select a database driver"));
        ui.comboDriver->setFocus();
    } else {
        accept();
    }
}
