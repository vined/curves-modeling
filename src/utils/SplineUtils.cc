

#include "SplineUtils.h"

std::vector<Point> changeSplineDegree(int oldDegree, int newDegree, std::vector<Point> points) {

    if (points.size() == oldDegree + 1) {
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
            return newPoints;
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
            return newPoints;
        }
    }
    return points;
}
