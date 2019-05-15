#ifndef EXTRAWIDGETS_H
#define EXTRAWIDGETS_H

#include <QWidget>

namespace Ui {
class Widget;
}

class ExtraWidgets : public QWidget
{
    Q_OBJECT

public:
    explicit ExtraWidgets(QWidget *parent = nullptr);
    ~ExtraWidgets();

private:
    Ui::Widget *ui;
};

#endif // EXTRAWIDGETS_H
