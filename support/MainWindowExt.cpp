#include "support/MainWindowExt.h"
#include "support/style/CustomStyle.h"
#include "support/dialog/WaitDialog.h"

#include "ui_AboutDialog.h"

#include <QApplication>
#include <QAction>
#include <QMenu>
#include <QWidgetAction>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QFrame>
#include <QKeyEvent>
#include <QMessageBox>
#include <QSettings>
#include <QCoreApplication>

MainWindowExt::MainWindowExt(QWidget *parent) :
    QMainWindow(parent),
    m_topDockTabbing(),
    m_bottomDockTabbing(),
    m_leftDockTabbing(),
    m_rightDockTabbing(),
    m_topDockTitles(),
    m_bottomDockTitles(),
    m_leftDockTitles(),
    m_rightDockTitles(),
    m_appStartTime(QDateTime::currentDateTime())
{

}

MainWindowExt::~MainWindowExt()
{

}

void MainWindowExt::createStyleMenu(QMenu* styleMenu)
{
    QActionGroup* styleGroup = new QActionGroup(styleMenu);
    styleGroup->setExclusive(true);

    connect(styleGroup, SIGNAL (triggered(QAction *)), this, SLOT (styleChanged_triggered(QAction *)));

    //Light
    QAction *actionLight = new QAction("Light", styleMenu);
    actionLight->setCheckable(true);
    actionLight->setChecked(false);
    actionLight->setData(ECustomStyle_Light);
    styleMenu->addAction(actionLight);
    styleGroup->addAction(actionLight);

    //Dark
    QAction *actionDark = new QAction("Dark", styleMenu);
    actionDark->setCheckable(true);
    actionDark->setChecked(true);
    actionDark->setData(ECustomStyle_Dark);
    styleMenu->addAction(actionDark);
    styleGroup->addAction(actionDark);

    //apply default
    qApp->setStyle(new CustomStyle(ECustomStyle_Dark));
}

void MainWindowExt::retranslate()
{
    //dock widgets titles

    int index = 0;
    foreach(QDockWidget* w, m_topDockTabbing)
    {
        if(m_topDockTitles.size()>index)
            w->setWindowTitle(tr(m_topDockTitles.at(index++).toLatin1()));
    }

    index = 0;
    foreach(QDockWidget* w, m_bottomDockTabbing)
    {
        if(m_bottomDockTitles.size()>index)
            w->setWindowTitle(tr(m_bottomDockTitles.at(index++).toLatin1()));
    }

    index = 0;
    foreach(QDockWidget* w, m_leftDockTabbing)
    {
        if(m_leftDockTitles.size()>index)
            w->setWindowTitle(tr(m_leftDockTitles.at(index++).toLatin1()));
    }

    index = 0;
    foreach(QDockWidget* w, m_rightDockTabbing)
    {
        if(m_rightDockTitles.size()>index)
            w->setWindowTitle(tr(m_rightDockTitles.at(index++).toLatin1()));
    }

}

void MainWindowExt::tabifyDockWidget(QDockWidget *dockwidget, const QString& title, const Qt::DockWidgetArea& area)
{
    switch(area)
    {
    case Qt::BottomDockWidgetArea:
        tabifyBottomDockWidget(dockwidget, title);
        break;
    case Qt::TopDockWidgetArea:
        tabifyTopDockWidget(dockwidget, title);
        break;
    case Qt::LeftDockWidgetArea:
        tabifyLeftDockWidget(dockwidget, title);
        break;
    case Qt::RightDockWidgetArea:
        tabifyRightDockWidget(dockwidget, title);
        break;
    default:
        break;
    }
}

void MainWindowExt::tabifyTopDockWidget(QDockWidget *dockwidget, const QString &title)
{
    m_topDockTabbing.insert(0, dockwidget);
    m_topDockTitles.insert(0, title);

    addDockWidget(Qt::TopDockWidgetArea, dockwidget);

    int size = m_topDockTabbing.size();
    if(size>1)
    {
        QDockWidget* first = m_topDockTabbing.at(size-1);
        QDockWidget* second = m_topDockTabbing.at(0);

        QMainWindow::tabifyDockWidget(first, second);
    }
}

void MainWindowExt::tabifyBottomDockWidget(QDockWidget *dockwidget, const QString &title)
{
    m_bottomDockTabbing.insert(0, dockwidget);
    m_bottomDockTitles.insert(0, title);

    addDockWidget(Qt::BottomDockWidgetArea, dockwidget);

    int size = m_bottomDockTabbing.size();
    if(size>1)
    {
        QDockWidget* first = m_bottomDockTabbing.at(size-1);
        QDockWidget* second = m_bottomDockTabbing.at(0);

        QMainWindow::tabifyDockWidget(first, second);
    }
}

