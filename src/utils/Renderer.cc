#include <GL/glut.h>

#include "Renderer.h"

int windowHeight;
int windowWidth;

void initRendering() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST); // Make round points, not square points
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST); // Antialias the lines
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void resizeWindowFn(int w, int h) {
    windowHeight = (h > 1) ? h : 2;
    windowWidth = (w > 1) ? w : 2;
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0f, 1.0f, 0.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


void render(
        int argc,
        char **argv,
        const char *title,
        void (*renderFn)(),
        void (*keyboardFn)(unsigned char, int, int),
        void (*mouseFn)(int, int, int, int)
) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(1200, 800);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(title);
    initRendering();

    glutDisplayFunc(renderFn);
    glutReshapeFunc(resizeWindowFn);
    glutKeyboardFunc(keyboardFn);
    glutMouseFunc(mouseFn);
    glutMainLoop();
}

int getWindowHeight() {
    return windowHeight;
}

int getWindowWidth() {
    return windowWidth;
}
