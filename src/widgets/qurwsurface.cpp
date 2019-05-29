#include "qurwsurface.h"
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

class QuRWSurfacePrivate {
public:
    bool read_only, animation_enabled;
    double xm, xM, ym, yM, x, y, xw, yw;
    int x_scale_h, y_scale_w, xmargin, ymargin;
    int read_indicator_size, write_indicator_size;
    int animation_duration;
    QString format;
    QRectF surface;
    TwoDRWWriterData w_data;
    QString message;
};

QuRWSurface::QuRWSurface(QWidget *parent) : QFrame (parent)
{
    d = new QuRWSurfacePrivate;
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
    d->w_data.signal_timer->setInterval(80);
    d->w_data.signal_timer->setSingleShot(true);
    connect(d->w_data.signal_timer, SIGNAL(timeout()), this, SLOT(m_onTargetSignalTimeout()));

    setMouseTracking(true);
    QPropertyAnimation *xAnim = new QPropertyAnimation(this, "x", this);
    QPropertyAnimation *yAnim = new QPropertyAnimation(this, "y", this);
    xAnim->setObjectName("xAnim");
    yAnim->setObjectName("yAnim");
    setAnimationDuration(d->animation_duration);
}

void QuRWSurface::drawRead(QPainter *pa, QPaintEvent *p)
{
    const QPen savePen(pa->pen());
    QPen pe = pa->pen();
    pe.setColor(QColor(Qt::green));
    pe.setWidthF(1.2);
    pa->setPen(pe);
    const double w = d->surface.width(), h = d->surface.height(), x0 = d->surface.x(), y0 = d->surface.y();
    const QPointF xyp = mapFromValue(QPointF(d->x, d->y));
    const double &xp = xyp.x(), &yp = xyp.y();

    if(xp >= 0 && yp >= 0) {
        pa->drawLine(xp - d->read_indicator_size/2, yp - d->read_indicator_size/2,
                     xp + d->read_indicator_size/2,  yp + d->read_indicator_size/2);
        pa->drawLine(xp + d->read_indicator_size/2, yp - d->read_indicator_size/2,
                     xp - d->read_indicator_size/2, yp + d->read_indicator_size/2);
    }
    // restore pen
    pa->setPen(savePen);
}

void QuRWSurface::drawWrite(QPainter *pa, QPaintEvent *p)
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

void QuRWSurface::drawBackground(QPainter *p, QPaintEvent *pe)
{
    p->fillRect(d->surface, QColor(Qt::white));
    QColor c(Qt::darkYellow);
    QPen pen(c);
    p->setPen(pen);
    p->drawRect(d->surface);
}

void QuRWSurface::drawScale(QPainter *pa, QPaintEvent *p, QuRWSurface::Scale s)
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

QPointF QuRWSurface::mapFromPos(const QPointF &pos) const
{
    const double w = d->surface.width(), h = d->surface.height(), x0 = d->surface.left(), y0 = d->surface.top();
    double px = (pos.x() - x0) * (d->xM - d->xm) / w;
    double py = (h - pos.y() + y0) * (d->yM - d->ym) / h;
    return QPointF(d->xm + px, d->ym + py);
}

QPointF QuRWSurface::mapFromValue(const QPointF &xy) const
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

double QuRWSurface::xMin() const
{
    return d->xm;
}

double QuRWSurface::xMax() const
{
    return d->xM;
}

double QuRWSurface::yMin() const
{
    return  d->ym;
}

double QuRWSurface::yMax() const
{
    return  d->yM;
}

bool QuRWSurface::readOnly() const
{
    return d->read_only;
}

double QuRWSurface::x() const
{
    return  d->x;
}

double QuRWSurface::y() const
{
    return  d->y;
}

double QuRWSurface::x_w() const
{
    return  d->xw;
}

double QuRWSurface::y_w() const
{
    return  d->yw;
}

QString QuRWSurface::format() const
{
    return d->format;
}

QString QuRWSurface::to_string(double v) const
{
    char s[32];
    snprintf(s, 32, d->format.toStdString().c_str(), v);
    return  QString(s);
}

QRectF QuRWSurface::surface() const
{
    return d->surface;
}

int QuRWSurface::animationDuration() const
{
    return  d->animation_duration;
}

int QuRWSurface::targetMoveSignalDelay() const
{
    return d->w_data.signal_timer->interval();
}

QPointF QuRWSurface::mapToSurface(const QPointF &pos) const
{
    return QPointF(pos.x() - d->surface.left(), pos.y() - d->surface.top());
}

QRectF QuRWSurface::targetBoundingRect() const
{
    QPointF pos = mapFromValue(QPointF(d->xw, d->yw));
    return QRectF(pos.rx() - d->write_indicator_size/2, pos.ry() - d->write_indicator_size/2,
                  d->write_indicator_size, d->write_indicator_size);
}

