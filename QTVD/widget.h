#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QVector>

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void paintEvent(QPaintEvent *event);

private:
    void drawSpline();
    qreal N(int k, int i, qreal u);
    qreal N1(int i, qreal u);
    qreal N2(int i, qreal u);
    qreal N3(int i, qreal u);

private:
    QVector<QPointF>    m_ctrlPoints;       // 控制点
    QVector<QPointF>    m_curvePoints;      // 曲线上的点
};


#endif // WIDGET_H
