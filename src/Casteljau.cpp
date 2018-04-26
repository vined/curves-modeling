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
const double EPSILON = 0.0015;

std::vector<std::vector<Point>> userPoints;

int splineDegree = 2;
int showWorkingLines = 0;
int showSpline = 1;
int showWire = 1;
int showCollisionRects = 1;
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

void toggleShowCollisionRects() {
    showCollisionRects = toggleValue(showCollisionRects);
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
        std::pair<long, long> pt = getNearbyPoint(newPoint, 0.02);

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
    for (Point p : pts) {
        glVertex2f(p.x, p.y);
    }
    glEnd();
}

Point getMidPoint(Point p1, Point p2) {
    return {(p1.x + p2.x) / 2.0, (p1.y + p2.y) / 2.0};
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

    if (points.size() < splineDegree + 1) {
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

std::pair<Point, Point> getCurveMaxArea(std::vector<Point> pts) {
    double minX = pts[0].x;
    double minY = pts[0].y;

    double maxX = pts[0].x;
    double maxY = pts[0].y;

    for (Point p : pts) {
        if (p.x > maxX) {
            maxX = p.x;
        } else if (p.x < minX) {
            minX = p.x;
        }

        if (p.y > maxY) {
            maxY = p.y;
        } else if (p.y < minY) {
            minY = p.y;
        }
    }

    return {{minX, minY},
            {maxX, maxY}};
}

int isLimit(std::pair<Point, Point> r) {
    if ((r.second.x - r.first.x < EPSILON) && (r.second.y - r.first.y < EPSILON)) {
        return 1;
    }
    return 0;
}

int isIn(double v, double from, double to) {

    if (v >= from && v <= to) {
        return 1;
    }

    return 0;
}

int isIntersecting(std::pair<Point, Point> r1, std::pair<Point, Point> r2) {

    int isInX = isIn(r1.first.x, r2.first.x, r2.second.x) || isIn(r1.second.x, r2.first.x, r2.second.x)
                || isIn(r2.first.x, r1.first.x, r1.second.x) || isIn(r2.second.x, r1.first.x, r1.second.x);

    int isInY = isIn(r1.first.y, r2.first.y, r2.second.y) || isIn(r1.second.y, r2.first.y, r2.second.y)
                || isIn(r2.first.y, r1.first.y, r1.second.y) || isIn(r2.second.y, r1.first.y, r1.second.y);

    if (isInX && isInY) {
        return 1;
    }

    return 0;
}

void drawRect(std::pair<Point, Point> r, Color color) {
    std::vector<Point> rect;
    rect.push_back({r.first.x, r.first.y});
    rect.push_back({r.second.x, r.first.y});
    rect.push_back({r.second.x, r.second.y});
    rect.push_back({r.first.x, r.second.y});
    rect.push_back({r.first.x, r.first.y});
    drawLine(rect, THIN_LINE, color);
}

std::pair<Point, Point> getIntersectionRect(std::pair<Point, Point> r1, std::pair<Point, Point> r2) {
    double xmin;
    double ymin;

    double xmax;
    double ymax;

    // Get min coordinate
    if (isIn(r1.first.x, r2.first.x, r2.second.x)) {
        xmin = r1.first.x;
    } else {
        xmin = r2.first.x;
    }

    if (isIn(r1.first.y, r2.first.y, r2.second.y)) {
        ymin = r1.first.y;
    } else {
        ymin = r2.first.y;
    }

    // Get max coordinate
    if (isIn(r1.second.x, r2.first.x, r2.second.x)) {
        xmax = r1.second.x;
    } else {
        xmax = r2.second.x;
    }

    if (isIn(r1.second.y, r2.first.y, r2.second.y)) {
        ymax = r1.second.y;
    } else {
        ymax = r2.second.y;
    }

    return {{xmin, ymin}, {xmax, ymax}};
}

Point getRectCenter(std::pair<Point, Point> r) {
    return getMidPoint(r.first, r.second);
}

Point getRectIntersectionMidPoint(std::pair<Point, Point> r1, std::pair<Point, Point> r2) {
    std::pair<Point, Point> intersectionRect = getIntersectionRect(r1, r2);
    return getRectCenter(intersectionRect);
}


std::vector<std::vector<Point>> splitToSubCurves(std::vector<Point> pts) {

    std::vector<std::vector<Point>> subCurves;
    std::vector<Point> curvePoints = getInnerPoints(pts);

    for (int i = 0; i < curvePoints.size() - splineDegree; i += splineDegree) {

        std::vector<Point> subCurve;

        for (int j = 0; j <= splineDegree; j++) {
            subCurve.push_back(curvePoints[i + j]);
        }

        subCurves.push_back(subCurve);
    }

    return subCurves;
}

std::vector<Point> getCurvesIntersections(std::vector<Point> pts1, std::vector<Point> pts2) {

    std::vector<Point> intersections;

    std::pair<Point, Point> r1 = getCurveMaxArea(pts1);
    std::pair<Point, Point> r2 = getCurveMaxArea(pts2);
    int intersects = isIntersecting(r1, r2);

    if (intersects) {

        if (showCollisionRects) {
            drawRect(r1, getDefaultWireLineColor());
            drawRect(r2, getDefaultWireLineColor());
        }

        if (!isLimit(r1) || !isLimit(r2)) {

            std::vector<std::vector<Point>> curves1 = splitToSubCurves(pts1);
            std::vector<std::vector<Point>> curves2 = splitToSubCurves(pts2);

            for (int i = 0; i < curves1.size(); i++) {
                for (int j = 0; j < curves2.size(); j++) {

                    std::vector<Point> newIntersections = getCurvesIntersections(curves1[i], curves2[j]);

                    if (!newIntersections.empty()) {
                        intersections.insert(intersections.end(), newIntersections.begin(), newIntersections.end());
                    }
                }
            }
        } else {
            printf("Limit reached!\n");
            intersections.push_back(getRectIntersectionMidPoint(r1, r2));
        }
    }

    return intersections;
}

void changeSplineDegree(int oldDegree, int newDegree) {
    std::vector<std::vector<Point>> newUserPoints;

    for (std::vector<Point> points : userPoints) {
        if (points.size() == oldDegree+1) {
            if (newDegree > oldDegree) {

                std::vector<Point> newPoints;
                newPoints.push_back(points[0]);

                for (int i = 1; i < points.size(); i++) {
                    double degree = ((double) i) / newDegree;
                    double x = degree * points[i - 1].x + (1 - degree) * points[i].x;
                    double y = degree * points[i - 1].y + (1 - degree) * points[i].y;

                    newPoints.push_back({x, y});
                }

                newPoints.push_back(points[points.size() - 1]);
                newUserPoints.push_back(newPoints);
            } else {

                std::vector<Point> newPoints;
                Point prev = points[0];
                newPoints.push_back(prev);

                for (int i = 1; i < points.size() - 2; i++) {
                    double degree = ((double) i) / oldDegree;
                    double x = (points[i].x - degree * prev.x) / (1 - degree);
                    double y = (points[i].y - degree * prev.y) / (1 - degree);

                    prev = {x, y};
                    newPoints.push_back(prev);
                }

                newPoints.push_back(points[points.size() - 1]);
                newUserPoints.push_back(newPoints);
            }
        } else {
            newUserPoints.push_back(points);
        }
    }

    userPoints.clear();
    userPoints.insert(userPoints.begin(), newUserPoints.begin(), newUserPoints.end());
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT);

    for (int i = 0; i < userPoints.size(); i++) {
        std::vector<Point> curve = userPoints[i];
        if (showWire) {
            drawLine(curve, THIN_LINE, getDefaultWireLineColor());
        }

        std::vector<Point> splinePoints = getSplinePoints(curve);
        if (showSpline) {
            drawLine(splinePoints, THICK_LINE, getSplineColor(i));
        }

        if (showWire) {
            drawPoints(curve, 12, getDefaultUserPointColor());
        }
    }

    std::vector<Point> collisions;

    for (int i = 0; i < userPoints.size(); i++) {
        for (int j = i + 1; j < userPoints.size(); j++) {

            if (userPoints[j].size() > splineDegree) {
                std::vector<Point> intersections = getCurvesIntersections(userPoints[i], userPoints[j]);

                if (!intersections.empty()) {
                    collisions.insert(collisions.end(), intersections.begin(), intersections.end());
                }
            }
        }
    }

    drawPoints(collisions, 14, getDefaultCollisionPointColor());

    glFlush();
}

void updateSplineDegree(int newDegree) {
    if (newDegree >= 2) {
        int oldDegree = splineDegree;
        splineDegree = newDegree;

        if (oldDegree != splineDegree) {
            changeSplineDegree(oldDegree, splineDegree);
        }
    }
}
void keyboardFn(unsigned char key, int x, int y) {
    if (key >= '2' && key <= '3') {
        updateSplineDegree(key - '0');
        glutPostRedisplay();
        return;
    }

    switch (key) {
        case 'i':
            updateSplineDegree(splineDegree - 1);
            glutPostRedisplay();
            break;
        case 'o':
            updateSplineDegree(splineDegree + 1);
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
        case 'r':
            toggleShowCollisionRects();
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

