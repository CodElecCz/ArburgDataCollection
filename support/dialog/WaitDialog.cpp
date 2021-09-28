#include "support/dialog/WaitDialog.h"
#include "support/controls/WaitingSpinner.h"

#include <QTimer>
#include <QVBoxLayout>
#include <QApplication>

namespace
{    
    const int cDelayedShowMs = 100;
}

WaitDialog::WaitDialog(QWidget *parent) :
    QDialog(parent)
{
    this->setModal(true);

    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    m_waitingSpinner.setRoundness(70.0);
    m_waitingSpinner.setMinimumTrailOpacity(15.0);
    m_waitingSpinner.setTrailFadePercentage(70.0);
    m_waitingSpinner.setNumberOfLines(12);
    m_waitingSpinner.setLineLength(15);
    m_waitingSpinner.setLineWidth(5);
    m_waitingSpinner.setInnerRadius(10);
    m_waitingSpinner.setRevolutionsPerSecond(1);
    m_waitingSpinner.setColor(Qt::darkGray);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(&m_waitingSpinner);
    setLayout(mainLayout);

    m_timeout.setSingleShot(true);
    connect(&m_timeout, SIGNAL(timeout()), this, SLOT(close()));

    m_delayedShow.setSingleShot(true);
    connect(&m_delayedShow, SIGNAL(timeout()), this, SLOT(delayedShow()));

    m_delayedClose.setSingleShot(true);
    connect(&m_delayedClose, SIGNAL(timeout()), this, SLOT(delayedClose()));
}

WaitDialog::~WaitDialog()
{

}

WaitDialog& WaitDialog::instance()
{
    static WaitDialog instance;

    return instance;
}

void WaitDialog::show(int timeoutMs)
{
    if(cDelayedShowMs>0)
        m_delayedShow.start(cDelayedShowMs);
    else
        delayedShow();

    if(timeoutMs>0)
        m_timeout.start(timeoutMs);
}

void WaitDialog::close(int delayMs)
{    
    m_timeout.stop();

    if(delayMs>0)       
        m_delayedClose.start(delayMs);
    else               
        delayedClose();
}

void WaitDialog::delayedShow()
{
    m_waitingSpinner.start();

    QDialog::show();
}

void WaitDialog::delayedClose()
{
    m_waitingSpinner.stop();
    m_delayedShow.stop();

    QDialog::close();
}
