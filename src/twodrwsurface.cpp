#include "twodrwsurface.h"
#include <QPainter>
#include <QPaintEvent>
#include <QtDebug>
#include <cumacros.h>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QTimer>

class TwoDRWWriterData {
public:
    QRectF rect;
    QPointF press_pos, release_pos, pos, dist;
    QTimer *signal_timer;
};

class TwoDRWSurfacePrivate {
public:
    bool read_only, animation_enabled;
    double xm, xM, ym, yM, x, y, xw, yw;
    int x_scale_h, y_scale_w, xmargin, ymargin;
    int read_indicator_size, write_indicator_size;
    int animation_duration;
    QString format;
    QRectF surface;
    TwoDRWWriterData w_data;
};

TwoDRWSurface::TwoDRWSurface(QWidget *parent) : QFrame (parent)
{
    d = new TwoDRWSurfacePrivate;
    d->read_only = false;
    d->animation_enabled = true;
    d->xM = d->yM = 1000;
    d->xm = d->ym = 0;
    d->x = 500;
    d->y = 500;
    d->xw = 0;
    d->yw = 0;
    d->x_scale_h = d->y_scale_w = -1;
    d->format = "%g";
    d->xmargin = d->ymargin = 4;
    d->surface = QRectF();
    d->read_indicator_size = 20;
    d->write_indicator_size = 24;
    d->animation_duration = 500;
    d->w_data.signal_timer = new QTimer(this);
    d->w_data.signal_timer->setInterval(800);
    d->w_data.signal_timer->setSingleShot(true);
    connect(d->w_data.signal_timer, SIGNAL(timeout()), this, SLOT(m_onTargetSignalTimeout()));

    setMouseTracking(true);
    QPropertyAnimation *xAnim = new QPropertyAnimation(this, "x", this);
    QPropertyAnimation *yAnim = new QPropertyAnimation(this, "y", this);
    xAnim->setObjectName("xAnim");
    yAnim->setObjectName("yAnim");
    setAnimationDuration(d->animation_duration);
}

void TwoDRWSurface::drawRead(QPainter *pa, QPaintEvent *p)
{
    QPen pe = pa->pen();
    pe.setColor(QColor(Qt::green));
    pa->setPen(pe);
    const double w = d->surface.width(), h = d->surface.height(), x0 = d->surface.x(), y0 = d->surface.y();
    const QPointF xyp = mapFromValue(QPointF(d->x, d->y));
    const double &xp = xyp.x(), &yp = xyp.y();

    if(xp >= 0 && yp >= 0) {
        pa->drawLine(xp, yp - d->read_indicator_size/2, xp,  yp + d->read_indicator_size/2);
        pa->drawLine(xp - d->read_indicator_size/2, yp, xp + d->read_indicator_size/2, yp);
    }
}

void TwoDRWSurface::drawWrite(QPainter *pa, QPaintEvent *p)
{
    const QPointF pcenter = mapFromValue(QPointF(d->xw, d->yw));
    const double& xc = pcenter.x();
    const double& yc = pcenter.y();
    if(xc >= 0 && yc >= 0) {
        d->w_data.rect = QRectF(xc - d->write_indicator_size/2, yc - d->write_indicator_size/2,
                                d->write_indicator_size, d->write_indicator_size);
        QPen pe = pa->pen();
        QColor circleCol(Qt::lightGray);
        circleCol.setAlpha(120);
        pa->setBrush(circleCol);
        pa->drawEllipse(d->w_data.rect);
        pe.setColor(QColor(Qt::red));
        pa->setPen(pe);
        pa->drawLine(xc, yc - d->write_indicator_size/2, xc,  yc + d->write_indicator_size/2);
        pa->drawLine(xc - d->write_indicator_size/2, yc, xc + d->write_indicator_size/2, yc);
    }
}

void TwoDRWSurface::drawBackground(QPainter *p, QPaintEvent *pe)
{
    p->fillRect(d->surface, QColor(Qt::white));
    QColor c(Qt::darkYellow);
    QPen pen(c);
    p->setPen(pen);
    p->drawRect(d->surface);
}

void TwoDRWSurface::drawScale(QPainter *pa, QPaintEvent *p, TwoDRWSurface::Scale s)
{
    const QFont& f = pa->font();
    QFontMetrics fm(f);
    int len;
    if(s == XScale) {
        pa->drawText(d->surface.left(), d->surface.bottom()  + fm.height()/2, to_string(d->xm));
        pa->drawText(d->surface.right() - fm.width(to_string(d->xM)),  d->surface.bottom() + fm.height()/2, to_string(d->xM));
    }
    else { // Y scale
        QString y = to_string(d->ym);
        len = p->rect().height();
        // Y min
        pa->drawText(d->surface.left() - fm.width(y), d->surface.bottom(), y);
        y = to_string(d->yM);
        pa->drawText(d->surface.left() - fm.width(y), d->surface.top() + fm.height()/2, y);
    }
}

