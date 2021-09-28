#include "CustomStyle.h"

#include <QDesktopWidget>
#include <QDebug>

namespace
{
    const int cDefaultDPI_85p = 72;
    const int cDefaultDPI_100p = 96;
    const int cDefaultDPI_125p = 120;
    const int cDefaultDPI_150p = 144;
    const int cDefaultDPI_175p = 168;
    const int cDefaultDPI_200p = 192;
}

CustomStyle::CustomStyle(ECustomStyle type) :
    CustomStyle(styleBase(type))
{
    m_type = type;
}

CustomStyle::CustomStyle(QStyle *style) :
    QProxyStyle(style)
{

}

QStyle *CustomStyle::styleBase(ECustomStyle type, QStyle *style) const
{    
    /* static */ QStyle *base = nullptr;
    if(!style)
    {
        switch(type)
        {
        case ECustomStyle_Light:
        case ECustomStyle_Dark:
            base = QStyleFactory::create(QStringLiteral("Fusion"));
            break;
        default:
            base = QStyleFactory::create(QStringLiteral("WindowsVista"));
            break;
        }
    }
    else
        base = style;

    return base;
}

QStyle *CustomStyle::baseStyle() const
{
    return styleBase();
}

void CustomStyle::polish(QPalette &palette)
{
    switch(m_type)
    {
    case ECustomStyle_Dark:
    {
        palette.setColor(QPalette::Window, QColor(53, 53, 53));
        palette.setColor(QPalette::WindowText, Qt::white);
        palette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(127, 127, 127));
        palette.setColor(QPalette::Base, QColor(42, 42, 42));
        palette.setColor(QPalette::AlternateBase, QColor(66, 66, 66));
        palette.setColor(QPalette::ToolTipBase, Qt::white);
        palette.setColor(QPalette::ToolTipText, QColor(53, 53, 53));
        palette.setColor(QPalette::Text, Qt::white);
        palette.setColor(QPalette::Disabled, QPalette::Text, QColor(127, 127, 127));
        palette.setColor(QPalette::Dark, QColor(35, 35, 35));
        palette.setColor(QPalette::Shadow, QColor(20, 20, 20));
        palette.setColor(QPalette::Button, QColor(53, 53, 53));
        palette.setColor(QPalette::ButtonText, Qt::white);
        palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(127, 127, 127));
        palette.setColor(QPalette::BrightText, Qt::red);
        palette.setColor(QPalette::Link, QColor(42, 130, 218));
        palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
        palette.setColor(QPalette::Disabled, QPalette::Highlight, QColor(80, 80, 80));
        palette.setColor(QPalette::HighlightedText, Qt::white);
        palette.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(127, 127, 127));
    }
        break;
    case ECustomStyle_Light:
        palette = QApplication::style()->standardPalette();
        break;
    default:
        break;
    }
}

void CustomStyle::polish(QApplication *app)
{
    if (!app)
        return;

    //Setting Qt To Ignore Windows DPI Text Size Personalization
    QFont defaultFont = QApplication::font();

    int dpi = qApp->desktop()->logicalDpiX();   

    if(dpi<=cDefaultDPI_100p)
    {
        defaultFont.setPixelSize(11); //11
    }
    else if(dpi<=cDefaultDPI_125p)
    {
        defaultFont.setPixelSize(15);
    }
    else if(dpi<=cDefaultDPI_150p)
    {
        defaultFont.setPixelSize(17);
    }
    else if(dpi<=cDefaultDPI_175p)
    {
        defaultFont.setPixelSize(19);
    }
    else //200p...250p
    {
        defaultFont.setPixelSize(21);
    }

    app->setFont(defaultFont);

    // loadstylesheet
    switch(m_type)
    {
    case ECustomStyle_Light:
        {
            qInfo() << "Application Light style, DPI: " << dpi;

            QString qsStylesheet = "";

            //Qt components style
            QFile fstyle(QStringLiteral(":/light/style.qss"));
            if (fstyle.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                // set stylesheet
                qsStylesheet.append(QString::fromLatin1(fstyle.readAll()));
                fstyle.close();
            }
            //project custom style
            QFile fstyleCustom(QStringLiteral(":/light/custom.qss"));
            if (fstyleCustom.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                // set stylesheet
                qsStylesheet.append(QString::fromLatin1(fstyleCustom.readAll()));
                fstyleCustom.close();
            }

            app->setStyleSheet(qsStylesheet);
            qApp->setPalette( app->style()->standardPalette());
        }
        break;
    case ECustomStyle_Dark:
        {
            qInfo() << "Application Dark style, DPI: " << dpi;

            QString qsStylesheet = "";

            //Qt components style
            QFile fstyle(QStringLiteral(":/dark/style.qss"));
            if (fstyle.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                // set stylesheet
                qsStylesheet.append(QString::fromLatin1(fstyle.readAll()));
                fstyle.close();
            }
            //project custom style
            QFile fstyleCustom(QStringLiteral(":/dark/custom.qss"));
            if (fstyleCustom.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                // set stylesheet
                qsStylesheet.append(QString::fromLatin1(fstyleCustom.readAll()));
                fstyleCustom.close();
            }

            app->setStyleSheet(qsStylesheet);
            qApp->setPalette( app->style()->standardPalette());
        }
        break;
    default:
        break;
    }

}
