#include <stdlib.h>
#include <GL/glut.h>
#include <stdio.h>
#include <vector>
#include <math.h>
#include <algorithm>

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

const double alpha = 0.5;

std::vector<Point> userPoints;

double divisionCoef = 3.8;
int showWorkingLines = 0;
int showSpline = 1;
int showWire = 1;
int iterationsCount = IT_CNT;
int drawOnlyLastIteration = 0;
int closedSpline = 1;

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

    double a = (divisionCoef - 1.0) / divisionCoef;
    double b = 1.0 / divisionCoef;

    return {
            a * p1.x + b * p2.x,
            a * p1.y + b * p2.y,
    };
}

std::vector<Point> getInnerPoints(std::vector<Point> initialPoints) {

    std::vector<Point> newPoints;
    long n = initialPoints.size() - 1;

    if (!closedSpline) {
        newPoints.push_back(initialPoints[0]);
    }

    for (int i = 0; i <= n - 1 ; i++) {
        Point p1 = initialPoints[i];
        Point p2 = initialPoints[i+1];

        newPoints.push_back(getMidPoint(p1, p2));
        newPoints.push_back(getMidPoint(p2, p1));
    }

    if (closedSpline) {
        newPoints.push_back(newPoints[0]);
    } else {
        newPoints.push_back(initialPoints[n]);
    }

    return newPoints;
}

Point getPhantomPoint(Point p1, Point p2) {
//    double t = 0.25;
    double t = 1;
    return {
            (2 * p1.x - p2.x) * t + p1.x * (1 - t),
            (2 * p1.y - p2.y) * t + p1.y * (1 - t)
    };
}

std::vector<Point> addPhantomPoints(std::vector<Point> pts) {
    if (!closedSpline) {
        long n = pts.size() - 1;
        std::vector<Point> withPhantoms;

        withPhantoms.push_back(getPhantomPoint(pts[0], pts[1]));
        withPhantoms.insert(withPhantoms.end(), pts.begin()+1, pts.end()-1);
        withPhantoms.push_back(getPhantomPoint(pts[n], pts[n-1]));
        return withPhantoms;
    }
    return pts;
};


std::vector<Point> getSplinePoints(std::vector<Point> points) {

    if (points.size() < 3) {
        std::vector<Point> empty;
        return empty;
    }

    long n = points.size() - 1;
    std::vector<Point> splinePoints = points;

    for (int i = 0; i < iterationsCount; i++) {

        std::vector<Point> newSplinePoints = getInnerPoints(addPhantomPoints(splinePoints));

        if (showWorkingLines && (!drawOnlyLastIteration || i == iterationsCount - 1)) {
            drawLine(newSplinePoints, THIN_LINE, getLineColor(i));
            drawPoints(newSplinePoints, 6, getPointColor(i));
        }

        if (!closedSpline) {
            splinePoints.clear();
            splinePoints.insert(splinePoints.begin(), newSplinePoints.begin() + 1, newSplinePoints.end() - 1);

            splinePoints[0] = points[0];
            splinePoints[splinePoints.size() - 1] = points[n];
        } else {
            splinePoints = newSplinePoints;
        }
    }

    return splinePoints;
}

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
        std::vector<Point> points = getWirePoints();
        if (showWire) {
            drawLine(points, THIN_LINE, getDefaultWireLineColor());
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
    if (key >= '3' && key <= '9') {
        divisionCoef = (double) key - '0';
        glutPostRedisplay();
        return;
    }

    switch (key) {
        case '2':
            divisionCoef = 3.8;
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
    render(argc, argv, "Chaikin spline", render, keyboardFn, mouseFn, mouseMotionFn);
    return 0;
}

