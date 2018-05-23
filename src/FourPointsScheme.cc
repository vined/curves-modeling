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

int splineDegree = 2;
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

Point getGhostPoint(Point prev, Point curr, Point next, Point next2) {
    double a = - 1.0 / 16.0;
    double b = 9.0 / 16.0;

    return {
            a * prev.x + b * curr.x + b * next.x + a * next2.x,
            a * prev.y + b * curr.y + b * next.y + a * next2.y,
    };
}

std::vector<Point> getInnerPoints(std::vector<Point> pts) {

    std::vector<Point> newPoints;
    long n = pts.size() - 1;

    for (int i = 1; i <= n - 2 ; i++) {
        Point prev = pts[i-1];
        Point curr = pts[i];
        Point next = pts[i+1];
        Point next2 = pts[i+2];

        newPoints.push_back(curr);
        newPoints.push_back(getGhostPoint(prev, curr, next, next2));
    }

    newPoints.push_back(newPoints[0]);
    return newPoints;
}

std::vector<Point> addPhantomPoints(std::vector<Point> pts) {
    long n = pts.size() - 1;
    std::vector<Point> withPhantoms;

    withPhantoms.push_back(pts[n]);
    withPhantoms.insert(withPhantoms.end(), pts.begin(), pts.end());
    withPhantoms.push_back(pts[1]);
    return withPhantoms;
};

std::vector<Point> getSplinePoints(std::vector<Point> points) {

    if (points.size() < 3) {
        std::vector<Point> empty;
        return empty;
    }

    std::vector<Point> result = points;

    for (int i = 0; i < iterationsCount; i++) {
        result = getInnerPoints(addPhantomPoints(result));
    }

    return result;
}

std::vector<Point> getWirePoints() {
    std::vector<Point> pts = userPoints;
    pts.push_back(userPoints[0]);
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
    switch (key) {
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
    render(argc, argv, "Four points scheme interpolating spline", render, keyboardFn, mouseFn, mouseMotionFn);
    return 0;
}

