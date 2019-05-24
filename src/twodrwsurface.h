#ifndef TWODRWSURFACE_H
#define TWODRWSURFACE_H

#include "cumbia-extra-widgets_global.h"
#include <QFrame>

class TwoDRWSurfacePrivate;

class TwoDRWSurface : public QFrame
{

Q_OBJECT
    Q_PROPERTY(double xMin READ xMin WRITE setXMin DESIGNABLE true)
    Q_PROPERTY(double xMax READ xMax WRITE setXMax DESIGNABLE true)
    Q_PROPERTY(double yMin READ yMin WRITE setYMin DESIGNABLE true)
    Q_PROPERTY(double yMax READ yMax WRITE setYMax DESIGNABLE true)
    Q_PROPERTY(double x READ x WRITE setX DESIGNABLE true)
    Q_PROPERTY(double y READ y WRITE setY DESIGNABLE true)
    Q_PROPERTY(bool readOnly READ readOnly WRITE setReadOnly DESIGNABLE true)
    Q_PROPERTY(int animationDuration READ animationDuration WRITE setAnimationDuration DESIGNABLE true)
    Q_PROPERTY(int targetMoveSignalDelay READ targetMoveSignalDelay WRITE setTargetMoveSignalDelay DESIGNABLE true)

public:

    enum Scale {XScale, YScale };

    TwoDRWSurface(QWidget *parent = nullptr);

    virtual void drawRead(QPainter *pa, QPaintEvent *p);
    virtual void drawWrite(QPainter *pa, QPaintEvent *p);
    virtual void drawBackground(QPainter *p, QPaintEvent *pe);
    virtual void drawScale(QPainter *pa, QPaintEvent *p, Scale s);

    QPointF mapFromPos(const QPointF &pos)  const;
    QPointF mapFromValue(const QPointF &xy) const;

    double xMin() const;
    double xMax() const;
    double yMin() const;
    double yMax() const;

    bool readOnly() const;

    double x() const;
    double y() const;
    double x_w() const;
    double y_w() const;

    QString format() const;

    QString to_string(double d) const;

    QRectF surface() const;

    int animationDuration() const;

    int targetMoveSignalDelay() const;

    QPointF mapToSurface(const QPointF &pos) const;
    QRectF targetBoundingRect() const;

    void updateSurface();

signals:
    void x_targetMoveFinished(double x);
    void y_targetMoveFinished(double y);

public slots:
    void setXMin(double m);
    void setXMax(double m);
    void setYMin(double m);
    void setYMax(double m);

    void setReadOnly(bool ro);

    void setX(double x);
    void setY(double y);

    void animateToX(double x);
    void animateToY(double y);

    void setX_W(double xw);
    void setY_W(double yw);

    void setFormat(const QString &f);

    void setSurface(const QRectF& r);

    void setAnimationDuration(int ms);
    void setTargetMoveSignalDelay(int ms);

protected:
    void paintEvent(QPaintEvent *event);

    void mouseMoveEvent(QMouseEvent *e);

    void mousePressEvent(QMouseEvent *e);

    void mouseReleaseEvent(QMouseEvent *e);

    void resizeEvent(QResizeEvent *re);

private:
    TwoDRWSurfacePrivate *d;

    int m_calc_scale_space(Scale s, const QFontMetrics *fm) const;

private slots:
    void m_onTargetSignalTimeout();
};


#endif // TWODRWSURFACE_H
