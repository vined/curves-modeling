#include <stdlib.h>
#include <GL/glut.h>
#include <stdio.h>
#include <vector>
#include <math.h>

#include "utils/Colors.h"
#include "utils/Points.h"
#include "utils/Renderer.h"
#include "utils/Draw.h"


#define THICK_LINE 3
#define THIN_LINE 1
#define IT_CNT 5
#define MAX_IT_CNT 10

const int ESC = 27;
const int ENTER = 13;
const int SPACE = 32;


std::vector<Point> userPoints;
std::vector<long> zeroedDeltas;

int splineDegree = 3;
int showWorkingLines = 0;
int showSpline = 1;
int showWire = 1;
int iterationsCount = IT_CNT;
int drawOnlyLastIteration = 0;

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
    zeroedDeltas.clear();
}

void updateIterationsCount(int cnt) {
    if (cnt >= 0 && cnt <= MAX_IT_CNT) {
        iterationsCount = cnt;
    }
}

void toggleDrawOnlyLastIteration() {
    drawOnlyLastIteration = toggleValue(drawOnlyLastIteration);
}

std::vector<Point> getUserPoints() {

    std::vector<Point> pts = userPoints;

    if (!userPoints.empty() && userPoints.size() >= 3) {
        pts.push_back(userPoints[0]);
        return pts;
    }

    return pts;
};

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

double getDistanceToLine(Point lp1, Point lp2, Point p) {
    return fabs( (lp2.y - lp1.y) * p.x - (lp2.x - lp1.x) * p.y + lp2.x * lp1.y - lp2.y * lp1.x)
           / sqrt( pow(lp2.y - lp1.y, 2.0) + pow(lp2.x - lp1.x, 2.0) );
}

int getNearbyLine(Point p, double epsilon) {
    std::vector<Point> pts = getUserPoints();
    for (int i = 0; i < pts.size() - 1; i++) {
        Point lp1 = pts[i];
        Point lp2 = pts[i+1];
        if (getDistanceToLine(lp1, lp2, p) < epsilon) {
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

int getItemIdx(std::vector<long> vec, long item) {
    for (int i = 0; i < vec.size(); i++) {
        if (zeroedDeltas[i] == item) {
            return i;
        }
    }
    return -1;
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

        } else {

            int lineId = getNearbyLine(newPoint, 0.01);

            if (lineId >= 0) {
                int idx = getItemIdx(zeroedDeltas, lineId);
                if (idx >= 0) {
                    zeroedDeltas.erase(zeroedDeltas.begin() + idx);
                } else {
                    zeroedDeltas.push_back(lineId);
                }
            }
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

Point getVertex(Point prev_b2, Point next_b1, Point current, double prev_d, double next_d) {
    double len = prev_d + next_d;
    if (len > 0) {
        return {
                (next_d / len) * prev_b2.x + (prev_d / len) * next_b1.x,
                (next_d / len) * prev_b2.y + (prev_d / len) * next_b1.y,
        };
    } else {
        return current;
    }
}

std::vector<Point> getApproximation3DegreePoints(std::vector<Point> points, std::vector<double> deltas) {

    long n = points.size() - 1;
    std::vector<Point> result;

    for (int i = 0; i <= n - 1; i++) {

        Point p1 = points[i];
        Point p2 = points[i + 1];

        long prevIdx = i-1;
        long nextIdx = i+1;

        if (i == 0) {
            prevIdx = n-1;
        } else if (i == n-1) {
            nextIdx = 0;
        }

        double length = deltas[prevIdx] + deltas[i] + deltas[nextIdx];

        Point b1 = p1;
        Point b2 = p2;

        if (length > 0) {
            b1 = {
                    ((deltas[i] + deltas[nextIdx]) / length) * p1.x + (deltas[prevIdx] / length) * p2.x,
                    ((deltas[i] + deltas[nextIdx]) / length) * p1.y + (deltas[prevIdx] / length) * p2.y,
            };

            b2 = {
                    (deltas[nextIdx] / length) * p1.x + ((deltas[prevIdx] + deltas[i]) / length) * p2.x,
                    (deltas[nextIdx] / length) * p1.y + ((deltas[prevIdx] + deltas[i]) / length) * p2.y,
            };
        }

        if (i != 0) {
            result.push_back(getVertex(result[result.size()-1], b1, p1, deltas[prevIdx], deltas[i]));
        }

        result.push_back(b1);
        result.push_back(b2);
    }

    result.push_back(getVertex(result[result.size()-1], result[0], points[n], deltas[n-1], deltas[0]));

    std::vector<Point> final;
    final.push_back(result[result.size()-1]);
    final.insert(final.end(), result.begin(), result.end());
    return final;
}

Point getDiff(Point p1, Point p2) {
    return {p1.x - p2.x, p1.y - p2.y};
}

double getDelta(Point p1, Point p2) {
    Point diff = getDiff(p1, p2);
    return sqrt( pow(diff.x, 2.0) + pow(diff.y, 2.0) );
}

std::vector<double> getDeltas(std::vector<Point> pts) {
    std::vector<double> deltas;

    for (int i = 0; i < pts.size() - 1; i++) {
        if (getItemIdx(zeroedDeltas, i) >= 0) {
            deltas.push_back(0.0);
        } else {
            deltas.push_back(getDelta(pts[i], pts[i + 1]));
        }
    }

    return deltas;
}

std::vector<Point> getSplinePoints(std::vector<Point> points) {

    if (points.size() < 3) {
        std::vector<Point> empty;
        return empty;
    }

    std::vector<double> deltas = getDeltas(points);
    std::vector<Point> splinePoints = getApproximation3DegreePoints(points, deltas);

    for (int i = 0; i < iterationsCount; i++) {
        splinePoints = getInnerPoints(splinePoints);
    }

    return splinePoints;
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT);

    if (!allowNewPoints) {
        std::vector<Point> points = getUserPoints();;
        if (showWire) {
            drawLine(points, THIN_LINE, getDefaultWireLineColor());
        }

        if (!zeroedDeltas.empty()) {
            for (long idx : zeroedDeltas) {
                std::vector<Point> line;
                line.push_back(points[idx]);
                line.push_back(points[idx+1]);

                drawLine(line, THIN_LINE, getLineColor(0));
            }
        }

        std::vector<Point> splinePoints = getSplinePoints(points);
        if (showSpline) {
            if (showWorkingLines) {
                drawPoints(splinePoints, 6, getDefaultUserPointColor());
            }
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
    render(argc, argv, "Approximating cubic C2 spline", render, keyboardFn, mouseFn, mouseMotionFn);
    return 0;
}
