#include "Spline.h"
#include <stdlib.h>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>


struct Point {
    double x;
    double y;
};

#define maxPoints 50
struct Point points[maxPoints];
int pointsCnt = 0;

int showWorkingLines = 1;
int showSpline = 1;

int windowHeight;
int windowWidth;

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

void toggleShowWorkingLines(void) {
    showWorkingLines = toggleValue(showWorkingLines);
}

void toggleShowSpline(void) {
    showSpline = toggleValue(showSpline);
}

int toggleValue(int val) {
    if (val == 0) {
        return 1;
    } else {
        return 0;
    }
}

void clearPoints() {
    pointsCnt = 0;
}

void mouseFunc(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        double xPos = ((float) x) / ((float) (windowWidth - 1));
        double yPos = 1.0f - ((float) y) / ((float) (windowHeight - 1));

        addNewPoint(xPos, yPos);
        glutPostRedisplay();
    }
}

void addNewPoint(double x, double y) {
    if (pointsCnt < maxPoints) {
        struct Point p;
        p.x = x;
        p.y = y;
        points[pointsCnt] = p;
        pointsCnt++;
    }
}

void render(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    drawLine(points, pointsCnt);
    drawPoints(points, pointsCnt, 6, 0.0f, 0.0f, 0.0f);

    if (showWorkingLines) {
        drawHelpingPoints(points, pointsCnt);
    }

    glFlush();
}

void drawLine(struct Point pts[], int size) {
    if (size > 1) {
        glLineWidth(1);
        glColor3f(0.0f, 0.0f, 0.8f);

        glBegin(GL_LINE_STRIP);
        for (int i = 0; i < pointsCnt; i++) {
            glVertex2f(points[i].x, points[i].y);
        }
        glEnd();
    }
}

void drawHelpingPoints(struct Point pts[], int size) {
    if (size < 2) {
        return;
    }

    glPointSize(3);
    glColor3f(1.0f, 0.0f, 0.8f);

    glBegin(GL_POINTS);
    for (int i = 1; i < size; i++) {
        double x = (pts[i].x + pts[i-1].x) / 2;
        double y = (pts[i].y + pts[i-1].y) / 2;
        glVertex2f(x, y);
    }
    glEnd();
}

void drawPoints(struct Point pts[], int len, int pointSize, float red, float green, float blue) {
    glPointSize(pointSize);
    glColor3f(red, green, blue);

    glBegin(GL_POINTS);
    for (int i = 0; i < len; i++) {
        glVertex2f(pts[i].x, pts[i].y);
    }
    glEnd();
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