QPointF TwoDRWSurface::mapFromPos(const QPointF &pos) const
{
    const double w = d->surface.width(), h = d->surface.height(), x0 = d->surface.left(), y0 = d->surface.top();
    double px = (pos.x() - x0) * (d->xM - d->xm) / w;
    double py = y0 + (h - pos.y() + y0) * (d->yM - d->ym) / h;
    return QPointF(d->xm + px, d->ym + py);
}

QPointF TwoDRWSurface::mapFromValue(const QPointF &xy) const
{
    const double w = d->surface.width(), h = d->surface.height(), x0 = d->surface.x(), y0 = d->surface.y();
    double xp = -1, yp = -1;
    if(xy.x() >= d->xm && xy.x() <= d->xM && d->xm <= d->xM) {
        xp = x0 + (xy.x() - d->xm) * w  / (d->xM - d->xm);
    }
    if(xy.y() >= d->ym && xy.y() <= d->yM && d->ym <= d->yM) {
        yp = h + y0 - (xy.y() - d->ym) * h  / (d->yM - d->ym);
    }
    return QPointF(xp, yp);
}

double TwoDRWSurface::xMin() const
{
    return d->xm;
}

double TwoDRWSurface::xMax() const
{
    return d->xM;
}

double TwoDRWSurface::yMin() const
{
    return  d->ym;
}

double TwoDRWSurface::yMax() const
{
    return  d->yM;
}

bool TwoDRWSurface::readOnly() const
{
    return d->read_only;
}

double TwoDRWSurface::x() const
{
    return  d->x;
}

double TwoDRWSurface::y() const
{
    return  d->y;
}

double TwoDRWSurface::x_w() const
{
    return  d->xw;
}

double TwoDRWSurface::y_w() const
{
    return  d->yw;
}

QString TwoDRWSurface::format() const
{
    return d->format;
}

QString TwoDRWSurface::to_string(double v) const
{
    char s[32];
    snprintf(s, 32, d->format.toStdString().c_str(), v);
    return  QString(s);
}

QRectF TwoDRWSurface::surface() const
{
    return d->surface;
}

int TwoDRWSurface::animationDuration() const
{
    return  d->animation_duration;
}

int TwoDRWSurface::targetMoveSignalDelay() const
{
    return d->w_data.signal_timer->interval();
}

QPointF TwoDRWSurface::mapToSurface(const QPointF &pos) const
{
    return QPointF(pos.x() - d->surface.left(), pos.y() - d->surface.top());
}

QRectF TwoDRWSurface::targetBoundingRect() const
{
    QPointF pos = mapFromValue(QPointF(d->xw, d->yw));
    return QRectF(pos.rx() - d->write_indicator_size/2, pos.ry() - d->write_indicator_size/2,
                  d->write_indicator_size, d->write_indicator_size);
}

void TwoDRWSurface::updateSurface()
{
    const QFont& f = font();
    QFontMetrics fm(f);
    const QRect& r = rect();
    d->x_scale_h = m_calc_scale_space(XScale, &fm);
    d->y_scale_w = m_calc_scale_space(YScale, &fm);
    int r_m = qMax(d->read_indicator_size/2, d->write_indicator_size/2);
    int l_m = qMax(r_m, d->y_scale_w);
    int top_m = r_m;
    int bot_m = qMax(r_m, d->x_scale_h);
    d->surface = QRectF(r.left() + l_m, r.top() + top_m, r.width() - l_m - r_m, r.height()- top_m - bot_m);
}

void TwoDRWSurface::setXMin(double m)
{
    d->xm = m;
    updateSurface(); // recalc scale width
    update();
}

void TwoDRWSurface::setXMax(double m)
{
    d->xM = m;
    updateSurface(); // recalc scale width
    update();
}

void TwoDRWSurface::setYMin(double m)
{
    d->ym = m;
    updateSurface(); // recalc scale width
    update();
}

void TwoDRWSurface::setYMax(double m)
{
    d->yM = m;
    updateSurface(); // recalc scale width
    update();
}

void TwoDRWSurface::setReadOnly(bool ro)
{
    d->read_only = ro;
    update();
}

void TwoDRWSurface::setX(double x)
{
    d->x = x;
    update();
}

void TwoDRWSurface::setY(double y)
{
    d->y = y;
    update();
}

void TwoDRWSurface::animateToX(double x)
{
    QPropertyAnimation *xa = findChild<QPropertyAnimation *>("xAnim");
    if(xa->state() == QAbstractAnimation::Running)
        xa->stop();
    xa->setStartValue(d->x);
    xa->setEndValue(x);
    xa->start();
}

