#ifndef QURWSURFACECONNECTOR_H
#define QURWSURFACECONNECTOR_H

#include <QObject>

class QuRWSurfaceConnector : public QObject
{
    Q_OBJECT
public:
    explicit QuRWSurfaceConnector(QObject *parent = nullptr);

signals:

public slots:
};

#endif // QURWSURFACECONNECTOR_H
