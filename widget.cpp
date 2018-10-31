#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget) {
    ui->setupUi(this);
    n = 0;
    flag = 0;
    base_repaint = false;
    pix = QPixmap(640, 300);
    pix.fill(Qt::white);
    R = 0;

    ui->showButton->setEnabled(false);
    ui->moveButton->setEnabled(false);
    ui->drawButton->setEnabled(true);
    ui->clearButton->setEnabled(true);

    obj = new QLabel(this);
    obj->setFixedSize(100, 100);
    obj->hide();
    img = new QImage;
    img->load(":/images/car.png");

    //每隔一段时间根据路径方向对图像进行旋转
    timer = new QTimer;
    //Qtimer的槽位处理旋转的rotate()函数
    //当达到超时时间，则发射信号，执行指定的槽函数
    connect(timer, SIGNAL(timeout()), this, SLOT(rorate()));
    pause = 0;
}

void Widget::on_drawButton_clicked() {
    flag = 1;
    //从ui中获取相应参数
    grain = ui->grainSlider->value();
    tension = (float)ui->tensionSlider->value()/1000;
    if(n > 1) {
        //生成Cardinal曲线
        mCSpline = new CSpline(canvas_x, canvas_y, n, grain, tension);

        //激活Cardinal曲线按钮
        ui->showButton->setEnabled(true);
        ui->moveButton->setEnabled(true);

        update();
    }
}

void Widget::on_clearButton_clicked() {
    flag = 0;
    n = 0;
    R = 0;
    grain = tension = 0;
    base_repaint = false;

    pix.fill(Qt::white);
    ui->showButton->setEnabled(false);
    ui->moveButton->setEnabled(false);

    ui->grainSlider->setValue(3);
    ui->tensionSlider->setValue(0);
    ui->speedSlider->setValue(7500);

    obj->hide();

    update();
}

void Widget::on_showButton_clicked() {
    flag = 2;
    update();
}

/*
void Widget::on_moveButton_clicked() {
    R = 0;
    grain = ui->grainSlider->value();
    tension = (float)ui->tensionSlider->value()/1000;
    speed = ui->speedSlider->value();

    //每隔该段时长调整一次小车角度
    timer->setInterval(speed/(n-1)/grain);
    double u = 1.0f/((n-1)*grain);
    count = (n-1)*grain;

    //设置动画时长 初始帧 关键帧 终止帧
    ani = new QPropertyAnimation(obj, "pos");
    ani->setDuration(speed);
    ani->setStartValue(QPoint(mCSpline->Spline[0].x-40, mCSpline->Spline[0].y-60));
    for(int i=1;i<count;i++)
        //参数：时间百分比 关键帧越多小车路线更符合曲线
        ani->setKeyValueAt(u*i, QPoint(mCSpline->Spline[i].x-40, mCSpline->Spline[i].y-60));
    ani->setEndValue(QPoint(mCSpline->Spline[count].x-40, mCSpline->Spline[count].y-60));

    timer->start();
    update();
}
*/
void Widget::on_moveButton_clicked() {
    flag = 4;
    R = 0;
    pause = 1;
    grain = ui->grainSlider->value();
    tension = (float)ui->tensionSlider->value()/1000;
    speed = ui->speedSlider->value();

    //每个这样一个时长调整小车角度一次
    timer->setInterval(speed/(n-1)/grain);
    double u = 1.0f/((n-1)*grain);
    count = (n-1)*grain;

    //设置动画时间 初始帧 关键帧 结束帧
    ani = new QPropertyAnimation(obj, "pos");
    ani->setDuration(speed);
    ani->setStartValue(QPoint(mCSpline->Spline[0].x-40, mCSpline->Spline[0].y-60));
    for(int i=1;i<count;i++)
        //参数：时间百分比 关键值 (帧数越多小车更不会偏移曲线)
        ani->setKeyValueAt(u*i, QPoint(mCSpline->Spline[i].x-40, mCSpline->Spline[i].y-60));
    ani->setEndValue(QPoint(mCSpline->Spline[count].x-40, mCSpline->Spline[count].y-60));

    timer->start();
    update();
}

/*
void Widget::mousePressEvent(QMouseEvent *e) {
    double x = e->pos().x();
    double y = e->pos().y();
    if y in drawArea:
        canvas_x[n] = x;
        canvas_y[n] = y;
        n++;
        endPoint.setX(x);
        endPoint.setY(y);
    end
    //调用paintEvent，更新画面
    update();
}

void Widget::paintEvent(QPaintEvent *) {
    //直线模式
    if flag == 1:
        drawLine(startPoint, endPoint);
        startPoint = endPoint;
        ......(painting others)
    //曲线模式
    else if flag == 2:
        for j = 0:(n-2)*grain
            Point1 = mCSpline->Spline[j];
            Point2 = mCSpline->Spline[j+1];
            paint.drawLine(aPoint, bPoint);
        end
    //显示插值点
    else if flag == 3:
        for j = 0:(n-2)*grain
            Point = mCSpline->Spline[j];
            paint.drawEllipse(Point, 1, 1);
        end
    ......(painting others)
    end
}
*/
void Widget::mousePressEvent(QMouseEvent *e) {
    double x = e->pos().x();
    double y = e->pos().y();
    if(y < 300) {
        canvas_x[n] = x;
        canvas_y[n] = y;
        n++;
        endPoint.setX(x);
        endPoint.setY(y);
    }

    update();
}

