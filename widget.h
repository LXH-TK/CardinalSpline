#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <cspline.h>
#include <QPoint>
#include <QPixmap>
#include <QPixmapCache>
#include <iostream>
#include <QLabel>
#include <QTimer>
#include <QPropertyAnimation>
#include <QImage>
#include <qmath.h>
#include <QMatrix>

using namespace std;

namespace Ui {
class Widget;
}

class Widget : public QWidget {
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    //实例化样条曲线类
    CSpline *mCSpline;

protected:
    //系统会自动调用
    void mousePressEvent(QMouseEvent *e);
    void paintEvent(QPaintEvent *);

//注意信号和槽对应的slots
private slots:
    //显示样条曲线
    void on_showButton_clicked();
    //点击绘制按钮
    void on_drawButton_clicked();
    //点击清除画布按钮
    void on_clearButton_clicked();
    //小车运动
    void on_moveButton_clicked();

    //是用下面两个函数时尽量不要重新setValue，否则也会执行
    //控制grain
    void on_grainSlider_valueChanged();
    //控制tension
    void on_tensionSlider_valueChanged();
    //控制速度
    void on_speedSlider_valueChanged();
    void rorate();

private:
    Ui::Widget *ui;

    //画布上的点
    double canvas_x[50];
    double canvas_y[50];
    //控制绘制模式
    int flag;
    //用于画点和线
    QPoint startPoint;
    QPoint endPoint;
    QPoint aPoint;
    QPoint bPoint;
    //重画基础线条
    bool base_repaint;
    //图像
    QPixmap pix;
    //其他变量
    int n;
    int grain;
    float tension;

    //小车
    QLabel *obj;
    QPropertyAnimation *ani;
    QTimer *timer;
    int count;
    float speed;
    QImage *img;
    float angle;
    int pause;
    //小车旋转情况，与斜率有关
    int R;
};

#endif // WIDGET_H
