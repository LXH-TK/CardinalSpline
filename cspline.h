#ifndef CSPLINE_H
#define CSPLINE_H

struct CPT {
    double x;
    double y;
};

class CSpline {
public:
    //生成插值点
    CSpline(double x[50], double y[50], int n, int grain, float tension);
    ~CSpline();
    //斜率数组
    float slope[2056];
    //插值点数组
    CPT Spline[2056];


private:
    void CubicSpline(int n, CPT *knots, int grain, float tension);
    void setSlope(int np, float tension, int grain);
    //控制点数组
    CPT *knots;
    //Cardinal矩阵
    float M[16];
    void GetMatrix(float tension);
    float Matrix(float a, float b, float c, float d, float alpha);
};

#endif // CSPLINE_H