void Widget::paintEvent(QPaintEvent *) {
    //要构建两个(一个this一个pix)才能在widget上画出来
    QPainter paint(&pix);
    QPainter painter(this);

    paint.setRenderHint(QPainter::Antialiasing);

    //重新绘制控制点及其连线
    if(base_repaint == true) {
        if(n == 1) {
            startPoint.setX(canvas_x[0]);
            startPoint.setY(canvas_y[0]);
            paint.setPen(Qt::blue);
            paint.drawEllipse(startPoint, 2, 2);
        }
        else {
            for(int i=0;i<n-1;i++) {
                startPoint.setX(canvas_x[i]);
                startPoint.setY(canvas_y[i]);
                endPoint.setX(canvas_x[i+1]);
                endPoint.setY(canvas_y[i+1]);
                paint.setPen(QPen(QColor(0,255,255), 1, Qt::SolidLine, Qt::RoundCap));
                paint.drawLine(startPoint, endPoint);
                paint.setPen(Qt::blue);
                paint.drawEllipse(startPoint, 2, 2);
            }
            paint.drawEllipse(endPoint, 2, 2);
        }
    }

    //设置笔刷
    paint.setPen(QPen(QColor(0,255,255), 1, Qt::SolidLine, Qt::RoundCap));
    //绘制控制点及其连线
    if(flag == 0 && n != 0) {
        if(n == 1)
            startPoint = endPoint;
        paint.drawLine(startPoint, endPoint);
        startPoint = endPoint;
        paint.setPen(Qt::blue);
        paint.drawEllipse(startPoint, 2, 2);
    }
    //绘制插值后的曲线
    else if(flag == 1) {
        for(int j=0;j<(n-1)*grain;j++) {
            aPoint.setX(mCSpline->Spline[j].x);
            aPoint.setY(mCSpline->Spline[j].y);
            bPoint.setX(mCSpline->Spline[j+1].x);
            bPoint.setY(mCSpline->Spline[j+1].y);
            paint.setPen(QPen(QColor(255, 0, 255), 1, Qt::SolidLine, Qt::RoundCap));
            paint.drawLine(aPoint, bPoint);
        }
    }
    //显示插值点
    else if(flag == 2) {
        for(int j=0;j<(n-1)*grain;j++) {
            aPoint.setX(mCSpline->Spline[j].x);
            aPoint.setY(mCSpline->Spline[j].y);
            paint.setPen(Qt::black);
            paint.drawEllipse(aPoint, 1, 1);
        }
    }
    painter.drawPixmap(0, 0, pix);
}

Widget::~Widget() {
    delete ui;
}

//grain会影响曲线的平滑程度
void Widget::on_grainSlider_valueChanged() {
    pix.fill(Qt::white);
    base_repaint = true;
    ui->drawButton->clicked(true);
    ui->grainValue->setText(QString::number(grain, 10));
}

void Widget::on_tensionSlider_valueChanged() {
    pix.fill(Qt::white);
    base_repaint = true;
    ui->drawButton->clicked(true);
    //float转化为QString，两位小数
    ui->tensionValue->setText(QString::number(tension, 'f', 2));
}

void Widget::on_speedSlider_valueChanged() {
    ui->speedValue->setText(QString::number(1.00 - ((float)ui->speedSlider->value() - 5000)/5000, 'f', 2));
}

/*
void Widget::rotate() {
    currentPoint P[index];
    if slope[index]*slope[index-1] < 0:
        //分情况讨论斜率，从曲线形状入手，画图更直观
        if angle+R < 0:
            if spline[index+2].y > spline[index-2].y:
                angle = atan(slope[index]);
            else:
                angle = atan(slope[index]);
                R += 180
            end
        end
        else if angle+R > 0:
            if spline[index+2].y > spline[index-2].y:
                angle = atan(slope[index]);
            else:
                angle = atan(slope[index]);
                R -= 180
            end
        end
    else
        angle = atan(slope[index]);
    end
    //R要及时更新
    if R == 360 || R == -360:
        R = 0;
    else if R+angle >= 180:
        R = -180;
    else if R+angle <= -180
        R = 180;
    end

    //旋转方法
    QMatrix m;
    m.rotate(angle + R);
    obj->setPixmap(QPixmap::fromImage(*img).transformed(matrix,Qt::SmoothTransformation));
}
*/
void Widget::rorate() {
    int t = ani->currentTime();
    int index = round(t/speed * count);
    //斜率反向，判断是否经过斜率为0的过渡
    if(index > 0 && index != pause && mCSpline->slope[index-1]*mCSpline->slope[index] < 0) {
        pause = index;
        if(angle+R < 0) {
            if(mCSpline->Spline[index+2].y > mCSpline->Spline[index-2].y)
                angle = atan(mCSpline->slope[index]) / M_PI * 180;
            else {
                angle = atan(mCSpline->slope[index]) / M_PI * 180;
                R -= 180;
            }
        }
        else if(angle+R > 0){
            if(mCSpline->Spline[index+2].y < mCSpline->Spline[index-2].y)
                angle = atan(mCSpline->slope[index]) / M_PI * 180;
            else {
                angle = atan(mCSpline->slope[index]) / M_PI * 180;
                R += 180;
            }
        }
    }
    else
        angle = atan(mCSpline->slope[index]) / M_PI * 180;

    if(R == 360 || R == -360)
        R = 0;
    else if(R+angle >= 180)
        R = -180;
    else if(R+angle <= -180)
        R = 180;

    QMatrix matrix;
    matrix.rotate(angle + R);
    QPixmapCache::clear();
    obj->setPixmap(QPixmap::fromImage(*img).transformed(matrix,Qt::SmoothTransformation));
    if(t == 0)
        ani->start();
    if(t > 100)
        obj->show();
    if(t >= speed-50) {
        timer->stop();
        ani->stop();
    }
}
