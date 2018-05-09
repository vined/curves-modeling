#include <stdlib.h>
#include <GL/glut.h>
#include <stdio.h>
#include <vector>
#include <math.h>

#include "InterpolatingSpline.h"
#include "utils/Colors.h"
#include "utils/Points.h"
#include "utils/Renderer.h"
#include "utils/Draw.h"
#include "utils/SplineUtils.h"


#define THICK_LINE 3
#define THIN_LINE 1
#define IT_CNT 5
#define MAX_IT_CNT 10

const int ESC = 27;
const int ENTER = 13;
const int SPACE = 32;


std::vector<Point> userPoints;

int splineDegree = 3;
int useVariedIntervals = 0;
int showWorkingLines = 0;
int showInterpolationLines = 1;
int showSpline = 1;
int showWire = 1;
int iterationsCount = IT_CNT;
int drawOnlyLastIteration = 0;
int closedSpline = 0;

int allowNewPoints = 1;

int dragging = 0;
int draggingId = -1;

int toggleValue(int val) {
    if (val == 0) {
        return 1;
    } else {
        return 0;
    }
}

void toggleShowWorkingLines() {
    showWorkingLines = toggleValue(showWorkingLines);
}

void toggleUseVariedIntervals() {
    useVariedIntervals = toggleValue(useVariedIntervals);
}

void toggleShowInterpolationLines() {
    showInterpolationLines = toggleValue(showInterpolationLines);
}

void toggleShowSpline() {
    showSpline = toggleValue(showSpline);
}

void toggleShowWire() {
    showWire = toggleValue(showWire);
}

void clearPoints() {
    userPoints.clear();
}

void updateIterationsCount(int cnt) {
    if (cnt >= 0 && cnt <= MAX_IT_CNT) {
        iterationsCount = cnt;
    }
}

void toggleDrawOnlyLastIteration() {
    drawOnlyLastIteration = toggleValue(drawOnlyLastIteration);
}

void toggleClosedSpline() {
    closedSpline = toggleValue(closedSpline);
}

double getPointsDiff(Point p1, Point p2) {
    return fabs(p1.x - p2.x) + fabs(p1.y - p2.y);
}

int getNearbyPoint(Point point, double epsilon) {
    for (int i = 0; i < userPoints.size(); i++) {
        Point p = userPoints[i];
        if (getPointsDiff(point, p) < epsilon) {
            return i;
        }
    }
    return -1;
}

Point getMousePosition(int x, int y) {

    double xPos = ((float) x) / ((float) (getWindowWidth() - 1));
    double yPos = 1.0f - ((float) y) / ((float) (getWindowHeight() - 1));

    return {xPos, yPos};
}

void mouseMotionFn(int x, int y) {
    if (dragging) {
        userPoints[draggingId] = getMousePosition(x, y);
        glutPostRedisplay();
    }
}

void mouseFn(int button, int state, int x, int y) {

    if (state == GLUT_UP) {
        dragging = 0;
        draggingId = -1;
    }

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

        Point newPoint = getMousePosition(x, y);
        int pointId = getNearbyPoint(newPoint, 0.01);

        if (pointId >= 0) {
            userPoints[pointId] = newPoint;
            draggingId = pointId;
            dragging = 1;
        } else if (allowNewPoints) {
            userPoints.push_back(newPoint);
        }
        glutPostRedisplay();
    }
}

Point getMidPoint(Point p1, Point p2) {

    double x = (p1.x + p2.x) / 2.0;
    double y = (p1.y + p2.y) / 2.0;

    return {x, y};
}

std::vector<Point> getMidPoints(std::vector<Point> pts) {
    if (pts.size() == 1) {
        return pts;
    }

    std::vector<Point> tmpPts;
    for (int i = 0; i < pts.size() - 1; i++) {
        tmpPts.push_back(getMidPoint(pts[i], pts[i + 1]));
    }

    std::vector<Point> midPoints = getMidPoints(tmpPts);
    std::vector<Point> newPoints;
    newPoints.push_back(pts[0]);

    for (Point pt: midPoints) {
        newPoints.push_back(pt);
    }

    newPoints.push_back(pts[pts.size() - 1]);
    return newPoints;
}

