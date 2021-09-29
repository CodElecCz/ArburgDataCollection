#ifndef MAIN_WINDOW_EXT_H
#define MAIN_WINDOW_EXT_H

#include <QObject>
#include <QWidget>
#include <QMainWindow>
#include <QDockWidget>
#include <QDateTime>

class MainWindowExt : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindowExt(QWidget *parent = nullptr);
    ~MainWindowExt();

    //docking suport
    void tabifyDockWidget(QDockWidget *dockwidget, const QString& title, const Qt::DockWidgetArea& area);

    //language support
    void retranslate();   

    //style
    void createStyleMenu(QMenu* styleMenu);

    //wait spinnbox
    static void waitShow(int timeoutMs = -1);
    static void waitHide(int delayMs = -1);

    static QDateTime buildDateTime();

    //restore size
    void initializeWindow();

public slots:
    //dialogs
    void aboutDialog();
    bool closeDialog();

protected slots:
    void styleChanged_triggered(QAction *action);

private:
    void readAppSettings();
    void writeAppSettings();

    void tabifyTopDockWidget(QDockWidget *dockwidget, const QString& title);
    void tabifyBottomDockWidget(QDockWidget *dockwidget, const QString& title);
    void tabifyLeftDockWidget(QDockWidget *dockwidget, const QString& title);
    void tabifyRightDockWidget(QDockWidget *dockwidget, const QString& title);    

    virtual void applicationQuit();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    QList<QDockWidget*> m_topDockTabbing;
    QList<QDockWidget*> m_bottomDockTabbing;
    QList<QDockWidget*> m_leftDockTabbing;
    QList<QDockWidget*> m_rightDockTabbing;

    QStringList         m_topDockTitles;
    QStringList         m_bottomDockTitles;
    QStringList         m_leftDockTitles;
    QStringList         m_rightDockTitles;

    QDateTime           m_appStartTime;
};

#endif // MAIN_WINDOW_EXT_H