void QuRWSurface::updateSurface()
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

void QuRWSurface::setXMin(double m)
{
    d->xm = m;
    updateSurface(); // recalc scale width
    update();
}

void QuRWSurface::setXMax(double m)
{
    d->xM = m;
    updateSurface(); // recalc scale width
    update();
}

void QuRWSurface::setYMin(double m)
{
    d->ym = m;
    updateSurface(); // recalc scale width
    update();
}

void QuRWSurface::setYMax(double m)
{
    d->yM = m;
    updateSurface(); // recalc scale width
    update();
}

void QuRWSurface::setReadOnly(bool ro)
{
    d->read_only = ro;
    update();
}

void QuRWSurface::setX(double x)
{
    d->x = x;
    update();
}

void QuRWSurface::setY(double y)
{
    d->y = y;
    update();
}

/*! \brief sets x and y from a vector of double
 *
 * @param v a vector of size >= 2. v[0] is used as x, v[1] as y
 */
void QuRWSurface::setXY(const QVector<double> &v)
{
    if(v.size() >= 2) {
        d->x = v[0];
        d->y = v[1];
        update();
    }
}

void QuRWSurface::animateToX(double x)
{
    QPropertyAnimation *xa = findChild<QPropertyAnimation *>("xAnim");
    if(xa->state() == QAbstractAnimation::Running)
        xa->stop();
    xa->setStartValue(d->x);
    xa->setEndValue(x);
    xa->start();
}

void QuRWSurface::animateToY(double y)
{
    QPropertyAnimation *ya = findChild<QPropertyAnimation *>("yAnim");
    if(ya->state() == QAbstractAnimation::Running)
        ya->stop();
    ya->setStartValue(d->y);
    ya->setEndValue(y);
    ya->start();
}

void QuRWSurface::setX_W(double xw)
{
    d->xw = xw;
    update();
}

void QuRWSurface::setY_W(double yw)
{
    d->yw = yw;
    update();
}

void QuRWSurface::setW(const QVector<double> &wv)
{
    if(wv.size() == 2) {
        d->xw = wv[0];  d->yw = wv[1];
        update();
    }
}

void QuRWSurface::setFormat(const QString &f)
{
    d->format = f;
    update();
}

void QuRWSurface::setSurface(const QRectF &r)
{
    d->surface = r;
    update();
}

void QuRWSurface::setAnimationDuration(int ms)
{
    foreach(QPropertyAnimation *a, findChildren<QPropertyAnimation *>()) {
        a->setDuration(ms);
    }
}

void QuRWSurface::setTargetMoveSignalDelay(int ms)
{
    d->w_data.signal_timer->setInterval(ms);
}

void QuRWSurface::writerConfigured(bool ok)
{
    setEnabled(ok);
    if(!ok)
        setToolTip("QuRWSurface: writer configuration error");
}

void QuRWSurface::paintEvent(QPaintEvent *event)
{
    QFrame::paintEvent(event);
    if(d->x_scale_h < 0)
        updateSurface(); // first time
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    drawBackground(&painter, event);
    drawScale(&painter, event, YScale);
    drawScale(&painter, event, XScale);
    if(!d->read_only)
        drawWrite(&painter, event);
    drawRead(&painter, event);
}

void QuRWSurface::mouseMoveEvent(QMouseEvent *e)
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

        const QPointF val_pt = mapFromPos(pos);
        d->xw = val_pt.x();
        d->yw = val_pt.y();
//        qDebug () << __PRETTY_FUNCTION__ << "original pos" << e->pos() << "pos" << pos
//                  << "surf" << d->surface << "surface contains pos " << d->surface.contains(pos)
//                  << "xw" << d->xw << "yw" << d->yw;
        if(d->xw < d->xm) d->xw = d->xm;
        else if(d->xw > d->xM) d->xw = d->xM;
        if(d->yw < d->ym) d->yw = d->ym;
        else if(d->yw > d->yM) d->yw = d->yM;

        if(!d->read_only)
            d->w_data.signal_timer->start();
    }
    update();
}

void QuRWSurface::mousePressEvent(QMouseEvent *e)
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

void QuRWSurface::mouseReleaseEvent(QMouseEvent *e)
{
    d->w_data.press_pos = QPointF();
    const QRectF& r = targetBoundingRect();
    if(r.contains(e->pos()))
        setCursor(QCursor(Qt::OpenHandCursor));
}

void QuRWSurface::resizeEvent(QResizeEvent *re)
{
    QWidget::resizeEvent(re);
    updateSurface();
}

int QuRWSurface::m_calc_scale_space(Scale sc, const QFontMetrics *fm) const
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

void QuRWSurface::m_onTargetSignalTimeout()
{
    emit x_targetMoveFinished(d->xw);
    emit y_targetMoveFinished(d->yw);
    emit targetMoveFinished(QVector<double> () << d->xw << d->yw);
}
