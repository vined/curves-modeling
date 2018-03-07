#include "Spline.h"
#include <stdlib.h>
#include <GL/glut.h>
#include <stdio.h>
#include <vector>
#include <math.h>


#define THICK_LINE 3
#define THIN_LINE 1

struct Point {
    double x,y;
};

struct Color {
    float r,g,b;
};

struct Color USER_POINTS_COLOR = {0.0f, 0.0f, 0.0f};
struct Color WIRE_LINES_COLOR = {0.0f, 0.0f, 0.8f};
struct Color SPLINE_COLOR = {1.0f, 0.0f, 0.8f};

#define maxColors 5
struct Color pointsColors[maxColors];
struct Color linesColors[maxColors];


std::vector<Point> userPoints;

int showWorkingLines = 1;
int showSpline = 1;
int showWire = 1;

int windowHeight;
int windowWidth;

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

void addNewPoint(double x, double y) {
    struct Point p = {x, y};
    userPoints.push_back(p);
}

void mouseFunc(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        double xPos = ((float) x) / ((float) (windowWidth - 1));
        double yPos = 1.0f - ((float) y) / ((float) (windowHeight - 1));

        addNewPoint(xPos, yPos);
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

    printf("first x: %f, y: %f\n", newPoints[0].x, newPoints[0].y);

    for (int i = 0; i < midPoints.size(); i++) {
        newPoints.push_back(midPoints[i]);
        printf("x: %f, y: %f\n", newPoints[i].x, newPoints[i].y);
    }

    newPoints.push_back(pts[pts.size() - 1]);

    printf("last x: %f, y: %f\n", newPoints[newPoints.size() - 1].x, newPoints[newPoints.size() - 1].y);
    return newPoints;
}

std::vector<Point> getInnerPoints(std::vector<Point> initialPoints) {

    std::vector<Point> splinePoints;

    for (int i = 0; i < initialPoints.size() - 2; i+=2) {
        std::vector<Point> innerPoints;

        innerPoints.push_back(initialPoints[i]);
        innerPoints.push_back(initialPoints[i + 1]);
        innerPoints.push_back(initialPoints[i + 2]);

        innerPoints = getMidPoints(innerPoints);

        if (i > 0) {
            splinePoints.insert(splinePoints.end(), innerPoints.begin()+1, innerPoints.end());
        } else {
            splinePoints.insert(splinePoints.end(), innerPoints.begin(), innerPoints.end());
        }
    }


    return splinePoints;
}

std::vector<Point> getSplinePoints(std::vector<Point> userPoints) {
    if (userPoints.size() < 3) {
        std::vector<Point> empty;
        return empty;
    }

    std::vector<Point> splinePoints = userPoints;

    for (int i = 0; i < 5; i++) {

        splinePoints = getInnerPoints(splinePoints);

        if (showWorkingLines) {
            drawLine(splinePoints, THIN_LINE, linesColors[i]);
            drawPoints(splinePoints, 6, pointsColors[i]);
        }
    }

    return splinePoints;
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT);

    if (showWire) {
        drawLine(userPoints, THIN_LINE, WIRE_LINES_COLOR);
    }

    std::vector<Point> splinePoints = getSplinePoints(userPoints);
    if (showSpline) {
        drawLine(splinePoints, THICK_LINE, SPLINE_COLOR);
    }

    drawPoints(userPoints, 6, USER_POINTS_COLOR);

    glFlush();
}

void initColors() {
    pointsColors[0] = {0.5f, 0.8f, 0.2f};
    pointsColors[1] = {0.1f, 0.8f, 0.8f};
    pointsColors[2] = {0.5f, 0.1f, 0.2f};
    pointsColors[3] = {0.8f, 0.5f, 0.2f};
    pointsColors[4] = {0.5f, 0.1f, 0.9f};

    linesColors[0] = {0.5f, 0.8f, 0.2f};
    linesColors[1] = {0.1f, 0.8f, 0.8f};
    linesColors[2] = {0.5f, 0.1f, 0.2f};
    linesColors[3] = {0.8f, 0.5f, 0.2f};
    linesColors[4] = {0.5f, 0.1f, 0.9f};
}
void initRendering() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST); // Make round points, not square points
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST); // Antialias the lines
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void resizeWindow(int w, int h) {
    windowHeight = (h > 1) ? h : 2;
    windowWidth = (w > 1) ? w : 2;
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0f, 1.0f, 0.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void keyboardFunc(unsigned char key, int x, int y) {
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
        case 'x':
        case 27: // Escape key
            exit(0);
            break;
    }
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(1200, 800);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);
    initRendering();
    initColors();

    glutDisplayFunc(render);
    glutReshapeFunc(resizeWindow);
    glutKeyboardFunc(keyboardFunc);
    glutMouseFunc(mouseFunc);
    glutMainLoop();

    return 0;
}
