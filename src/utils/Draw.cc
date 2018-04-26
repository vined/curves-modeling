#include <GL/glut.h>
#include <vector>

#include "Draw.h"

void drawPoints(std::vector<Point> pts, int pointSize, struct Color color) {
    glPointSize(pointSize);
    glColor3f(color.r, color.g, color.b);

    glBegin(GL_POINTS);
    for (Point p : pts) {
        glVertex2f(p.x, p.y);
    }
    glEnd();
}

void drawLine(std::vector<Point> pts, int width, struct Color color) {
    if (pts.size() > 1) {
        glLineWidth(width);
        glColor3f(color.r, color.g, color.b);

        glBegin(GL_LINE_STRIP);
        for (Point p : pts) {
            glVertex2f(p.x, p.y);
        }
        glEnd();
    }
}

void drawRect(std::pair<Point, Point> r, int width, Color color) {
    std::vector<Point> rect;
    rect.push_back({r.first.x, r.first.y});
    rect.push_back({r.second.x, r.first.y});
    rect.push_back({r.second.x, r.second.y});
    rect.push_back({r.first.x, r.second.y});
    rect.push_back({r.first.x, r.first.y});
    drawLine(rect, width, color);
}