void MainWindowExt::tabifyLeftDockWidget(QDockWidget *dockwidget, const QString &title)
{
    m_leftDockTabbing.insert(0, dockwidget);
    m_leftDockTitles.insert(0, title);

    addDockWidget(Qt::LeftDockWidgetArea, dockwidget);

    int size = m_leftDockTabbing.size();
    if(size>1)
    {
        QDockWidget* first = m_leftDockTabbing.at(size-1);
        QDockWidget* second = m_leftDockTabbing.at(0);

        QMainWindow::tabifyDockWidget(first, second);
    }
}

void MainWindowExt::tabifyRightDockWidget(QDockWidget *dockwidget, const QString& title)
{
    m_rightDockTabbing.insert(0, dockwidget);
    m_rightDockTitles.insert(0, title);

    addDockWidget(Qt::RightDockWidgetArea, dockwidget);

    int size = m_rightDockTabbing.size();
    if(size>1)
    {
        QDockWidget* first = m_rightDockTabbing.at(size-1);
        QDockWidget* second = m_rightDockTabbing.at(0);

        QMainWindow::tabifyDockWidget(first, second);
    }
}


#include <QCoreApplication>
#include <QDateTime>

QDateTime MainWindowExt::buildDateTime()
{
    QDateTime dateTime;
    dateTime.setDate(QLocale("en_US").toDate(QString(__DATE__).simplified(), "MMM d yyyy"));
    dateTime.setTime(QTime::fromString(__TIME__, "hh:mm:ss"));
    return dateTime;
}

void MainWindowExt::aboutDialog()
{
    QDialog *aboutDialog = new QDialog(this, nullptr);
    Ui_AboutDialog aboutUi;
    aboutUi.setupUi(aboutDialog);
    aboutUi.labelAppNameText->setText(QCoreApplication::applicationName());
    aboutUi.labelAppVerText->setText(QCoreApplication::applicationVersion());
    aboutUi.labelBuildText->setText(buildDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    aboutUi.labelStartText->setText(m_appStartTime.toString("yyyy-MM-dd hh:mm:ss"));

    aboutDialog->show();
}

void MainWindowExt::styleChanged_triggered(QAction *action)
{
    if(action != nullptr)
    {
        ECustomStyle customStyle = static_cast<ECustomStyle>(action->data().toInt());

        switch(customStyle)
        {
        case ECustomStyle_Light:
            {
                CustomStyle *customStyle = new CustomStyle(ECustomStyle_Light);

                QPalette palette;
                customStyle->polish(palette);

                qApp->setStyle(customStyle);
                qApp->setPalette(palette);
            }
            break;

        case ECustomStyle_Dark:
            {
                CustomStyle *customStyle = new CustomStyle(ECustomStyle_Dark);

                QPalette palette;
                customStyle->polish(palette);

                qApp->setStyle(customStyle);
                qApp->setPalette(palette);
            }
            break;

        default:
            break;
        }
    }
}

void MainWindowExt::waitShow(int timeoutMs)
{
    WaitDialog::instance().show(timeoutMs);
}

void MainWindowExt::waitHide(int delayMs)
{
    WaitDialog::instance().close(delayMs);
}

void MainWindowExt::keyPressEvent(QKeyEvent *event)
{
    static bool maximized = false;

    if(event->key() == Qt::Key_F11)
    {
        if(this->isFullScreen())
        {
            if(maximized)
                this->showMaximized();
            else
                this->showNormal();
        }
        else
        {
            maximized = this->isMaximized();

            this->showFullScreen();
        }
    }
}

void MainWindowExt::closeEvent(QCloseEvent *event)
{
    bool close = closeDialog();
    if(!close)
        event->ignore();
}

bool MainWindowExt::closeDialog()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Exit", "Close application?", QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
        //write settings
        writeAppSettings();

        hide();

        applicationQuit();
    }

    return (reply == QMessageBox::Yes);
}

void MainWindowExt::applicationQuit()
{
    QApplication::quit();
}

void MainWindowExt::readAppSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

    settings.beginGroup("MainWindow");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
    settings.endGroup();
}

void MainWindowExt::writeAppSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

    settings.beginGroup("MainWindow");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    settings.endGroup();
}

void MainWindowExt::initializeWindow()
{
    readAppSettings();
}
