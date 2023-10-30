#include "BooleanOperation.hh"
#include <vector>
#include <algorithm>


double polygon_directed_area(PolyLine* polygon) {
    const std::vector<PolyLine::Point>& vertices = polygon->points();
    size_t n_vertices = polygon->n_vertices();
    double area = 0.0;
    for (size_t i = 0; i < n_vertices; i++) {
        const PolyLine::Point& p1 = vertices[i];
        const PolyLine::Point& p2 = vertices[(i + 1) % n_vertices];
        area += p1[0] * p2[1] - p2[0] * p1[1];
    }
    return 0.5 * area;
}


void ensure_counter_clockwise_polygon(PolyLine* polygon) {
    if (polygon_directed_area(polygon) < 0.0) { // vertices order is clockwise
        std::vector<PolyLine::Point>& vertices = polygon->points();
        std::reverse(vertices.begin(), vertices.end()); // clockwise to counter-clockwise
    }
}