void TwoDRWSurface::animateToY(double y)
{
    QPropertyAnimation *ya = findChild<QPropertyAnimation *>("yAnim");
    if(ya->state() == QAbstractAnimation::Running)
        ya->stop();
    ya->setStartValue(d->y);
    ya->setEndValue(y);
    ya->start();
}

void TwoDRWSurface::setX_W(double xw)
{
    d->xw = xw;
    update();
}

void TwoDRWSurface::setY_W(double yw)
{
    d->yw = yw;
    update();
}

void TwoDRWSurface::setFormat(const QString &f)
{
    d->format = f;
    update();
}

void TwoDRWSurface::setSurface(const QRectF &r)
{
    d->surface = r;
    update();
}

void TwoDRWSurface::setAnimationDuration(int ms)
{
    foreach(QPropertyAnimation *a, findChildren<QPropertyAnimation *>()) {
        a->setDuration(ms);
    }
}

void TwoDRWSurface::setTargetMoveSignalDelay(int ms)
{
    d->w_data.signal_timer->setInterval(ms);
}

void TwoDRWSurface::paintEvent(QPaintEvent *event)
{
    QFrame::paintEvent(event);
    if(d->x_scale_h < 0)
        updateSurface(); // first time
    QPainter painter(this);

    // temp
    painter.setPen(QPen(Qt::black));
    QRect widrect = rect();
    widrect.setRight(widrect.right() - 1);
    widrect.setBottom(widrect.bottom() - 1);
    painter.drawRect(widrect);
    // end temp

    drawBackground(&painter, event);
    drawScale(&painter, event, YScale);
    drawScale(&painter, event, XScale);
    drawRead(&painter, event);
    if(!d->read_only)
        drawWrite(&painter, event);
}

void TwoDRWSurface::mouseMoveEvent(QMouseEvent *e)
{
    QPointF pos = e->pos();
    const QRectF& r = targetBoundingRect();
    if(r.contains(pos) && d->w_data.press_pos == QPointF())
        setCursor(QCursor(Qt::OpenHandCursor));
    else if(d->w_data.press_pos == QPointF()) {
        setCursor(QCursor(Qt::ArrowCursor));
    }
    if(d->w_data.press_pos != QPointF()) {
        setCursor(QCursor(Qt::ClosedHandCursor));
        pos.rx() -= d->w_data.dist.rx();
        pos.ry() -= d->w_data.dist.ry();
//        qDebug () << __PRETTY_FUNCTION__ << "original pos" << e->pos() << "pos" << pos
//                  << "surf" << d->surface << "surface contains pos " << d->surface.contains(pos)
//                  << "xw" << d->xw << "yw" << d->yw;
        const QPointF val_pt = mapFromPos(pos);
        d->xw = val_pt.x();
        d->yw = val_pt.y();
        if(d->xw < d->xm) d->xw = d->xm;
        else if(d->xw > d->xM) d->xw = d->xM;
        if(d->yw < d->ym) d->yw = d->ym;
        else if(d->yw > d->yM) d->yw = d->yM;
        if(!d->read_only)
            d->w_data.signal_timer->start();
    }
    update();
}

void TwoDRWSurface::mousePressEvent(QMouseEvent *e)
{
    const QRectF& r = targetBoundingRect();
    if(d->surface.contains(e->pos()) && r.contains(e->pos())) {
        d->w_data.press_pos = e->pos();
        d->w_data.dist = QPointF(e->pos().rx() - r.center().rx(),
                                 e->pos().ry() - r.center().ry());
        qDebug() << "saving press pos " << e->pos() << "dist " << d->w_data.dist;
        setCursor(QCursor(Qt::ClosedHandCursor));
    }
}

void TwoDRWSurface::mouseReleaseEvent(QMouseEvent *e)
{
    d->w_data.press_pos = QPointF();
    const QRectF& r = targetBoundingRect();
    if(r.contains(e->pos()))
        setCursor(QCursor(Qt::OpenHandCursor));
}

void TwoDRWSurface::resizeEvent(QResizeEvent *re)
{
    QWidget::resizeEvent(re);
    updateSurface();
}

int TwoDRWSurface::m_calc_scale_space(Scale sc, const QFontMetrics *fm) const
{
    int l = 0, w;
    char s[32];
    if(sc == YScale) {
        QVector<double> vs;
        vs << d->ym << d->yM;
        foreach(double v, vs) {
            snprintf(s, 32, d->format.toStdString().c_str(), v);
            if(( w = fm->width(s)) > l)
                l = w;
        }
    }
    else {
        l = fm->height();
    }
    return l;
}

void TwoDRWSurface::m_onTargetSignalTimeout()
{
    qDebug() << __PRETTY_FUNCTION__ << d->xw << d->yw;
    emit x_targetMoveFinished(d->xw);
    emit y_targetMoveFinished(d->yw);
}
