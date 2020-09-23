#ifndef BSPLINE_H
#define BSPLINE_H

#include <stdio.h>
#include <vector>
#include <iostream>

#include <vec2.h>

using namespace std;

class BSpline
{
public:
    BSpline();
    vector<Vec2> ctrlPoints;
    vector<Vec2> resultPoints;
    int colorMap[256];

    void addOriginPoints(Vec2 point) {
        ctrlPoints.push_back(point);
    }

    void clear() {
        ctrlPoints.clear();
        resultPoints.clear();
    }

    void update() {
        computeBSplineCurveArray3(colorMap, 256, ctrlPoints, ctrlPoints.size());
    }

    float F(float x) {
        float y = x * x * x - x;
        return y;
    }

    void setPandU(vector<float> p, vector<float> u, vector<Vec2> pt, int ptCount)
    {
        if (p.size() == 0 || u.size() == 0 || pt.size() == 0) {
            return;
        }
        try {
            float d[ptCount];
            float w[ptCount];
            for (int i = 0; i < ptCount; i++)
            {
                d[i] = 0.0f;
                w[i] = 0.0f;
            }
            for (int i = 1; i < ptCount - 1; i++)
                d[i] = 2 * (pt[i + 1].x - pt[i - 1].x);
            for (int i = 0; i < ptCount - 1; i++)
                u[i] = (pt[i + 1].x - pt[i].x);
            for (int i = 1; i < ptCount - 1; i++)
                w[i] = 6.0f * ((pt[i + 1].y - pt[i].y) / u[i] - (pt[i].y - pt[i - 1].y) / u[i - 1]);
            for (int i = 1; i < ptCount - 2; i++)
            {
                w[i + 1] = w[i + 1] - w[i] * u[i] / d[i];
                d[i + 1] = d[i + 1] - u[i] * u[i] / d[i];
            }
            p[0] = 0.0f;
            for (int i = ptCount - 2; i > 0; i--)
                p[i] = (w[i] - u[i] * p[i + 1]) / d[i];
            p[ptCount - 1] = 0.0f;
        } catch(exception& e){
            cout << e.what() << endl;
        }
    }

    int getCurvePoint(int i, int v, vector<Vec2> pt, vector<float> u, vector<float> p)
    {
        float t = 0.0f;
        float m = (float)v;
        int getcurvepoint;
        t = (float)((m - pt[i].x) / u[i]);
        getcurvepoint = (int)(t*(pt[i + 1].y) + (1 - t)*(pt[i].y) + u[i] * u[i] * (F(t)*p[i + 1] + F(1 - t)*p[i]) / 6 + 0.5);
        return getcurvepoint;
    }

    int computeBSplineCurveArray3(int colors[], int colorsCount, vector<Vec2> pt, int ptCount){
        try {
            int i;
            int minX = colorsCount;
            int maxX = -1;
            vector<float> p(ptCount);
            for (i = 0; i < ptCount; ++i)
                p[i] = 0.0f;
            vector<float> u(ptCount);
            for (i = 0; i < ptCount; i++)
                u[i] = 0.0f;
            setPandU(p, u, pt, ptCount);

            int xPos, yPos;
            for (i = 0; i < ptCount - 1; i++)
            {
                for (xPos = (int)(pt[i].x); xPos < pt[i + 1].x; xPos++)
                {
                    yPos = getCurvePoint(i, xPos, pt, u, p);
                    if (xPos < minX)
                        minX = xPos;
                    if (xPos > maxX)
                        maxX = xPos;
                    if (yPos > colorsCount)
                        yPos = colorsCount - 1;
                    if (yPos < 0)
                        yPos = 0;
                    colors[xPos] = yPos;

                    if (colors[xPos] > 255) {
                        colors[xPos] = 255;
                    }
                    if (colors[xPos] < 0) {
                        colors[xPos] = 0;
                    }
                }
            }
            if (minX > 255) {
                minX = 255;
            }
            for (i = 0; i < minX; i++) {
                colors[i] = colors[minX];
                if (colors[i] > 255) {
                    colors[i] = 255;
                }
                if (colors[i] < 0) {
                    colors[i] = 0;
                }
            }
            for (i = colorsCount - 1; i > maxX; i--) {
                if (ptCount < 1 )
                    break;
                colors[i] = (int)pt[ptCount - 1].y;
                if (colors[i] > 255) {
                    colors[i] = 255;
                }
                if (colors[i] < 0) {
                    colors[i] = 0;
                }
            }

            return 1;
        } catch(exception& e){
            cout << e.what() << endl;
        }
        return 0;
    }
};

#endif // BSPLINE_H
