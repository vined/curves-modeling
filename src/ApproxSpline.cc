#include <stdlib.h>
#include <GL/glut.h>
#include <stdio.h>
#include <vector>
#include <math.h>

#include "ApproxSpline.h"
#include "utils/Points.h"
#include "utils/Renderer.h"
#include "utils/Colors.h"


#define THICK_LINE 3
#define THIN_LINE 1
#define IT_CNT 5
#define MAX_IT_CNT 10

const int ESC = 27;


// TODO points dragging


std::vector<Point> userPoints;

int splineDegree = 2;
int showWorkingLines = 1;
int showSpline = 1;
int showWire = 1;
int iterationsCount = IT_CNT;
int drawOnlyLastIteration = 0;
int closedSpline = 1;

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
        } else {
            userPoints.push_back(newPoint);
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

Point getThirdPoint(Point p1, Point p2) {

    double x = (2.0 * p1.x / 3.0) + (p2.x / 3.0);
    double y = (2.0 * p1.y / 3.0) + (p2.y / 3.0);

    return {x, y};
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

std::vector<Point> getApproximation3DegreePoints(std::vector<Point> userPoints) {

    std::vector<Point> result;
    Point firstPoint;
    Point lastPoint;

    result.push_back(userPoints[0]);

    for (int i = 0; i < userPoints.size() - 1; i++) {
        Point p1 = userPoints[i];
        Point p2 = userPoints[i + 1];

        firstPoint = getThirdPoint(p1, p2);

        if (i > 0) {
            result.push_back(getMidPoint(lastPoint, firstPoint));
        }

        lastPoint = getThirdPoint(p2, p1);

        result.push_back(firstPoint);
        result.push_back(lastPoint);
    }

    if (closedSpline) {
        lastPoint = getMidPoint(lastPoint, result[1]);
        result[0] = lastPoint;
        result.push_back(lastPoint);
    }

    return result;
}

std::vector<Point> getApproximation2DegreePoints(std::vector<Point> userPoints) {

    std::vector<Point> result;

    for (int i = 0; i < userPoints.size() - 1; i++) {
        result.push_back(
                getMidPoint(userPoints[i], userPoints[i + 1])
        );
        result.push_back(userPoints[i + 1]);
    }

    if (closedSpline) {
        result.push_back(result[0]);
    }

    return result;
}

std::vector<Point> getSplinePoints(std::vector<Point> userPoints) {

    if (userPoints.size() < 3) {
        std::vector<Point> empty;
        return empty;
    }

    std::vector<Point> splinePoints;
    if (splineDegree == 3) {
        splinePoints = getApproximation3DegreePoints(userPoints);
    } else {
        splinePoints = getApproximation2DegreePoints(userPoints);
    }

    for (int i = 0; i < iterationsCount; i++) {

        splinePoints = getInnerPoints(splinePoints);

        if (showWorkingLines && (!drawOnlyLastIteration || i == iterationsCount - 1)) {
            drawLine(splinePoints, THIN_LINE, getLineColor(i));
            drawPoints(splinePoints, 6, getPointColor(i));
        }
    }

    return splinePoints;
}

std::vector<Point> getUserPoints() {

    std::vector<Point> result = userPoints;

    if (!userPoints.empty() && userPoints.size() >= 3) {
        if (closedSpline) {
            result.push_back(userPoints[0]);
        }
    }

    return result;
};

void render() {
    glClear(GL_COLOR_BUFFER_BIT);

    std::vector<Point> points = getUserPoints();
    if (showWire) {
        drawLine(points, THIN_LINE, getDefaultWireLineColor());
    }

    std::vector<Point> splinePoints = getSplinePoints(points);
    if (showSpline) {
        drawLine(splinePoints, THICK_LINE, getDefaultSplineColor());
    }

    drawPoints(points, 12, getDefaultUserPointColor());

    glFlush();
}

void keyboardFn(unsigned char key, int x, int y) {
    if (key >= '2' && key <= '3') {
        splineDegree = key - '0';
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
        case 'o':
            toggleClosedSpline();
            glutPostRedisplay();
            break;
        case 'x':
        case 'q':
        case ESC:
            exit(0);
    }
}

int main(int argc, char **argv) {
    render(argc, argv, "Approximating spline", render, keyboardFn, mouseFn, mouseMotionFn);
    return 0;
}
