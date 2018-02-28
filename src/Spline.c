#include "Spline.h"
#include <stdlib.h>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>


struct Point {
    double x;
    double y;
};

struct Points {
    int size;
    struct Point *points;
};

#define maxPoints 50
struct Point pointsList[maxPoints];
struct Points userPoints = {0, pointsList};

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
    userPoints.size = 0;
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
    if (userPoints.size < maxPoints) {
        struct Point p = {x, y};
        userPoints.points[userPoints.size] = p;
        userPoints.size++;
    }
}

void render(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    drawLine(userPoints);
    drawPoints(userPoints, 6, 0.0f, 0.0f, 0.0f);

    if (showWorkingLines) {
        drawHelpers(userPoints);
    }

    glFlush();
}

void drawLine(struct Points pts) {
    if (pts.size > 1) {
        glLineWidth(1);
        glColor3f(0.0f, 0.0f, 0.8f);

        glBegin(GL_LINE_STRIP);
        for (int i = 0; i < pts.size; i++) {
            glVertex2f(pts.points[i].x, pts.points[i].y);
        }
        glEnd();
    }
}

void drawPoints(struct Points pts, int pointSize, float red, float green, float blue) {
    glPointSize(pointSize);
    glColor3f(red, green, blue);

    glBegin(GL_POINTS);
    for (int i = 0; i < pts.size; i++) {
        glVertex2f(pts.points[i].x, pts.points[i].y);
    }
    glEnd();
}

struct Points getMidPoints(struct Points pts) {
    int size = pts.size + 1;
    struct Point midPoints[size];

    struct Point first = {pts.points[0].x, pts.points[0].y};
    midPoints[0] = first;

    printf("size: %d\n", size);
    printf("last x: %f, y: %f\n", first.x, first.y);

    for (int i = 0; i < pts.size - 1; i++) {
        double x = (pts.points[i].x + pts.points[i + 1].x) / 2;
        double y = (pts.points[i].y + pts.points[i + 1].y) / 2;

        printf("x: %f, y: %f\n", x, y);
        struct Point p = {x, y};
        midPoints[i + 1] = p;
    }

    struct Point last = {pts.points[pts.size - 1].x, pts.points[pts.size - 1].y};
    midPoints[size - 1] = last;

    printf("last x: %f, y: %f\n", last.x, last.y);
    struct Points result = {size, midPoints};
    return result;
}

void drawHelpers(struct Points userPoints) {
    if (userPoints.size < 3) {
        return;
    }

    struct Points midPoints = userPoints;

    for (int i = 0; i < 5; i++) {
        midPoints = getMidPoints(midPoints);
        drawLine(midPoints);
        drawPoints(midPoints, 6, 1.0f, 0.0f, 0.8f);
    }
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
