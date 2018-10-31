#include "cspline.h"

/*
CSpline::CSpline(double x[50], double y[50], int n, int grain, float tension) {
    controlPoints[50];
    //重新复制控制点
    for i = 1:n
        controlPoints[i].x = x[i-1];
        controlPoints[i].y = y[i-1];
    end
    controlPoints[0].x = x[0];
    controlPoints[0].y = y[0];
    controlPoints[n+1].x = x[n-1];
    controlPoints[50].y = y[n-1];
    //生成插值点
    CubicSpline(n+2, controlPoints, grain, tension);
}
*/
CSpline::CSpline(double x[50], double y[50], int n, int grain, float tension) {
    CPT KNOTS[50];
    for(int i=1;i<=n;i++) {
        KNOTS[i].x = x[i-1];
        KNOTS[i].y = y[i-1];
    }
    //复制首尾端点，原因在PPT44页上
    KNOTS[0].x = x[0];
    KNOTS[n+1].x = x[n-1];
    KNOTS[0].y = y[0];
    KNOTS[n+1].y = y[n-1];

    knots = KNOTS;
    CubicSpline(n+2, knots, grain, tension);
    setSlope(n-1, tension, grain);
}

/*
void CSpline::CubicSpline(int n, CPT *controlPoints, int grain, float tension) {
    Point p1, p2, p3, p4;   //四个参考点
    float alpha[50];        //即u，相当于横坐标
    Point *Spline;          //插值点数组
    //根据tension计算矩阵
    GetMatrix(tension);
    //u属于[0,1]
    for i = 0:grain-1
        alpha[i] = i/grain;
    end

    p1 = controlPoints;
    p2 = p1 + 1;
    p3 = p2 + 1;
    p4 = p3 + 1;
    //n-2个控制点，每段grain个插值点，计算插值点坐标
    for i = 0:n-4
        for j = 0:grain
            Spline->x = Matrix(p1->x, p2->x, p3->x, p4->x);
            Spline->x = Matrix(p1->x, p2->x, p3->x, p4->x);
            Spline ++;
        end
        p1++; p2++; p3++; p4++;
    end
}
*/
void CSpline::CubicSpline(int n, CPT *knots, int grain, float tension) {
    CPT *s, *km1, *k0, *k1, *k2;
    //alpha就是u
    float alpha[50];
    //将u代入，计算矩阵
    GetMatrix(tension);

    //根据分段值确定alpha[]即u[]，从而绘制曲线，相当于x坐标，u∈[0,1]
    for(int i=0;i<grain;i++)
        alpha[i] = ((float)i)/grain;

    s = Spline;

    //四个参数点
    km1 = knots;
    k0 = km1 + 1;
    k1 = k0 + 1;
    k2 = k1 + 1;

    //求插值点的坐标，一共n-3段，每段grain个插值点
    for(int i=0;i<n-3;i++) {
        for(int j=0;j<grain;j++) {
            s->x = Matrix(km1->x, k0->x, k1->x, k2->x, alpha[j]);
            s->y = Matrix(km1->y, k0->y, k1->y, k2->y, alpha[j]);
            s++;
        }
        km1++;
        k0++;
        k1++;
        k2++;
    }
    s->x = k0->x;
    s->y = k0->y;
}

//矩阵见PPT，tension是'_tao_'
void CSpline::GetMatrix(float tension) {
    M[0] = -tension;   M[1] = 2.-tension;  M[2] = tension-2.;   M[3] = tension;
    M[4] = 2.*tension; M[5] = tension-3.;  M[6] = 3.-2*tension; M[7] = -tension;
    M[8] = -tension;   M[9] = 0.;          M[10] = tension;     M[11] = 0.;
    M[12] = 0.;        M[13] = 1.;         M[14] = 0.;          M[15] = 0.;
}

float CSpline::Matrix(float a, float b, float c, float d, float alpha) {
    float p0, p1, p2, p3;
    p0 = M[0]*a + M[1]*b + M[2]*c + M[3]*d;
    p1 = M[4]*a + M[5]*b + M[6]*c + M[7]*d;
    p2 = M[8]*a + M[9]*b + M[10]*c + M[11]*d;
    p3 = M[12]*a + M[13]*b + M[14]*c + M[15]*d;
    return (p3 + alpha*(p2+alpha*(p1+alpha*p0)));
}

//利用相邻的两个点计算斜率
void CSpline::setSlope(int np, float tension, int grain) {
    //np段，每段grain个
    int n = np*grain;
    //PPT-39页斜率计算
    for(int i=1;i<=n-1;i++) {
        slope[i] = tension *
                   (Spline[i+1].y - Spline[i-1].y) /
                   (Spline[i+1].x - Spline[i-1].x);
    }
    //PPT-44页端点斜率计算
    slope[0] = tension*( (2*Spline[1].y - Spline[0].y - Spline[2].y) /
                     (2*Spline[1].x - Spline[0].x - Spline[2].x) );
    slope[n] = tension*( (2*Spline[n-1].y - Spline[n].y - Spline[n-2].y) /
                       (2*Spline[n-1].x - Spline[n].x - Spline[n-2].x) );
}

CSpline::~CSpline() {
}
