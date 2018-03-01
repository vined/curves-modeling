#include "Spline.h"
#include <stdlib.h>
#include <GL/glut.h>
#include <stdio.h>
#include <vector>
#include <math.h>


struct Point {
    double x;
    double y;
};

//std::vector<Point> {
//    int size;
//    struct Point *points;
//};

//#define maxPoints 50
//struct Point pointsList[maxPoints];
//std::vector<Point> userPoints = {0, pointsList};
std::vector<Point> userPoints;

int showWorkingLines = 1;
int showSpline = 1;

int windowHeight;
int windowWidth;

int toggleValue(int val) {
    if (val == 0) {
        return 1;
    } else {
        return 0;
    }
}

void toggleShowWorkingLines(void) {
    showWorkingLines = toggleValue(showWorkingLines);
}

void toggleShowSpline(void) {
    showSpline = toggleValue(showSpline);
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

void drawLine(std::vector<Point> pts) {
    if (pts.size() > 1) {
        glLineWidth(1);
        glColor3f(0.0f, 0.0f, 0.8f);

        glBegin(GL_LINE_STRIP);
        for (int i = 0; i < pts.size(); i++) {
            glVertex2f(pts[i].x, pts[i].y);
        }
        glEnd();
    }
}

void drawPoints(std::vector<Point> pts, int pointSize, float red, float green, float blue) {
    glPointSize(pointSize);
    glColor3f(red, green, blue);

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
        printf("x: %f, y: %f\n", newPoints[i+1].x, newPoints[i+1].y);
    }

    newPoints.push_back(pts[pts.size() - 1]);

    printf("last x: %f, y: %f\n", newPoints[newPoints.size() - 1].x, newPoints[newPoints.size() - 1].y);
    return newPoints;
}

void drawHelpers(std::vector<Point> userPoints) {
    if (userPoints.size() < 3) {
        return;
    }

    std::vector<Point> midPoints = userPoints;

    for (int i = 0; i < 5; i++) {
        midPoints = getMidPoints(midPoints);
        drawLine(midPoints);
        drawPoints(midPoints, 6, 1.0f, 0.0f, 0.8f);
    }
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT);

    drawLine(userPoints);
    drawPoints(userPoints, 6, 0.0f, 0.0f, 0.0f);

    if (showWorkingLines) {
        drawHelpers(userPoints);
    }

    glFlush();
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
        case 'w':
            toggleShowWorkingLines();
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

    glutDisplayFunc(render);
    glutReshapeFunc(resizeWindow);
    glutKeyboardFunc(keyboardFunc);
    glutMouseFunc(mouseFunc);
    glutMainLoop();

    return 0;
}
