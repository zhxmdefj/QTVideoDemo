#ifndef IVIDEOTREAD_H
#define IVIDEOTREAD_H

#include <QThread>
#include <QImage>

class IVideoTread : public QThread
{
    Q_OBJECT
public:
    explicit IVideoTread(QObject *parent = Q_NULLPTR);
    void openCamera();

protected:
    virtual void run();

Q_SIGNALS:
    void sgImage(QImage);

private:
    QString _cameraName;

};

#endif // IVIDEOTREAD_H