std::vector<Point> getInnerPoints(std::vector<Point> initialPoints) {

    std::vector<Point> splinePoints;

    for (int i = 0; i < initialPoints.size() - splineDegree; i += splineDegree) {
        std::vector<Point> innerPoints;

        for (int j = 0; j <= splineDegree; j++) {
            innerPoints.push_back(initialPoints[i + j]);
        }

        innerPoints = getMidPoints(innerPoints);

        if (i > 0) {
            splinePoints.insert(splinePoints.end(), innerPoints.begin() + 1, innerPoints.end());
        } else {
            splinePoints.insert(splinePoints.end(), innerPoints.begin(), innerPoints.end());
        }
    }

    return splinePoints;
}


std::vector<Point> getSplinePoints(std::vector<Point> points) {

    if (points.size() < 3) {
        std::vector<Point> empty;
        return empty;
    }

    std::vector<Point> splinePoints = points;

    for (int i = 0; i < iterationsCount; i++) {

        splinePoints = getInnerPoints(splinePoints);

        if (showWorkingLines && (!drawOnlyLastIteration || i == iterationsCount - 1)) {
            drawLine(splinePoints, THIN_LINE, getLineColor(i));
            drawPoints(splinePoints, 6, getPointColor(i));
        }
    }

    return splinePoints;
}

std::vector<Point> getInterpolatingSplineWorkPoints(Point p1, Point p2, Point p1d, Point p2d) {

    std::vector<Point> pts;
    if (useVariedIntervals) {
        double d = sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
        pts.push_back(p1);
        pts.push_back({p1.x + p1d.x * d / 3.0, p1.y + p1d.y * d / 3.0});
        pts.push_back({p2.x - p2d.x * d / 3.0, p2.y - p2d.y * d / 3.0});
    } else {
        pts.push_back(p1);
        pts.push_back({p1.x + p1d.x / 3.0, p1.y + p1d.y / 3.0});
        pts.push_back({p2.x - p2d.x / 3.0, p2.y - p2d.y / 3.0});
    }
    return pts;
}

Point getDerivative(Point pPrev, Point pCurr, Point pNext) {
    if (useVariedIntervals) {
        double dPrev = sqrt(pow(pPrev.x - pCurr.x, 2) + pow(pPrev.y - pCurr.y, 2));
        double d = sqrt(pow(pCurr.x - pNext.x, 2) + pow(pCurr.y - pNext.y, 2));

        double a = (d - dPrev) / (d * dPrev);
        double b = dPrev / (d * (d + dPrev));
        double c = d / (dPrev * (d + dPrev));

        return {
                a * pCurr.x + b * pNext.x - c * pPrev.x,
                a * pCurr.y + b * pNext.y - c * pPrev.y,
        };
    }

    return {
            (pNext.x - pPrev.x) / 2.0,
            (pNext.y - pPrev.y) / 2.0,
    };
}

