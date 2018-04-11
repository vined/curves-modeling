#include <stdlib.h>
#include <GL/glut.h>
#include <stdio.h>
#include <vector>
#include <utility>
#include <math.h>

#include "Casteljau.h"
#include "utils/Points.h"
#include "utils/Renderer.h"
#include "utils/Colors.h"


#define THICK_LINE 3
#define THIN_LINE 1
#define IT_CNT 5
#define MAX_IT_CNT 10

const int ESC = 27;


std::vector<std::vector<Point>> userPoints;

int splineDegree = 2;
int showWorkingLines = 1;
int showSpline = 1;
int showWire = 1;
int iterationsCount = IT_CNT;
int drawOnlyLastIteration = 0;

int dragging = 0;
std::pair<long, long> dragPoint = {-1, -1};

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

double getPointsDiff(Point p1, Point p2) {
    return fabs(p1.x - p2.x) + fabs(p1.y - p2.y);
}

std::pair<long, long> getNearbyPoint(Point point, double epsilon) {
    for (int i = 0; i < userPoints.size(); i++) {
        for (int j = 0; j < userPoints[i].size(); j++) {
            Point p = userPoints[i][j];
            if (getPointsDiff(point, p) < epsilon) {
                return {i, j};
            }
        }
    }
    return {-1, -1};
}

Point getMousePosition(int x, int y) {

    double xPos = ((float) x) / ((float) (getWindowWidth() - 1));
    double yPos = 1.0f - ((float) y) / ((float) (getWindowHeight() - 1));

    return {xPos, yPos};
}

void mouseMotionFn(int x, int y) {
    if (dragging) {
        userPoints[dragPoint.first][dragPoint.second] = getMousePosition(x, y);
        glutPostRedisplay();
    }
}

void mouseFn(int button, int state, int x, int y) {

    if (state == GLUT_UP) {
        dragging = 0;
        dragPoint = {-1, -1};
    }

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

        Point newPoint = getMousePosition(x, y);
        std::pair<long, long> pt = getNearbyPoint(newPoint, 0.01);

        if (pt.first >= 0) {

            userPoints[pt.first][pt.second] = newPoint;
            dragPoint = pt;
            dragging = 1;

        } else {

            long last = userPoints.size() - 1;

            if (last >= 0 && userPoints[last].size() < splineDegree + 1) {
                userPoints[last].push_back(newPoint);
            } else {
                std::vector<Point> newCurve;
                newCurve.push_back(newPoint);
                userPoints.push_back(newCurve);
            }
        }
        glutPostRedisplay();
    }
}

void drawLine(std::vector<Point> pts, int width, struct Color color) {
    if (pts.size() > 1) {
        glLineWidth(width);
        glColor3f(color.r, color.g, color.b);

        glBegin(GL_LINE_STRIP);
        for (int i = 0; i < pts.size(); i++) {
            glVertex2f(pts[i].x, pts[i].y);
        }
        glEnd();
    }
}

void drawPoints(std::vector<Point> pts, int pointSize, struct Color color) {
    glPointSize(pointSize);
    glColor3f(color.r, color.g, color.b);

    glBegin(GL_POINTS);
    for (int i = 0; i < pts.size(); i++) {
        glVertex2f(pts[i].x, pts[i].y);
    }
    glEnd();
}

std::vector<Point> getMidPoints(std::vector<Point> pts) {
    if (pts.size() == 1) {
        return pts;
    }

    std::vector<Point> tmpPts;
    for (int i = 0; i < pts.size() - 1; i++) {
        double x = (pts[i].x + pts[i + 1].x) / 2.0;
        double y = (pts[i].y + pts[i + 1].y) / 2.0;

        struct Point p = {x, y};
        tmpPts.push_back(p);
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

    for (int i = 0; i < initialPoints.size() - splineDegree; i+=splineDegree) {
        std::vector<Point> innerPoints;

        for (int j = 0; j <= splineDegree; j++) {
            innerPoints.push_back(initialPoints[i + j]);
        }

        innerPoints = getMidPoints(innerPoints);

        if (i > 0) {
            splinePoints.insert(splinePoints.end(), innerPoints.begin()+1, innerPoints.end());
        } else {
            splinePoints.insert(splinePoints.end(), innerPoints.begin(), innerPoints.end());
        }
    }

    return splinePoints;
}

std::vector<Point> getSplinePoints(std::vector<Point> points) {

    if (points.size() < splineDegree + 1) {
        std::vector<Point> empty;
        return empty;
    }

    std::vector<Point> splinePoints = points;

    for (int i = 0; i < iterationsCount; i++) {

        splinePoints = getInnerPoints(splinePoints);

        if (showWorkingLines && (!drawOnlyLastIteration || i == iterationsCount - 1) ) {
            drawLine(splinePoints, THIN_LINE, getLineColor(i));
            drawPoints(splinePoints, 6, getPointColor(i));
        }
    }

    return splinePoints;
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT);

    for (std::vector<Point> curve : userPoints) {
        if (showWire) {
            drawLine(curve, THIN_LINE, getDefaultWireLineColor());
        }

        std::vector<Point> splinePoints = getSplinePoints(curve);
        if (showSpline) {
            drawLine(splinePoints, THICK_LINE, getDefaultSplineColor());
        }

        if (showWire) {
            drawPoints(curve, 12, getDefaultUserPointColor());
        }
    }

    glFlush();
}

void keyboardFn(unsigned char key, int x, int y) {
    if (key >= '2' && key <= '3') {
        splineDegree = key - '0';
        clearPoints();
        glutPostRedisplay();
        return;
    }

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
        case 'x':
        case 'q':
        case ESC:
            exit(0);
    }
}

int main(int argc, char **argv) {
    render(argc, argv, "Casteljau", render, keyboardFn, mouseFn, mouseMotionFn);
    return 0;
}

