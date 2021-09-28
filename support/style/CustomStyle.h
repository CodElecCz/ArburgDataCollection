#ifndef CUSTOMSTYLE_H
#define CUSTOMSTYLE_H

#include <QApplication>
#include <QFile>
#include <QFont>
#include <QProxyStyle>
#include <QStyleFactory>

typedef enum _ECustomStyle
{
    ECustomStyle_Default = 0,   //WindowsVista    
    ECustomStyle_Light,         //Fusion
    ECustomStyle_Dark           //Fusion

} ECustomStyle;


class CustomStyle : public QProxyStyle
{
    Q_OBJECT

public:
    CustomStyle(ECustomStyle type = ECustomStyle_Default);
    explicit CustomStyle(QStyle *style);

    QStyle *baseStyle() const;

    void polish(QPalette &palette) override;
    void polish(QApplication *app) override;

private:
    QStyle*  styleBase(ECustomStyle type = ECustomStyle_Default, QStyle *style = nullptr) const;

private:
    ECustomStyle  m_type;
};

#endif  // CUSTOMSTYLE_H