std::vector<Point> getInterpolationPoints(std::vector<Point> pts) {

    if (!userPoints.empty() && userPoints.size() >= 3) {
        std::vector<Point> newPoints;
        int i = 0;
        long n = pts.size() - 1;
        long l = n;

        if (!closedSpline) {
            i++;
            l--;l--;
            Point p0 = pts[0];
            Point p1 = pts[1];
            Point p1d = getDerivative(p0, p1, pts[2]);

            std::vector<Point> start;

            start.push_back(p0);
            start.push_back({p1.x - 0.5 * p1d.x, p1.y - 0.5 * p1d.y});
            start.push_back(p1);

            std::vector<Point> updated = changeSplineDegree(2, 3, start);
            newPoints.insert(newPoints.end(), updated.begin(), updated.end() - 1);
        }

        for (; i <= l; i++) {
            Point p1 = pts[i];
            Point p1d;
            Point p2;
            Point p2d;

            if (i == n) {
                p2 = pts[0];
            } else {
                p2 = pts[i + 1];
            }

            if (i == 0) {
                p1d = getDerivative(pts[n], p1, p2);
            } else {
                p1d = getDerivative(pts[i - 1], p1, p2);
            }

            if (i >= (n - 1)) {
                p2d = getDerivative(p1, p2, pts[i - (n - 1)]);
            } else {
                p2d = getDerivative(p1, p2, pts[i + 2]);
            };

            std::vector<Point> workPoints = getInterpolatingSplineWorkPoints(p1, p2, p1d, p2d);
            newPoints.insert(newPoints.end(), workPoints.begin(), workPoints.end());
        }

        if (closedSpline) {
            newPoints.push_back(userPoints[0]);
        } else {
            Point p0 = pts[n];
            Point p1 = pts[n-1];
            Point p1d = getDerivative(p0, p1, pts[n-2]);

            std::vector<Point> end;

            end.push_back(p1);
            end.push_back({p1.x - 0.5 * p1d.x, p1.y - 0.5 * p1d.y});
            end.push_back(p0);

            std::vector<Point> updated = changeSplineDegree(2, 3, end);
            newPoints.insert(newPoints.end(), updated.begin(), updated.end());
        }
        return newPoints;
    }

    return pts;
};

std::vector<Point> getWirePoints() {
    std::vector<Point> pts = userPoints;
    if (closedSpline) {
        pts.push_back(userPoints[0]);
    }
    return pts;
};

void render() {
    glClear(GL_COLOR_BUFFER_BIT);

    if (!allowNewPoints) {
        if (showWire) {
            drawLine(getWirePoints(), THIN_LINE, getDefaultWireLineColor());
        }

        std::vector<Point> points = getInterpolationPoints(userPoints);
        if (showInterpolationLines) {
            drawLine(points, THIN_LINE, {0.5f, 0.8f, 0.2f});
            drawPoints(points, 6, {0.5f, 0.1f, 0.9f});
        }
        std::vector<Point> splinePoints = getSplinePoints(points);
        if (showSpline) {
            drawLine(splinePoints, THICK_LINE, getDefaultSplineColor());
        }
    }

    if (allowNewPoints || showWire) {
        drawPoints(userPoints, 12, getDefaultUserPointColor());
    }

    glFlush();
}

void keyboardFn(unsigned char key, int x, int y) {
    switch (key) {
        case 'v':
            toggleUseVariedIntervals();
            glutPostRedisplay();
            break;
        case 'i':
            toggleShowInterpolationLines();
            glutPostRedisplay();
            break;
        case 's':
            toggleShowSpline();
            glutPostRedisplay();
            break;
        case 'h':
            toggleShowWorkingLines();
            glutPostRedisplay();
            break;
        case 'w':
            toggleShowWire();
            glutPostRedisplay();
            break;
        case 'c':
            clearPoints();
            allowNewPoints = 1;
            glutPostRedisplay();
            break;
        case 'k':
            updateIterationsCount(iterationsCount + 1);
            glutPostRedisplay();
            break;
        case 'j':
            updateIterationsCount(iterationsCount - 1);
            glutPostRedisplay();
            break;
        case 'l':
            toggleDrawOnlyLastIteration();
            glutPostRedisplay();
            break;
        case 'o':
            toggleClosedSpline();
            glutPostRedisplay();
            break;
        case SPACE:
        case ENTER:
            allowNewPoints = 0;
            glutPostRedisplay();
            break;
        case 'x':
        case 'q':
        case ESC:
            exit(0);
    }
}

int main(int argc, char **argv) {
    render(argc, argv, "Interpolating spline", render, keyboardFn, mouseFn, mouseMotionFn);
    return 0;
}
