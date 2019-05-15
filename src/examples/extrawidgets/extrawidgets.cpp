#include "extrawidgets.h"
#include "ui_extrawidgets.h"

#include <twodrwsurface.h>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QGridLayout>

ExtraWidgets::ExtraWidgets(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    QGridLayout *lo = new QGridLayout(this);
    TwoDRWSurface *rws = new TwoDRWSurface(this);
    lo->addWidget(rws, 0, 0, 5, 5);

    int row = 6, col = 1;
    QLabel *la = new QLabel("bounds", this);
    lo->addWidget(la, row, 0, 1, 1);
    foreach(QString s, QStringList() << "xm" << "xM" << "ym" << "yM") {
        QDoubleSpinBox *sb = new QDoubleSpinBox(this);
        sb->setObjectName("sb" + s);
        sb->setMinimum(-1000000);
        sb->setMaximum(1000000);
        lo->addWidget(sb, row, col++, 1 , 1);
        s.contains("M") ? sb->setValue(1000) : sb->setValue(0.0);
    }
    connect(findChild<QDoubleSpinBox *>("sbxm"), SIGNAL(valueChanged(double)), rws, SLOT(setXMin(double)));
    connect(findChild<QDoubleSpinBox *>("sbxM"), SIGNAL(valueChanged(double)), rws, SLOT(setXMax(double)));
    connect(findChild<QDoubleSpinBox *>("sbym"), SIGNAL(valueChanged(double)), rws, SLOT(setYMin(double)));
    connect(findChild<QDoubleSpinBox *>("sbyM"), SIGNAL(valueChanged(double)), rws, SLOT(setYMax(double)));

    row++;

    QDoubleSpinBox *xsb = new QDoubleSpinBox(this);
    QDoubleSpinBox *ysb = new QDoubleSpinBox(this);
    QDoubleSpinBox *xwsb = new QDoubleSpinBox(this);
    QDoubleSpinBox *ywsb = new QDoubleSpinBox(this);

    // appropriate boundaries on spin boxes
    foreach(QDoubleSpinBox *sb, findChildren<QDoubleSpinBox *>()) {
        sb->setMinimum(-1000000);
        sb->setMaximum(1000000);
    }

    QLabel *xlab = new QLabel("X value");
    lo->addWidget(xlab, row, 0, 1, 1);
    xsb->setObjectName("sbx");
    xsb->setValue(rws->x());
    lo->addWidget(xsb, row, 1, 1, 1);

    QLabel *ylab = new QLabel("Y value");
    lo->addWidget(ylab, row, 2, 1, 1);
    ysb->setObjectName("sby");
    ysb->setValue(rws->y());
    lo->addWidget(ysb, row, 3, 1, 1);

    connect(xsb, SIGNAL(valueChanged(double)), rws, SLOT(animateToX(double)));
    connect(ysb, SIGNAL(valueChanged(double)), rws, SLOT(animateToY(double)));

    row++;

    QLabel *xwlab = new QLabel("X value (W)");
    lo->addWidget(xwlab, row, 0, 1, 1);
    xwsb->setObjectName("sbxw");
    xwsb->setValue(rws->x_w());
    lo->addWidget(xwsb, row, 1, 1, 1);

    QLabel *ywlab = new QLabel("Y value (W)");
    lo->addWidget(ywlab, row, 2, 1, 1);
    ywsb->setObjectName("sbyw");
    ywsb->setValue(rws->y_w());
    lo->addWidget(ywsb, row, 3, 1, 1);

    connect(xwsb, SIGNAL(valueChanged(double)), rws, SLOT(setX_W(double)));
    connect(ywsb, SIGNAL(valueChanged(double)), rws, SLOT(setY_W(double)));

    foreach(QLabel *l, findChildren<QLabel *>())
        l->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
}

ExtraWidgets::~ExtraWidgets()
{
    delete ui;
}
